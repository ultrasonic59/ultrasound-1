#include "ultrasound.h"

#include <algorithm>

#include <QDesktopServices>
#include <QMessageBox>
#include <QPrintDialog>
#include <QPrinterInfo>
#include <QPrintPreviewDialog>
#include <QShortcut>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QStandardPaths>
#include <QtWidgets>

#include "app/ultrasound_system_message_id.h"
#include "audio_device_setting.h"
#include "config.h"
#include "core_service/core_service.h"
#include "device_setting.h"
#include "nim_service/nim_service.h"
#include "profile.h"
#include "report_html.h"
#include "report_query.h"
#include "report_template.h"
#include "third_party/libyuv/include/libyuv.h"
#include "update_password.h"

ultrasound::ultrasound(QWidget *parent)
    : QMainWindow(parent), m_Sound(":/ultrasound/res/caller_ringing.wav")
{
    ui.setupUi(this);

	m_system_message_viewer = new SystemMessageViewer(this);

    m_Sound.setLoops(QSound::Infinite);

    OpenTemplateDB();

    ui.sex_combo_box->addItem(QString::fromLocal8Bit("男"));
    ui.sex_combo_box->addItem(QString::fromLocal8Bit("女"));

    ui.speaker_frame_background->stackUnder(ui.speaker_frame);
    ui.mic_frame_background->stackUnder(ui.mic_frame);

    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(UpdateTime()));
    m_timer->start(1000);

	m_show_system_message_statusbar_label = new ClickableLabel(ui.statusBar);
	m_show_system_message_statusbar_label->setCursor(Qt::PointingHandCursor);
	m_show_system_message_statusbar_label->setStyleSheet("font-weight: bold; color: red");
	m_show_system_message_statusbar_label->setText(QString::fromLocal8Bit("点击查看系统消息   "));
	ui.statusBar->addPermanentWidget(m_show_system_message_statusbar_label);
	connect(m_show_system_message_statusbar_label, SIGNAL(clicked()), this, SLOT(OpenSystemMessageWindow()));

    UpdateVChatUI(Idle, "");

    connect(ui.video_device_button, SIGNAL(clicked()), this, SLOT(OpenVideoDeviceSetting()));
    
    connect(ui.capture_button, SIGNAL(clicked()), this, SLOT(CaptureVideoData()));
    ui.capture_button->setShortcut(QKeySequence(Qt::Key_F4));
    
    connect(ui.delete_button, SIGNAL(clicked()), this, SLOT(DeleteCaptureImage()));

    connect(ui.new_button, SIGNAL(clicked()), this, SLOT(NewReport()));
    connect(ui.save_button, SIGNAL(clicked()), this, SLOT(SaveReport()));

    connect(ui.template_button, SIGNAL(clicked()), this, SLOT(OpenReportTemplate()));

    connect(ui.preview_button, SIGNAL(clicked()), this, SLOT(PreviewReport()));
    ui.preview_button->setShortcut(QKeySequence(Qt::Key_F5));

    connect(ui.print_button, SIGNAL(clicked()), this, SLOT(PrintReport()));
    ui.print_button->setShortcut(QKeySequence(Qt::Key_F6));

    connect(ui.audio_device_button, SIGNAL(clicked()), this, SLOT(OpenAudioDeviceSetting()));
    connect(ui.call_button, SIGNAL(clicked()), this, SLOT(ShowCallWidget()));
    connect(ui.end_button, SIGNAL(clicked()), this, SLOT(EndVChat()));

    connect(ui.query_button, SIGNAL(clicked()), this, SLOT(OpenReportQuery()));
    connect(ui.change_password_button, SIGNAL(clicked()), this, SLOT(OpenUpdatePassword()));
    connect(ui.profile_button, SIGNAL(clicked()), this, SLOT(OpenProfile()));

    connect(this, SIGNAL(UpdateCapturedVideoDataSignal(const QImage&)), 
        this, SLOT(UpdateCapturedVideoData(const QImage&)));
    connect(this, SIGNAL(ActivateDeviceErrorSignal(const QString&)),
        this, SLOT(ActivateDeviceError(const QString&)));
    connect(this, SIGNAL(UpdateVChatVideoDataSignal(const QImage&)), 
        this, SLOT(UpdateVChatVideoData(const QImage&)));
    connect(this, SIGNAL(StartAudioSignal()), this, SLOT(StartAudio()));
    connect(this, SIGNAL(UpdateRealtimeStatsInfoSignal(const QString&, const QString&)),
        this, SLOT(UpdateRealtimeStatsInfo(const QString&, const QString&)));

    connect(this, SIGNAL(VChatErrorSignal(const QString&)),
        this, SLOT(VChatError(const QString&)));
    connect(this, SIGNAL(VChatCalleeRejectedSignal()), this, SLOT(VChatCalleeRejected()));
    connect(this, SIGNAL(VChatReceiveInviteSignal(const QString&, const QString&)),
        this, SLOT(VChatReceiveInvite(const QString&, const QString&)));
    connect(this, SIGNAL(VChatHangupSignal()), this, SLOT(VChatHangup()));
    connect(this, SIGNAL(VChatBusyLineSignal()), this, SLOT(VChatBusyLine()));
    connect(this, SIGNAL(VChatConnectLogoutSignal()), this, SLOT(VChatConnectLogout()));
    connect(this, SIGNAL(UpdateAudioVolumeMeterSignal(const int, const int)), 
        this, SLOT(UpdateAudioVolumeMeter(const int, const int)));

    connect(this, SIGNAL(AddCaptureImageSignal(const QImage&)),
        this, SLOT(AddCaptureImage(const QImage&)));

    connect(ui.listWidget, SIGNAL(itemClicked(QListWidgetItem*)),
        this, SLOT(CaptureImageClicked(QListWidgetItem*)));
    connect(ui.listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
        this, SLOT(CaptureImageDoubleClicked(QListWidgetItem*)));

    connect(ui.check_point_combobox, SIGNAL(currentIndexChanged(const QString&)),
        this, SLOT(OnCheckPointIndexChanged(const QString&)));
    connect(ui.check_point_combobox->lineEdit(), SIGNAL(textEdited(const QString&)),
        this, SLOT(OnCheckPointEditTextChanged(const QString&)));

    connect(this, SIGNAL(KickOutSignal()), this, SLOT(KickOut()));
	connect(this, SIGNAL(receiveSuperiorMessage(const QString)), this, SLOT(onReceiveSuperiorMessage(const QString)));
	connect(this, SIGNAL(ReceiveReportConfirmedMessageSignal(const QString&, const QString&,
		const QString&, const QString&, const QString&, const int,
		const QString&)),
		this, SLOT(ReceiveReportConfirmedMessage(const QString&, const QString&,
			const QString&, const QString&, const QString&, const int,
			const QString&)));
}

void ultrasound::Initialize()
{    
    m_dshow = new DShow::DShowInput();
    m_dshow->Initialize();
    m_dshow->AddDShowInputEventHandler(this);

    std::vector<DShow::VideoDevice> devices;
    DShow::DShowInput::EnumVideoDevices(devices);
		
    if (devices.size() > 0)
    {
        DShow::VideoConfig config = Config::GetInstance()->GetVideoConfig(devices);
        ui.current_device_label->setText(QString::fromWCharArray(config.name.c_str()));
        m_dshow->SetCurrentVideoConfig(config);
        m_dshow->Activate();
    }

    m_user_path = QStandardPaths::writableLocation(
        QStandardPaths::AppLocalDataLocation);
    std::string current_uid = core::CoreService::GetInstance()->GetCurrentUid();
    m_user_path.append("/users/" + QString::fromStdString(current_uid));

    QDir dir = QDir::root();
    dir.mkpath(m_user_path);

    IOThreadWorker::StartParams params;
    params.handler = this;
    params.keep_log_files_month = 1;
    params.keep_report_files_month = 6;
    params.user_path = m_user_path;
    params.log_path = Config::GetInstance()->GetLogFilePath();
    m_io_worker.Start(params);

    NewReport();
}

void ultrasound::closeEvent(QCloseEvent *event) {
    if (m_is_kick_out) {
        QMessageBox::critical(this, QString::fromLocal8Bit("多点远程超声"),
            QString::fromLocal8Bit("当前账户已被踢出！"), QMessageBox::Ok);
        event->accept();
    }
    else {
        QMessageBox box(
            QMessageBox::Question,
            QString::fromLocal8Bit("多点远程超声"),
            QString::fromLocal8Bit("确定退出系统？"),
            QMessageBox::Yes | QMessageBox::No);

        box.setButtonText(QMessageBox::Yes, QString::fromLocal8Bit("确定"));
        box.setButtonText(QMessageBox::No, QString::fromLocal8Bit("取消"));

        int ret = box.exec();
        if (ret == QMessageBox::Yes) {
            event->accept();
        }
        else {
            event->ignore();
        }
    }
}

ultrasound::~ultrasound()
{
    m_io_worker.Stop();

    if (m_dshow) {
        m_dshow->Shutdown();
        delete m_dshow;
        m_dshow = nullptr;
    }

    if (m_timer) {
        delete m_timer;
        m_timer = NULL;
    }

    CloseTemplateDB();
}

void ultrasound::UpdateCapturedVideoData(const QImage& image) {
    int w = ui.capture_video_data_label->minimumWidth();
    int h = ui.capture_video_data_label->minimumHeight();

    ui.capture_video_data_label->setPixmap(
        QPixmap::fromImage(image).scaled(w, h, Qt::KeepAspectRatio));
}

void ultrasound::ActivateDeviceError(const QString& config) {
    QMessageBox::warning(this, QString::fromLocal8Bit("多点远程超声"), 
        QString::fromLocal8Bit("设备无法启动！设备信息：\n") + config, QMessageBox::Ok);
}

void ultrasound::UpdateVChatVideoData(const QImage& image) {
    int w = ui.vchat_video_data_label->minimumWidth();
    int h = ui.vchat_video_data_label->minimumHeight();

    ui.vchat_video_data_label->setPixmap(
        QPixmap::fromImage(image).scaled(w, h, Qt::KeepAspectRatio));
}

void ultrasound::OnCapturedVideoData(unsigned char *data, size_t size,
    uint32_t width, uint32_t height,
    DShow::VideoFormat format, uint64_t timestamp,
    YUVColorSpace color_space) {

    switch (format)
    {
    case DShow::VideoFormat::Any:
    case DShow::VideoFormat::Unknown:
        return;
    case DShow::VideoFormat::ARGB:
        return;
    case DShow::VideoFormat::XRGB:
    {
        QImage image(data, width, height, QImage::Format_RGBX8888);
        QImage& mirroredImage =  image.mirrored();

        emit UpdateCapturedVideoDataSignal(mirroredImage);

        SaveCapturedVideoData(mirroredImage);

        if (nim::NimService::GetInstance()->IsVChatConnected()) {

            std::string dst_data;
            int wxh = width * height;
            dst_data.resize(wxh * 4);
            uint8_t* dst_y = (uint8_t*)dst_data.c_str();
            uint8_t* dst_u = dst_y + (width * height);
            uint8_t* dst_v = dst_u + (width * height / 4);
            int dst_stride_y = width;
            int dst_stride_u = width / 2;
            int dst_stride_v = width / 2;

            libyuv::ARGBToI420(mirroredImage.bits(),
                width * 4,
                dst_y,
                dst_stride_y,
                dst_u,
                dst_stride_u,
                dst_v,
                dst_stride_v,
                width,
                height);

            int data_size = wxh * 3 / 2;
            nim::NimService::GetInstance()->CustomVideoDataI420(
                (uint8_t*)dst_data.c_str(), data_size,
                width, height);
        }

        return;
    }
    case DShow::VideoFormat::I420:
    {
        QImage image(width, height, QImage::Format_RGB32);

        uint8_t* src_y = data;
        int src_stride_y = width;
        uint8_t* src_u = data + (width * height);
        int src_stride_u = width / 2;
        uint8_t* src_v = src_u + (width * height / 4);
        int src_stride_v = width / 2;

        if (color_space == BT_709) {
            libyuv::H420ToARGB(src_y, src_stride_y,
                src_u, src_stride_u,
                src_v, src_stride_v,
                (uint8_t*)image.bits(), width * 4,
                width, height);
        }
        else {
             libyuv::I420ToARGB(src_y, src_stride_y,
                src_u, src_stride_u,
                src_v, src_stride_v,
                (uint8_t*)image.bits(), width * 4,
                width, height);
        }
            
        emit UpdateCapturedVideoDataSignal(image);

        SaveCapturedVideoData(image);

        if (nim::NimService::GetInstance()->IsVChatConnected()) {
            nim::NimService::GetInstance()->CustomVideoDataI420(
                data, size, width, height);
        }

        return;
    }
    case DShow::VideoFormat::NV12:
        return;
    case DShow::VideoFormat::YV12:
        return;
    case DShow::VideoFormat::Y800:
        return;
    case DShow::VideoFormat::YVYU:
        return;
    case DShow::VideoFormat::YUY2:
    {
        QImage image(width, height, QImage::Format_RGB32);

        libyuv::YUY2ToARGB(data, width * 2,
            (uint8_t*)image.bits(), image.bytesPerLine(), image.width(), image.height());

        emit UpdateCapturedVideoDataSignal(image);

        SaveCapturedVideoData(image);

        if (nim::NimService::GetInstance()->IsVChatConnected()) {
            std::string dst_data;
            int wxh = width * height;
            dst_data.resize(wxh * 4);
            uint8_t* dst_y = (uint8_t*)dst_data.c_str();
            uint8_t* dst_u = dst_y + (width * height);
            uint8_t* dst_v = dst_u + (width * height / 4);
            int dst_stride_y = width;
            int dst_stride_u = width / 2;
            int dst_stride_v = width / 2;

            libyuv::YUY2ToI420(data, width * 2,
                dst_y,
                dst_stride_y,
                dst_u,
                dst_stride_u,
                dst_v,
                dst_stride_v,
                width,
                height);

            int data_size = wxh * 3 / 2;
            nim::NimService::GetInstance()->CustomVideoDataI420(
                (uint8_t*)dst_data.c_str(), data_size,
                width, height);
        }

        return;
    }
    case DShow::VideoFormat::UYVY:
    {
        QImage image(width, height, QImage::Format_RGB32);

        libyuv::UYVYToARGB(data, width * 2,
            (uint8_t*)image.bits(), image.bytesPerLine(), image.width(), image.height());

        emit UpdateCapturedVideoDataSignal(image);

        SaveCapturedVideoData(image);

        if (nim::NimService::GetInstance()->IsVChatConnected()) {
            std::string dst_data;
            int wxh = width * height;
            dst_data.resize(wxh * 4);
            uint8_t* dst_y = (uint8_t*)dst_data.c_str();
            uint8_t* dst_u = dst_y + (width * height);
            uint8_t* dst_v = dst_u + (width * height / 4);
            int dst_stride_y = width;
            int dst_stride_u = width / 2;
            int dst_stride_v = width / 2;

            libyuv::UYVYToI420(data, width * 2,
                dst_y,
                dst_stride_y,
                dst_u,
                dst_stride_u,
                dst_v,
                dst_stride_v,
                width,
                height);

            int data_size = wxh * 3 / 2;
            nim::NimService::GetInstance()->CustomVideoDataI420(
                (uint8_t*)dst_data.c_str(), data_size,
                width, height);
        }

        return;
    }
    case DShow::VideoFormat::HDYC:
    case DShow::VideoFormat::MJPEG:
    case DShow::VideoFormat::H264:
        return;
    default:
        break;
    }
}

void ultrasound::OnActivateDeviceError(DShow::VideoConfig& config) {
    QString info;
    info.append(QString::fromWCharArray(config.name.c_str()));
    info.append("\n");
    info.append(DeviceSetting::GetFormatName(config.format));
    info.append(",");
    info.append(DeviceSetting::GetFormatName(config.internalFormat));
    info.append(",");
    info.append(QString::number(config.cx));
    info.append("x");
    info.append(QString::number(config.cy));
    info.append(",");
    info.append(QString::number((float)config.frameInterval / 10000)); //ms

    emit ActivateDeviceErrorSignal(info);
}

void ultrasound::StartAudio() {
    UpdateVChatUI(Speaking, QString::fromStdString(m_current_channel_info.name));

    nim::NimService::GetInstance()->StartAudioDevice();
}

void ultrasound::ShowCallWidget() {
    call c(this);
    core::CoreService::GetInstance()->AddUserEventHandler(&c);
    core::CoreService::GetInstance()->QuerySubscriberSuperiorUser();

    if (c.exec() == QDialog::Accepted) {
        m_current_channel_info.Clear();

        m_current_channel_info.uid = c.GetCallee().GetUid();
        m_current_channel_info.name = c.GetCallee().GetName();
		m_current_channel_info.ruds_report_title = c.GetCallee().GetRudsReportTitle();
		m_current_channel_info.ruds_report_bedside_title = c.GetCallee().GetRudsReportBedsideTitle();
		m_current_channel_info.hospital = c.GetCallee().GetHospital();
		m_current_channel_info.hospital_id = c.GetCallee().GetHospitalId();
		m_current_channel_info.ruds_title_request = c.GetCallee().GetRudsTitleRequest();
		m_current_channel_info.ruds_confirm_request = c.GetCallee().GetRudsConfirmRequest();

        if (!m_current_channel_info.uid.empty()) {
            StartVChat();
        }
    }

    core::CoreService::GetInstance()->RemoveUserEventHandler(&c);
}

void ultrasound::StartVChat() {
    UpdateVChatUI(Waiting, QString::fromStdString(m_current_channel_info.name));
    
    Json::Value custom_info;
    custom_info["name"] = core::CoreService::GetInstance()->GetCurrentPrimaryUser().GetName();
	custom_info["uid"] = core::CoreService::GetInstance()->GetCurrentPrimaryUser().GetUid();

    nim::NimService::GetInstance()->VChatStart("test", custom_info.toStyledString(),
      m_current_channel_info.uid, m_current_channel_info.uid);
}

void ultrasound::EndVChat() {
    UpdateVChatUI(Idle, "");
    nim::NimService::GetInstance()->VChatEnd("");
}

void ultrasound::VChatError(const QString& message) {
    EndVChat();

    QMessageBox::critical(this, QString::fromLocal8Bit("多点远程超声"),
        message, QMessageBox::Ok);
}

void ultrasound::VChatCalleeRejected() {
    EndVChat();

    QMessageBox::information(this, QString::fromLocal8Bit("多点远程超声"),
        QString::fromLocal8Bit("对方拒绝了您的通话请求。"), QMessageBox::Ok);
}

void ultrasound::VChatReceiveInvite(const QString& uid, const QString& info) {
       nim::NimService::GetInstance()->VChatControl(
         ControlTypeReceiveStartNotifyFeedback);

	   QJsonDocument info_doc(QJsonDocument::fromJson(info.toUtf8()));
	   QJsonObject myInfo = info_doc.object();
	   QString name = myInfo["name"].toString();
	   if (name.isNull() || name.isEmpty())
	   {
		   name = QString::fromLocal8Bit("上级医生:")+uid;
	   }
	   else
	   {
		   name = QString::fromLocal8Bit("上级医生:")+ name;
	   }

       QMessageBox box(
           QMessageBox::Information,
           QString::fromLocal8Bit("多点远程超声"),
		   name + QString::fromLocal8Bit(", 正在呼叫你........          "),
           QMessageBox::Yes | QMessageBox::No);

       box.setButtonText(QMessageBox::Yes, QString::fromLocal8Bit("接听"));
       box.setButtonText(QMessageBox::No, QString::fromLocal8Bit("拒绝"));

       int ret = box.exec();
       if (ret == QMessageBox::Yes) {
		   if (!core::CoreService::GetInstance()->GetCurrentReport().GetChannelCreated())
		   {
			   QJsonDocument info_doc(QJsonDocument::fromJson(info.toUtf8()));
			   QJsonObject info = info_doc.object();
			   m_current_channel_info.name = info["name"].toString().toStdString();
			   if (info["uid"].toString().isNull() || info["uid"].toString().isEmpty())
			   {
				   m_current_channel_info.uid = uid.toStdString();
			   }
			   else
			   {
				   m_current_channel_info.uid = info["uid"].toString().toStdString();
			   }		   
		   }
		   nim::NimService::GetInstance()->VChatCalleeAck(true, "");
       }
       else {
           nim::NimService::GetInstance()->VChatCalleeAck(false, "");
           EndVChat();
       }
}

void ultrasound::VChatHangup() {
    EndVChat();
}

void ultrasound::VChatBusyLine() {
    EndVChat();

    QMessageBox::information(this, QString::fromLocal8Bit("多点远程超声"),
        QString::fromLocal8Bit("对方正在通话中，请稍后再试。"), QMessageBox::Ok);
}

void ultrasound::VChatConnectLogout() {

    if (!core::CoreService::GetInstance()->GetCurrentReport().GetChannelCreated()) 
    {
        QMessageBox box(
            QMessageBox::Question,
            QString::fromLocal8Bit("多点远程超声"),
            QString::fromLocal8Bit("会诊已结束。本次会诊有效吗？\n(选择“有效”后会保存报告，以后将不能再针对本报告进行远程会诊！)"),
            QMessageBox::Yes | QMessageBox::No);

        box.setButtonText(QMessageBox::Yes, QString::fromLocal8Bit("有效"));
        box.setButtonText(QMessageBox::No, QString::fromLocal8Bit("无效"));

        int ret = box.exec();
        if (ret == QMessageBox::Yes) {
            m_current_channel_info.is_valid = true;
            core::CoreService::GetInstance()->GetCurrentReport().SetChannelCreated(true);

            QDir dir = QDir::root();
            if (dir.mkpath(m_report_path)) {

                SaveReportToServer(true);

                UpdateReportUI(Saved);
            }
        }
        else {
            m_current_channel_info.is_valid = false;
            core::CoreService::GetInstance()->GetCurrentReport().SetChannelCreated(false);
        }
    }

    UpdateVChatUI(Idle, "");
}

void ultrasound::UpdateAudioVolumeMeter(const int self_volume,
  const int other_volume) {
    // int16_max 65535

    ui.speaker_frame->resize(other_volume, ui.speaker_frame->height());
    ui.mic_frame->resize(self_volume, ui.mic_frame->height());
}

void ultrasound::UpdateTime() {
    if(ui.call_status_stacked_widget->currentIndex() == Speaking) {
        m_time = m_time.addSecs(1);
        QString text = m_time.toString("mm:ss");
        ui.speaking_lcd->display(text);
    }
}

void ultrasound::AddCaptureImage(const QImage& image) {
    if (m_capture_image_list.size() >= MAX_CAPTURE_IMAGE) {
        QMessageBox::warning(this, QString::fromLocal8Bit("多点远程超声"),
            QString::fromLocal8Bit("最多可以截图") +
            QString::number(MAX_CAPTURE_IMAGE) +
            QString::fromLocal8Bit("张！"), QMessageBox::Ok);

        return;
    }

    QListWidgetItem *item = new QListWidgetItem;
    QString itemText = QString::number(m_capture_filename_index.fetchAndAddAcquire(1));
    item->setText(itemText);
    item->setIcon(QIcon(QPixmap::fromImage(image)));
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setCheckState(Qt::Unchecked);
    ui.listWidget->addItem(item);

    m_capture_image_list[itemText] = image;
}

void ultrasound::DeleteCaptureImage() {
    if (m_checked_image_list.size() <= 0)
        return;

    QMessageBox box(
        QMessageBox::Question,
        QString::fromLocal8Bit("多点远程超声"),
        QString::fromLocal8Bit("确定删除所选截图吗？"),
        QMessageBox::Yes | QMessageBox::No);

    box.setButtonText(QMessageBox::Yes, QString::fromLocal8Bit("确定"));
    box.setButtonText(QMessageBox::No, QString::fromLocal8Bit("取消"));

    int ret = box.exec();
    if (ret == QMessageBox::Yes) {
        for (QString& id : m_checked_image_list) {
            QList<QListWidgetItem*> item_list = ui.listWidget->findItems(id, Qt::MatchExactly);
            foreach(QListWidgetItem * item, item_list)
            {
                delete ui.listWidget->takeItem(ui.listWidget->row(item));
            }

            m_capture_image_list.erase(id);

            QString image_file = m_report_path + "/" + id + ".png";
            QFile::remove(image_file);

            QString object_name = GetCurrentReportId() + "/" + id + ".png";
            core::CoreService::GetInstance()->GetCurrentReport().RemoveImage(object_name.toStdString());
        }

        m_checked_image_list.clear();
    }
}

void ultrasound::CaptureImageClicked(QListWidgetItem *item) {
    if (item->checkState() == Qt::Checked) {
        if (std::find(m_checked_image_list.begin(), m_checked_image_list.end(), item->text()) 
          != m_checked_image_list.end()) {
            return;
        }

        if (m_checked_image_list.size() >= MAX_SELECT_IMAGE) {
            QMessageBox::warning(this, QString::fromLocal8Bit("多点远程超声"),
              QString::fromLocal8Bit("最多可以选择") + 
              QString::number(MAX_SELECT_IMAGE) + 
              QString::fromLocal8Bit("张截图来生成报告！"), QMessageBox::Ok);

            item->setCheckState(Qt::Unchecked);

            return;
        }

        m_checked_image_list.push_back(item->text());
    }
    else if (item->checkState() == Qt::Unchecked) {
        m_checked_image_list.remove(item->text());
    }
}

void ultrasound::CaptureImageDoubleClicked(QListWidgetItem *item) {
    if (m_capture_image_list.count(item->text()) > 0) {
        QImage& image = m_capture_image_list[item->text()];

        if (m_image_viewer == nullptr) {
            m_image_viewer = new ImageViewer(this);
        }

        m_image_viewer->setImage(image);
        m_image_viewer->show();
    }
}

void ultrasound::OnCheckPointIndexChanged(const QString& text) {
    if (!m_check_point_text.isEmpty())
        m_check_point_text.append(QString::fromLocal8Bit("，"));
    m_check_point_text.append(text);
    ui.check_point_combobox->setCurrentText(m_check_point_text);
}

void ultrasound::OnCheckPointEditTextChanged(const QString &text) {
    m_check_point_text = text;
}

void ultrasound::KickOut() {
    m_is_kick_out = true;
    close();
}

void ultrasound::LoadReport(const core::Report& report) {
	m_current_channel_info.Clear();
    m_capture_image_list.clear();
    m_checked_image_list.clear();
    m_capture_filename_index.store(1);

    BuildReportPath();

    QDir dir = QDir::root();
    dir.mkpath(m_report_path);

    // Show process bar
    QThread thread;
    // Do not set a parent. The object cannot be moved if it has a parent. 
    ReportThreadWorker worker(this, false);

    worker.moveToThread(&thread);

    QProgressDialog progressDialog(this);
    progressDialog.setWindowTitle(QString::fromLocal8Bit("多点远程超声"));
    progressDialog.setMinimumWidth(300);
    progressDialog.setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    progressDialog.setWindowFlags(windowFlags() & ~Qt::WindowCloseButtonHint);
    progressDialog.setWindowModality(Qt::WindowModal);
    progressDialog.setAutoReset(true);
    progressDialog.setAutoClose(true);
    progressDialog.setCancelButton(nullptr);
    progressDialog.setMinimum(0);
    progressDialog.setMaximum(100);
    progressDialog.setLabelText(QString::fromLocal8Bit("正在加载报告截图..."));

    connect(&thread, SIGNAL(started()), &worker, SLOT(Download()));
    connect(&worker, SIGNAL(ProgressValueSignal(int)), &progressDialog, SLOT(setValue(int)));

    thread.start();

    progressDialog.exec();

    if (worker.HasError()) {
        QMessageBox::critical(this, QString::fromLocal8Bit("多点远程超声"),
            QString::fromLocal8Bit("加载报告截图出错！\n") + worker.GetErrorMessage() +
            QString::fromLocal8Bit("(error_code=") + QString::number(worker.GetErrorCode()) +
            QString::fromLocal8Bit(")"), QMessageBox::Ok);
    }
}

std::string ultrasound::GenNewReportId() {
    QDate date = QDate::currentDate();
    QTime time = QTime::currentTime();
    QString id = date.toString("yyyyMMdd") + time.toString("hhmmsszzz");
    return id.toStdString();
}


void ultrasound::UpdateRealtimeStatsInfo(const QString& video_info, 
  const QString& audio_info) {
    QString message;
    message.append(QString::fromLocal8Bit("视频："));
    message.append(video_info);
    message.append(QString::fromLocal8Bit("     音频："));
    message.append(audio_info);
    
	// TODO: show nim realtime info
}

void ultrasound::OpenVideoDeviceSetting() {
    DeviceSetting setting(this, m_dshow);
    if (setting.exec() == QDialog::Accepted) {
        
        ui.current_device_label->setText(
          QString::fromWCharArray(m_dshow->GetCurrentVideoConfig().name.c_str()));

        m_dshow->Activate();
    }
}

void ultrasound::CaptureVideoData() {
    m_do_capture.store(1);
}

void ultrasound::OpenAudioDeviceSetting() {
    AudioDeviceSetting setting;
    if (setting.exec() == QDialog::Accepted) {
        if (m_vchat_current_status == Speaking) {
            nim::NimService::GetInstance()->StartAudioDevice();
        }
    }
}

void ultrasound::NewReport() {
    core::CoreService::GetInstance()->CreateReport(GenNewReportId());
    BuildReportPath();
    
	m_current_channel_info.Clear();
    m_capture_image_list.clear();
    m_checked_image_list.clear();
    m_capture_filename_index.store(1);

    UpdateReportUI(New);
}

void ultrasound::SaveReport() {
    if (!ui.submitting_doctor_combobox->currentText().isEmpty())
        Config::GetInstance()->AddSubmittingDoctor(ui.submitting_doctor_combobox->currentText());

    if (!ui.department_combobox->currentText().isEmpty())
        Config::GetInstance()->AddDepartment(ui.department_combobox->currentText());

	// Check report required fields
	/*
	#不能为空
	@validates('title')
	@validates('report_url')
	@validates('uid')
	@validates('name')
	@validates('sex')
	@validates('content')
	@validates('conclusion')
	@validates('check_date')
	@validates('primary')
	@validates('primary_id')
	*/
	QString missing;
	if (ui.name_edit->text().isEmpty())
		missing = QString::fromLocal8Bit("“姓名”");
	else if (ui.date_edit->text().isEmpty())
		missing = QString::fromLocal8Bit("“检查日期”");
	else if (ui.description_edit->toPlainText().isEmpty())
		missing = QString::fromLocal8Bit("“描述”");
	else if (ui.conclusion_edit->toPlainText().isEmpty())
		missing = QString::fromLocal8Bit("“结论”");
	if (!missing.isEmpty()) {
		QMessageBox::information(this,
			QString::fromLocal8Bit("多点远程超声"),
			missing + QString::fromLocal8Bit(" 必须填写，不能为空！"));
		return;
	}

	// Check captured images
    if (m_capture_image_list.size() > 0 && m_checked_image_list.size() <= 0) {

        QMessageBox box(
            QMessageBox::Question,
            QString::fromLocal8Bit("多点远程超声"),
            QString::fromLocal8Bit("您还没有选择截图，确定保存报告吗？"),
            QMessageBox::Yes | QMessageBox::No);

        box.setButtonText(QMessageBox::Yes, QString::fromLocal8Bit("确定"));
        box.setButtonText(QMessageBox::No, QString::fromLocal8Bit("取消"));

        int ret = box.exec();
        if (ret == QMessageBox::No)
            return;
    }

    QDir dir = QDir::root();
    if (dir.mkpath(m_report_path)) {

        SaveReportToServer(false);

        UpdateReportUI(Saved);
    }
    else {
        QMessageBox::critical(this, QString::fromLocal8Bit("多点远程超声"),
            QString::fromLocal8Bit("创建报告目录失败！") + m_report_path,
            QMessageBox::Ok);
    }
}

void ultrasound::PreviewReport() {
    
    // Save report first
    SaveReport();

    QPrinter printer(QPrinter::HighResolution);
    printer.setPageSize(QPrinter::A4);
	printer.setPageMargins(2, 2, 2, 2, QPrinter::Millimeter);
    QPrintPreviewDialog preview(&printer, this);
    preview.setWindowTitle(QString::fromLocal8Bit("打印预览"));
    preview.setWindowFlags(Qt::Window);
    connect(&preview, SIGNAL(paintRequested(QPrinter*)), this, SLOT(Print(QPrinter*)));
    preview.exec();
}

void ultrasound::PrintReport() {
	core::Report& report = core::CoreService::GetInstance()->GetCurrentReport();
	if (report.GetConfirmStatus() == core::CONFIRM_STATUS_REJECT ||
		report.GetConfirmStatus() == core::CONFIRM_STATUS_WAIT) {
		QMessageBox box(
			QMessageBox::Question,
			QString::fromLocal8Bit("多点远程超声"),
			QString::fromLocal8Bit("报告还未审核通过，不能打印！"),
			QMessageBox::Ok);
		box.exec();
		return;
	}

    // Save report first
    SaveReport();

    QPrinter printer(QPrinter::HighResolution);
    printer.setPageSize(QPrinter::A4);
	printer.setPageMargins(2, 2, 2, 2, QPrinter::Millimeter);
    QPrintDialog print(&printer, this);
    if (print.exec() == QDialog::Accepted) {
        Print(&printer);
    }
}

void ultrasound::Print(QPrinter *p)
{
    Json::Value data;
	core::Report& report = core::CoreService::GetInstance()->GetCurrentReport();
    report.SaveToJson(data, false);

    QJsonDocument loadDoc(QJsonDocument::fromJson(data.toStyledString().c_str()));
    QJsonObject json_report = loadDoc.object();

    if (!json_report.isEmpty()) {
        QTextDocument document;
        document.setHtml(ReportHtml::BuildHtml(json_report, m_report_path, report.GetHospitalId(), report.GetSuperiorHospitalId()));
        document.print(p);
    }
}

void ultrasound::OpenReportTemplate() {
    ReportTemplate t;
    if (t.exec() == QDialog::Accepted) {
        ui.description_edit->append(t.GetDescription());
        ui.conclusion_edit->append(t.GetConclusion());
    }
}

void ultrasound::OpenReportQuery() {
    ReportQuery r(this);
    core::CoreService::GetInstance()->AddReportEventHandler(&r);
    if (r.exec() == QDialog::Accepted) {
        core::CoreService::GetInstance()->RemoveReportEventHandler(&r);

        core::Report report;
        if (r.GetSelectedReport(report)) {
            core::CoreService::GetInstance()->LoadReport(report);
            LoadReport(report);

            UpdateReportUI(Loaded);
        }
        else {
            QMessageBox::critical(this, QString::fromLocal8Bit("多点远程超声"),
                QString::fromLocal8Bit("打开报告失败！"),
                QMessageBox::Ok);
        }
    }
    else {
        core::CoreService::GetInstance()->RemoveReportEventHandler(&r);
    }
}

void ultrasound::OpenUpdatePassword() {
    UpdatePassword u;

    core::CoreService::GetInstance()->AddUserEventHandler(&u);

    u.exec();

    core::CoreService::GetInstance()->RemoveUserEventHandler(&u);
}

void ultrasound::OpenProfile() {
    Profile p;
    core::CoreService::GetInstance()->AddUserEventHandler(&p);

    p.exec();

    core::CoreService::GetInstance()->RemoveUserEventHandler(&p);
}

void ultrasound::onReceiveSuperiorMessage(const QString message)
{
	ShowSystemMessageInStatusBar(message);
}

void ultrasound::ReceiveReportConfirmedMessage(const QString& superior_id, const QString& superior_name,
	const QString& uid, const QString& content, const QString& conclusion, const int confirm_status,
	const QString& confirm_msg) {
	
	if (confirm_status == core::CONFIRM_STATUS_PASS || 
		confirm_status == core::CONFIRM_STATUS_REJECT) {

		core::Report& report = core::CoreService::GetInstance()->GetCurrentReport();
		if (report.GetUid() == uid.toStdString()) {
			report.SetContent(content.toStdString());
			report.SetConclusion(conclusion.toStdString());
			report.SetConfirmStatus((core::CONFIRM_STATUS)confirm_status);

			if (confirm_status == core::CONFIRM_STATUS_PASS)
				ui.status_line_edit->setText(QString::fromLocal8Bit("审核通过"));
			else
				ui.status_line_edit->setText(QString::fromLocal8Bit("审核未通过"));
			ui.description_edit->setText(content);
			ui.conclusion_edit->setText(conclusion);
		}

		QString result = confirm_status == core::CONFIRM_STATUS_PASS ? QString::fromLocal8Bit(" *通过* ") :
			QString::fromLocal8Bit(" *未通过* ");

		QString message = superior_name + QString::fromLocal8Bit(" 审核") + result +
			QString::fromLocal8Bit("报告（超声编号：") + uid + QString::fromLocal8Bit("）。审核意见：") +
			confirm_msg;
		ShowSystemMessageInStatusBar(message);
	}
}

void ultrasound::OpenSystemMessageWindow() {
	ui.statusBar->clearMessage();
	m_system_message_viewer->show();
}

void ultrasound::OnKickOut(ClientType client_type, KickReason kick_reason) {
    emit KickOutSignal();
}

void ultrasound::OnVChatChannelCreated(const int64_t channel_id) {

}

void ultrasound::OnVChatStartError(int code) {
    emit VChatErrorSignal(QString::fromLocal8Bit("通话创建错误！(code = ") +
        QString::number(code) + QString::fromLocal8Bit(")"));
}

void ultrasound::OnVChatConnected(const int64_t channel_id) {
    m_current_channel_info.channel = channel_id;

    emit StartAudioSignal();
}

void ultrasound::OnVChatConnectLogout() {
    
    m_current_channel_info.channel_time =  m_time.hour() * 60 * 60 + m_time.minute() * 60 + m_time.second();

    emit VChatConnectLogoutSignal();
}

void ultrasound::OnVChatConnectError(const int code) {
    emit VChatErrorSignal(QString::fromLocal8Bit("通话链接错误！(code = ") + 
      QString::number(code) + QString::fromLocal8Bit(")"));
}

void ultrasound::OnCalleeRejected() {
    emit VChatCalleeRejectedSignal();
}

void ultrasound::OnControlNotify(const std::string& uid, ControlType type) {
    switch (type)
    {
    case ControlTypeOpenAudio:
        break;
    case ControlTypeCloseAudio:
        break;
    case ControlTypeOpenVideo:
        break;
    case ControlTypeCloseVideo:
        break;
    case ControlTypeAudioToVideo:
        break;
    case ControlTypeAgreeAudioToVideo:
        break;
    case ControlTypeRejectAudioToVideo:
        break;
    case ControlTypeVideoToAudio:
        break;
    case ControlTypeBusyLine: {
        emit VChatBusyLineSignal();
        break;
    }
    case ControlTypeCamaraNotAvailable:
        break;
    case ControlTypeEnterBackground:
        break;
    case ControlTypeReceiveStartNotifyFeedback:
        break;
    case ControlTypeMp4StartRecord:
        break;
    case ControlTypeMp4StopRecord:
        break;
    default:
        break;
    }
}

void ultrasound::OnReceivedVChatInvite(const int64_t channel_id, 
  const std::string& uid, const std::string& custom_info) {
    emit VChatReceiveInviteSignal(QString::fromStdString(uid), 
      QString::fromStdString(custom_info));
}

void ultrasound::OnHangup() {
    emit VChatHangupSignal();
}

void ultrasound::OnCapturedVideoData(unsigned __int64 time, const char* data,
    unsigned int size, unsigned int width, unsigned int height) {

}

void ultrasound::OnReceivedVideoData(unsigned __int64 time, const char* data,
    unsigned int size, unsigned int width, unsigned int height) {
        QImage image(width, height, QImage::Format_ARGB32);
    
        uint8_t* src_y = (uint8_t*)data;
        uint8_t* src_u = src_y + width * height;
        uint8_t* src_v = src_u + width * height / 4;
    
        libyuv::I420ToARGB(
            src_y, width,
            src_u, width / 2,
            src_v, width / 2,
            (uint8_t*)image.bits(), width * 4,
            width, height);
    
        emit UpdateVChatVideoDataSignal(image);
}

void ultrasound::OnRealtimeStatsInfo(const StatsInfo& info) {
    QString videoInfo;
    videoInfo.append("rtt:");
    videoInfo.append(QString::number(info.rtt));
    videoInfo.append(",fps:");
    videoInfo.append(QString::number(info.video_fps));
    videoInfo.append(",KBps:");
    videoInfo.append(QString::number(info.video_KBps));
    videoInfo.append(",lost_rate:");
    videoInfo.append(QString::number(info.video_lost_rate));
    videoInfo.append(",width:");
    videoInfo.append(QString::number(info.video_width));
    videoInfo.append(",height:");
    videoInfo.append(QString::number(info.video_height));
    
    QString audioInfo;
    audioInfo.append("fps:");
    audioInfo.append(QString::number(info.audio_fps));
    audioInfo.append(",KBps:");
    audioInfo.append(QString::number(info.audio_KBps));
    audioInfo.append(",lost_rate:");
    audioInfo.append(QString::number(info.audio_lost_rate));
    
    emit UpdateRealtimeStatsInfoSignal(videoInfo, audioInfo);
}

void ultrasound::OnSessionNetStatus(const std::string& uid,
  const SessionNetStatus status) {

}

void ultrasound::OnVChatOptError(const OptType type, const int code) {

}

void ultrasound::OnAudioVolumeNotify(const int32_t self_volume,
  const int32_t other_volume) {
    emit UpdateAudioVolumeMeterSignal(self_volume, other_volume);
}

void ultrasound::OnVchatSyncNotify(bool accept) {

}

void ultrasound::OnGetPrimaryUserInfoSuccessed(const core::PrimaryUser& user) {}
void ultrasound::OnGetPrimaryUserInfoFailed(int error_code, const std::string& message) {}
void ultrasound::OnGetSuperiorUserInfoSuccessed(const core::SuperiorUser& user) {}
void ultrasound::OnGetSuperiorUserInfoFailed(int error_code, const std::string& message) {}

void ultrasound::OnUpdatePasswordSuccessed(const std::string& uid, 
  const std::string& new_password) {
    Config::GetInstance()->UpdateUserPassword(QString::fromStdString(uid), 
        QString::fromStdString(new_password));
}

void ultrasound::OnUpdatePasswordFailed(int error_code, const std::string& message) {}
void ultrasound::OnQuerySubscriberSuperiorUserSuccessed(std::list<core::SuperiorUser> users) {}
void ultrasound::OnQuerySubscriberSuperiorUserFailed(int error_code, const std::string& message) {}
void ultrasound::OnQuerySuperiorUserSuccessed(std::list<core::SuperiorUser> users) {}
void ultrasound::OnQuerySuperiorUserFailed(int error_code, const std::string& message) {}
void ultrasound::OnUpdatePrimaryUserInfoSuccessed() {}
void ultrasound::OnUpdatePrimaryUserInfoFailed(int error_code, const std::string& message) {}
void ultrasound::OnUpdateSuperiorUserInfoSuccessed() {}
void ultrasound::OnUpdateSuperiorUserInfoFailed(int error_code, const std::string& message) {}
void ultrasound::OnQueryControlPrimaryUserSuccessed(std::list<core::PrimaryUser> users) {}
void ultrasound::OnQueryControlPrimaryUserFailed(int error_code, const std::string& message) {}

void ultrasound::OnCreateReportSuccessed() {}
void ultrasound::OnCreateReportFailed(int error_code, const std::string& message) {}
void ultrasound::OnUpdateReportSuccessed() {}
void ultrasound::OnUpdateReportFailed(int error_code, const std::string& message) {}
void ultrasound::OnConfirmReportSuccessed(const core::ConfirmReportData& data) {}
void ultrasound::OnConfirmReportFailed(int error_code, const std::string& message) {}
void ultrasound::OnQueryReportSuccessed(std::list<core::Report> reports) {}
void ultrasound::OnQueryReportFailed(int error_code, const std::string& message) {}

void ultrasound::SaveCapturedVideoData(const QImage& image) {
    if (m_do_capture.testAndSetAcquire(1, 0)) {
        emit AddCaptureImageSignal(image);
    }
}

void ultrasound::UpdateVChatUI(VChatStatus status, const QString& remote_user_name) {
    m_vchat_current_status = status;

    ui.call_status_stacked_widget->setCurrentIndex(status);
    switch (status)
    {
    case Idle: {
        m_Sound.stop();

        QImage black;
        black.fill(Qt::black);
        ui.vchat_video_data_label->setPixmap(QPixmap::fromImage(black));
        ui.speaker_frame->resize(0, ui.speaker_frame->height());
        ui.mic_frame->resize(0, ui.mic_frame->height());

        if (core::CoreService::GetInstance()->GetCurrentReport().GetChannelCreated())
            ui.call_button->setEnabled(false);
        else
            ui.call_button->setEnabled(true);
        ui.end_button->setEnabled(false);
        break;
    }
    case Waiting: {
        m_Sound.play();

        ui.speaker_frame->resize(0, ui.speaker_frame->height());
        ui.mic_frame->resize(0, ui.mic_frame->height());
        ui.uid_waiting_label->setText(remote_user_name);

        ui.call_button->setEnabled(false);
        ui.end_button->setEnabled(true);
        break;
    }
    case Speaking: {
        m_Sound.stop();

        ui.speaker_frame->resize(0, ui.speaker_frame->height());
        ui.mic_frame->resize(0, ui.mic_frame->height());
        ui.uid_speaking_label->setText(remote_user_name);
        
        m_time.setHMS(0, 0, 0);
        QString text = m_time.toString("mm:ss");
        ui.speaking_lcd->display(text);
        
        ui.call_button->setEnabled(false);
        ui.end_button->setEnabled(true);
        break;
    }
    default:
        break;
    }
}

void ultrasound::UpdateReportUI(ReportStatus status) {
    m_report_current_status = status;

    switch (status)
    {
    case ultrasound::New: {
        ui.name_edit->clear();
        ui.number_edit->clear();
        
        LoadDepartment(false);
        LoadDoctor(false);
        LoadCheckPoint(false);

        m_check_point_text.clear();
        ui.check_point_combobox->setCurrentIndex(0);
        m_check_point_text = ui.check_point_combobox->currentText();

		ui.patient_id_edit->clear();
		ui.ruds_reason_edit->clear();
		ui.ruds_target_edit->clear();
		ui.bedside_type_checkbox->setChecked(false);
		ui.status_line_edit->setText(QString::fromLocal8Bit("无需审核"));

        ui.description_edit->clear();
        ui.conclusion_edit->clear();
        
        QDate date = QDate::currentDate();
        ui.date_edit->setText(date.toString("yyyy-MM-dd"));
        ui.ultrasound_number_edit->setText(GetCurrentReportId());

        ui.new_button->setEnabled(true);
        ui.template_button->setEnabled(true);
        ui.save_button->setEnabled(true);
        ui.preview_button->setEnabled(false);
        ui.print_button->setEnabled(false);
        
        ui.call_button->setEnabled(true);
        ui.end_button->setEnabled(false);
        ui.query_button->setEnabled(true);
        ui.change_password_button->setEnabled(true);
        ui.profile_button->setEnabled(true);

        ui.capture_button->setEnabled(true);
        ui.delete_button->setEnabled(true);

        ui.listWidget->clear();
        
        break;
    }
    case ultrasound::Loaded: {
        const core::Report& report = core::CoreService::GetInstance()->GetCurrentReport();

        ui.name_edit->setText(QString::fromStdString(report.GetName()));
        ui.sex_combo_box->setCurrentText(QString::fromStdString(report.GetSex()));
        ui.age_spin_box->setValue(report.GetAge());
		ui.patient_id_edit->setText(QString::fromStdString(report.GetPatientId()));
        ui.number_edit->setText(QString::fromStdString(report.GetDialogId()));

        ui.department_combobox->setCurrentText(QString::fromStdString(report.GetDepartment()));
        ui.submitting_doctor_combobox->setCurrentText(QString::fromStdString(report.GetDiaDoctor()));
        m_check_point_text = QString::fromStdString(report.GetParts());
        ui.check_point_combobox->setCurrentText(m_check_point_text);
		ui.ruds_reason_edit->setText(QString::fromStdString(report.GetRudsReason()));
		ui.ruds_target_edit->setText(QString::fromStdString(report.GetRudsTarget()));

        ui.date_edit->setText(QString::fromStdString(report.GetCheckDate()));
        ui.ultrasound_number_edit->setText(QString::fromStdString(report.GetUid()));
		ui.bedside_type_checkbox->setChecked(report.GetBedsideType());

		ui.status_line_edit->setText(QString::fromLocal8Bit("无需审核"));
		switch (report.GetConfirmStatus()) {
		case core::CONFIRM_STATUS_PASS:
			ui.status_line_edit->setText(QString::fromLocal8Bit("审核通过"));
			break;
		case core::CONFIRM_STATUS_REJECT:
			ui.status_line_edit->setText(QString::fromLocal8Bit("审核未通过"));
			break;
		case core::CONFIRM_STATUS_WAIT:
			ui.status_line_edit->setText(QString::fromLocal8Bit("未审核"));
			break;
		default:
			break;
		}

        ui.description_edit->setPlainText(QString::fromStdString(report.GetContent()));
        ui.conclusion_edit->setPlainText(QString::fromStdString(report.GetConclusion()));

        ui.new_button->setEnabled(true);
        ui.template_button->setEnabled(true);
        ui.save_button->setEnabled(true);
        ui.preview_button->setEnabled(true);
        ui.print_button->setEnabled(true);

        if (report.GetChannelCreated()) {
            ui.call_button->setEnabled(false);
            ui.end_button->setEnabled(false);
        }

        ui.query_button->setEnabled(true);
        ui.change_password_button->setEnabled(true);
        ui.profile_button->setEnabled(true);

        ui.capture_button->setEnabled(true);
        ui.delete_button->setEnabled(true);

        ui.listWidget->clear();
        std::map<QString, QImage>::iterator it = m_capture_image_list.begin();
        for (; it != m_capture_image_list.end(); it++) {
            QImage& image = it->second;

            QListWidgetItem *item = new QListWidgetItem;
            QString itemText = it->first;
            item->setText(itemText);
            item->setIcon(QIcon(QPixmap::fromImage(image)));
            item->setFlags(item->flags() | Qt::ItemIsUserCheckable);

            if (std::find(m_checked_image_list.begin(), m_checked_image_list.end(), itemText) 
                != m_checked_image_list.end())
                item->setCheckState(Qt::Checked);
            else
                item->setCheckState(Qt::Unchecked);
            
            ui.listWidget->addItem(item);
        }
        
        break;
    }
    case ultrasound::Saved: {
        ui.preview_button->setEnabled(true);
        ui.print_button->setEnabled(true);
        break;
    }
    default:
        break;
    }
}

void ultrasound::LoadDepartment(bool reload) {
    if (reload || ui.department_combobox->count() <= 0) {
        ui.department_combobox->addItems(Config::GetInstance()->GetDepartmentList());
    }
}

void ultrasound::LoadDoctor(bool reload) {
    if (reload || ui.submitting_doctor_combobox->count() <= 0) {
        ui.submitting_doctor_combobox->addItems(Config::GetInstance()->GetSubmittingDoctorList());
    }
}

void ultrasound::LoadCheckPoint(bool reload) {
    if (reload || ui.check_point_combobox->count() <= 0) {
        QSqlQueryModel *model = new QSqlQueryModel;
        model->setQuery("SELECT checkpoint_name FROM checkpoint_name");
        ui.check_point_combobox->setModel(model);
    }
}

void ultrasound::BuildReportPath() {
    m_report_path = m_user_path + "/" + GetCurrentReportId();
}

void ultrasound::OpenTemplateDB() {
    if (m_template_db.isOpen()) {
        return;
    }

    QString template_db_path = QCoreApplication::applicationDirPath();
    template_db_path.append("/db/template.db");

    m_template_db = QSqlDatabase::addDatabase("QSQLITE");
    m_template_db.setDatabaseName(template_db_path);
    bool res = m_template_db.open();
    if (!res) {
        QSqlError error = m_template_db.lastError();
        QString text = error.databaseText() + ", " + error.driverText() + ", " +
            error.nativeErrorCode() + ", " + error.text();
        QMessageBox::warning(this, "Debug", text);
    }
}

void ultrasound::CloseTemplateDB() {
    if (m_template_db.isOpen()) {
        m_template_db.close();
    }
}

void ultrasound::SaveReportToServer(bool auto_save) {
    core::Report& report = core::CoreService::GetInstance()->GetCurrentReport();

	// Make sure requied fields have value
	if(ui.name_edit->text().isEmpty())
		report.SetName(" ");
	else
		report.SetName(ui.name_edit->text().toStdString());

    report.SetSex(ui.sex_combo_box->currentText().toStdString());
    report.SetAge(ui.age_spin_box->text().toInt());
    report.SetParts(m_check_point_text.toStdString());

	if(ui.description_edit->toPlainText().isEmpty())
		report.SetContent(" ");
	else
		report.SetContent(ui.description_edit->toPlainText().toStdString());
    
	if(ui.conclusion_edit->toPlainText().isEmpty())
		report.SetConclusion(" ");
	else
		report.SetConclusion(ui.conclusion_edit->toPlainText().toStdString());

	report.SetDepartment(ui.department_combobox->currentText().toStdString());
    report.SetDialogId(ui.number_edit->text().toStdString());

	if (ui.date_edit->text().isEmpty())
		report.SetCheckDate(QDate::currentDate().toString("yyyy-MM-dd").toStdString());
	else
		report.SetCheckDate(ui.date_edit->text().toStdString());

	report.SetPatientId(ui.patient_id_edit->text().toStdString());
	report.SetRudsReason(ui.ruds_reason_edit->text().toStdString());
	report.SetRudsTarget(ui.ruds_target_edit->text().toStdString());
	report.SetBedsideType(ui.bedside_type_checkbox->isChecked());

    report.SetPrimary(core::CoreService::GetInstance()->GetCurrentPrimaryUser().GetName());
    report.SetPrimaryId(core::CoreService::GetInstance()->GetCurrentPrimaryUser().GetUid());
    report.SetHospital(core::CoreService::GetInstance()->GetCurrentPrimaryUser().GetHospital());

    if (m_current_channel_info.is_valid) {
        report.SetChannel(std::to_string(m_current_channel_info.channel));
        report.SetChannelTime(m_current_channel_info.channel_time);
        report.SetSuperior(m_current_channel_info.name);
        report.SetSuperiorId(m_current_channel_info.uid);
		report.SetSuperiorHospitalId(m_current_channel_info.hospital_id);
		report.SetSuperiorHospitalName(m_current_channel_info.hospital);

		// Save superior properties so we can build report title next time after loading report
		report.SetRudsTitleRequest(m_current_channel_info.ruds_title_request);
		report.SetRudsReportTitle(m_current_channel_info.ruds_report_title);
		report.SetRudsReportBedsideTitle(m_current_channel_info.ruds_report_bedside_title);

		if (m_current_channel_info.ruds_confirm_request &&
			report.GetConfirmStatus() == core::CONFIRM_STATUS_NONE) {
			report.SetConfirmStatus(core::CONFIRM_STATUS_WAIT);
			ui.status_line_edit->setText(QString::fromLocal8Bit("未审核"));
		}
    }

	// Build report title, format is "hospital" + " " + "title"
	if (report.GetChannelCreated() && report.GetRudsTitleRequest()) {
		// User superior title
		std::string report_tile = report.GetSuperiorHospitalName();
		report_tile += " ";
		if (report.GetBedsideType()) 
			report_tile += report.GetRudsReportBedsideTitle();
		else 
			report_tile += report.GetRudsReportTitle();
		
		report.SetTitle(report_tile);
	}
	else {
		// User primary title
		std::string report_tile = core::CoreService::GetInstance()->GetCurrentPrimaryUser().GetHospital();
		report_tile += " ";
		if (report.GetBedsideType()) {
			std::string bedside_title =
				core::CoreService::GetInstance()->GetCurrentPrimaryUser().GetRudsReportBedsideTitle();

			if (bedside_title.empty()) 
				bedside_title = QString::fromLocal8Bit("床旁超声检查报告单").toStdString();

			report_tile += bedside_title;
		}
		else {
			std::string title =
				core::CoreService::GetInstance()->GetCurrentPrimaryUser().GetRudsReportTitle();

			if (title.empty())
				title = QString::fromLocal8Bit("远程超声会诊报告单").toStdString();

			report_tile += title;
		}

		report.SetTitle(report_tile);
	}

    report.SetDiaDoctor(ui.submitting_doctor_combobox->currentText().toStdString());
    report.SetHospitalId(core::CoreService::GetInstance()->GetCurrentPrimaryUser().GetHospitalId());

    // Show process bar
    QThread thread;
    // Do not set a parent. The object cannot be moved if it has a parent. 
    ReportThreadWorker worker(this, auto_save);

    core::CoreService::GetInstance()->AddReportEventHandler(&worker);

    worker.moveToThread(&thread);

    QProgressDialog progressDialog(this);
    progressDialog.setWindowTitle(QString::fromLocal8Bit("多点远程超声"));
    progressDialog.setMinimumWidth(300);
    progressDialog.setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    progressDialog.setWindowFlags(windowFlags() & ~Qt::WindowCloseButtonHint);
    progressDialog.setWindowModality(Qt::WindowModal);
    progressDialog.setAutoReset(true);
    progressDialog.setAutoClose(true);
    progressDialog.setCancelButton(nullptr);
    progressDialog.setMinimum(0);
    progressDialog.setMaximum(100);
    progressDialog.setLabelText(QString::fromLocal8Bit("正在保存报告..."));

    connect(&thread, SIGNAL(started()), &worker, SLOT(Upload()));
    connect(&worker, SIGNAL(ProgressValueSignal(int)), &progressDialog, SLOT(setValue(int)));

    thread.start();

    progressDialog.exec();

    core::CoreService::GetInstance()->RemoveReportEventHandler(&worker);

    if (worker.HasError()) {
        QMessageBox::critical(this, QString::fromLocal8Bit("多点远程超声"),
          QString::fromLocal8Bit("保存报告出错，请重试！\n") + worker.GetErrorMessage() + 
          QString::fromLocal8Bit("(error_code=") + QString::number(worker.GetErrorCode()) + 
          QString::fromLocal8Bit(")"), QMessageBox::Ok);
    }
}

QString ultrasound::GetCurrentReportId() {
    return QString::fromStdString(core::CoreService::GetInstance()->GetCurrentReport().GetUid());
}

QFileInfoList ultrasound::GetReportCapturedImages() {
    QDir report_dir(m_report_path);
    QStringList filters;
    filters << "*.png";
    report_dir.setNameFilters(filters);
    report_dir.setSorting(QDir::Time);

	QFileInfoList image_list =  report_dir.entryInfoList();
	QFileInfoList return_list;
	
	for each (const QFileInfo& info in image_list) {
		if (info.fileName() != "qr_code.png" && info.fileName() != "sign.png")
			return_list.append(info);
	}

	return return_list;
}

void ultrasound::OnReceiveMessage(const nim::SystemMessage& sys_msg)
{
	// See app\ultrasound_system_message_id.h for message format

	QString json_message = QString::fromStdString(sys_msg.message);

	QJsonParseError json_error;
	QJsonDocument jsonDoc(QJsonDocument::fromJson(json_message.toUtf8(), &json_error));

	if (json_error.error != QJsonParseError::NoError) {
		// Show raw message text by default.
		QString message = QString::fromStdString(sys_msg.message);
		if (message.length() > 0) {
			emit receiveSuperiorMessage(message);
		}
		return;
	}
	else {
		QJsonObject root_object = jsonDoc.object();
		QString message_id = root_object.value("message_id").toString();
		QJsonValue message_value = root_object.value("message");
		if (!message_id.isEmpty() && message_value.isObject()) {
			if (message_id == kUltrasoundMessageIdReportConfirmed) {
				QJsonObject message_object = message_value.toObject();
				QString superior_id = message_object.value("superior_id").toString();
				QString superior_name = message_object.value("superior_name").toString();
				QString uid = message_object.value("uid").toString();
				QString content = message_object.value("content").toString();
				QString conclusion = message_object.value("conclusion").toString();
				int confirm_status = message_object.value("confirm_status").toInt();
				QString confirm_msg = message_object.value("confirm_msg").toString();

				emit ReceiveReportConfirmedMessageSignal(superior_id, superior_name, 
					uid, content, conclusion, confirm_status, confirm_msg);
			}
		}
	}
}

void ultrasound::OnQuerySubscriberSuperiorUserOnlineStatusSuccessed(std::list<core::SuperiorUserStatus> users)
{
}

void ultrasound::OnQuerySubscriberSuperiorUserOnlineStatusFailed(int error_code, const std::string & message)
{
}

void ultrasound::ShowSystemMessageInStatusBar(const QString& message, bool add_ts) {
	QString display_msg = QString::fromLocal8Bit("系统消息：");;
	QString ts;
	if (add_ts) {
		QDateTime now = QDateTime::currentDateTime();
		ts = now.toString("yyyy-MM-dd hh:mm:ss ");
		display_msg += ts;
	}
	display_msg += message;
	ui.statusBar->showMessage(display_msg);

	m_system_message_viewer->AddMessage(ts + message);
}

