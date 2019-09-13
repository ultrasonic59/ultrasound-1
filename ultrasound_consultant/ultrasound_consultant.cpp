#include "ultrasound_consultant.h"

#include <QCalendarWidget>
#include <QMovie>
#include <QMessageBox>
#include <QProcess>
#include <QtWidgets>

#include "app/ultrasound_system_message_id.h"
#include "config.h"
#include "ConfirmReport.h"
#include "core_service/core_service.h"
#include "nim_service/nim_service.h"
#include "oss_service/oss_service.h"
#include "third_party/libyuv/include/libyuv.h"
#include "RecentCall.h"

UltrasoundConsultant::UltrasoundConsultant(QWidget *parent)
    : QMainWindow(parent), m_Sound(":/UltrasoundConsultant/Resources/callee_ringing.wav"),
    m_Control_Sound(":/UltrasoundConsultant/Resources/caller_ringing.wav")
{
    ui.setupUi(this);

	m_system_message_viewer = new SystemMessageViewer(this);
	m_show_system_message_statusbar_label = new ClickableLabel(ui.status_bar);
	m_show_system_message_statusbar_label->setCursor(Qt::PointingHandCursor);
	m_show_system_message_statusbar_label->setStyleSheet("font-weight: bold; color: red");
	m_show_system_message_statusbar_label->setText(QString::fromLocal8Bit("点击查看系统消息   "));
	ui.status_bar->addPermanentWidget(m_show_system_message_statusbar_label);
	connect(m_show_system_message_statusbar_label, SIGNAL(clicked()), this, SLOT(OpenSystemMessageWindow()));

    m_Sound.setLoops(QSound::Infinite);
    m_Control_Sound.setLoops(QSound::Infinite);

    // profile_widget
    connect(ui.disturb_button, SIGNAL(clicked()), this, SLOT(SetDisturb()));

    // button_list_widget
    QListWidgetItem *phone_item = new QListWidgetItem;
    phone_item->setText(QString::fromLocal8Bit("会诊"));
    phone_item->setIcon(QIcon(":/UltrasoundConsultant/Resources/phone_idle.png"));
    ui.button_list_widget->addItem(phone_item);

    QListWidgetItem *recent_item = new QListWidgetItem;
    recent_item->setText(QString::fromLocal8Bit("最近通话"));
    recent_item->setIcon(QIcon(":/UltrasoundConsultant/Resources/clock.png"));
    ui.button_list_widget->addItem(recent_item);

    QListWidgetItem *history_item = new QListWidgetItem;
    history_item->setText(QString::fromLocal8Bit("会诊历史"));
    history_item->setIcon(QIcon(":/UltrasoundConsultant/Resources/report.png"));
    ui.button_list_widget->addItem(history_item);

    QListWidgetItem *my_item = new QListWidgetItem;
    my_item->setText(QString::fromLocal8Bit("我的资料"));
    my_item->setIcon(QIcon(":/UltrasoundConsultant/Resources/doctor.png"));
    ui.button_list_widget->addItem(my_item);

    QListWidgetItem *settings_item = new QListWidgetItem;
    settings_item->setText(QString::fromLocal8Bit("设置"));
    settings_item->setIcon(QIcon(":/UltrasoundConsultant/Resources/settings.png"));
    ui.button_list_widget->addItem(settings_item);
	settings_item->setHidden(true);

    connect(ui.button_list_widget, SIGNAL(currentRowChanged(int)),
        ui.right_stacked_widget, SLOT(setCurrentIndex(int)));
    ui.button_list_widget->setCurrentItem(phone_item);

    // right_stacked_widget
    connect(ui.right_stacked_widget, SIGNAL(currentChanged(int)),
        this, SLOT(OnRightStackedWidgetCurrentChanged(int)));

    // consultant_page
    connect(ui.consultant_control_button, SIGNAL(clicked()), this, SLOT(QueryControlPrimaryUser()));
    connect(ui.consultant_control_call_button, SIGNAL(clicked()), this, SLOT(VChatStart()));

    connect(this, SIGNAL(QueryControlPrimaryUserSuccessedSignal()),
        this, SLOT(QueryControlPrimaryUserSuccessedSlot()));
    connect(this, SIGNAL(QueryControlPrimaryUserFailedSignal()),
        this, SLOT(QueryControlPrimaryUserFailedSlot()));

    QStringList consultant_headers = { "uid",
        QString::fromLocal8Bit("姓名"),
		QString::fromLocal8Bit("状态"),
        QString::fromLocal8Bit("电话"),
        QString::fromLocal8Bit("科室")};

    ui.consultant_control_table_widget->setColumnCount(consultant_headers.count());
    ui.consultant_control_table_widget->setHorizontalHeaderLabels(consultant_headers);
    ui.consultant_control_table_widget->setColumnHidden(0, true);
    ui.consultant_control_table_widget->setRowCount(0);
	ui.consultant_control_table_widget->setColumnWidth(2, 0.5 * ui.consultant_control_table_widget->columnWidth(1));
    ui.consultant_control_table_widget->setColumnWidth(3, 1.1 * ui.consultant_control_table_widget->columnWidth(1));
    ui.consultant_control_table_widget->setColumnWidth(4, 1.1 * ui.consultant_control_table_widget->columnWidth(1));

    ui.consultant_control_call_button->setEnabled(false);
    connect(ui.consultant_control_table_widget, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(OnConsultantControlItemClicked(QTableWidgetItem*)));

    ui.consultant_speaker_frame_background->stackUnder(ui.consultant_speaker_frame);
    ui.consultant_mic_frame_background->stackUnder(ui.consultant_mic_frame);

    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(UpdateTime()));
    m_timer->start(1000);

    connect(ui.consultant_answer_button, SIGNAL(clicked()), this, SLOT(VChatAnswer()));
    connect(ui.consultant_wait_button, SIGNAL(clicked()), this, SLOT(VChatWait()));
    connect(ui.consultant_reject_button, SIGNAL(clicked()), this, SLOT(VChatReject()));
    connect(ui.consultant_end_button, SIGNAL(clicked()), this, SLOT(VChatEnd()));

    ui.consultant_stacked_widget->setCurrentIndex(1);

    QMovie *movie2 = new QMovie(":/UltrasoundConsultant/Resources/ringing.gif");
    ui.consultant_ringing_gif_label->setMovie(movie2);
    movie2->start();

	//recent_page
	QStringList recent_headers = { QString::fromLocal8Bit("uid"), QString::fromLocal8Bit("姓名"),
		QString::fromLocal8Bit("性别"), QString::fromLocal8Bit("发生时间"),QString::fromLocal8Bit("接听状态"),
		QString::fromLocal8Bit("通话时长"),QString::fromLocal8Bit("电话号码"), QString::fromLocal8Bit("所属医院"),QString::fromLocal8Bit("在线提醒") };
	ui.recent_table_widget->setColumnCount(recent_headers.count());
	ui.recent_table_widget->setHorizontalHeaderLabels(recent_headers);
	ui.recent_table_widget->setRowCount(0);

	ui.recent_table_widget->setColumnWidth(0, 1 * ui.recent_table_widget->columnWidth(0));
	ui.recent_table_widget->setColumnWidth(2, 1 * ui.recent_table_widget->columnWidth(0));
	ui.recent_table_widget->setColumnWidth(3, 3 * ui.recent_table_widget->columnWidth(0));
	ui.recent_table_widget->setColumnWidth(4, 1 * ui.recent_table_widget->columnWidth(0));
	ui.recent_table_widget->setColumnWidth(5, 1.5 * ui.recent_table_widget->columnWidth(0));
	ui.recent_table_widget->setColumnWidth(6, 1 * ui.recent_table_widget->columnWidth(0));
	ui.recent_table_widget->setColumnWidth(7, 5 * ui.recent_table_widget->columnWidth(0));
        
	//connect(ui.recent_table_widget, SIGNAL(itemClicked(QTableWidgetItem*)),
	//	this, SLOT(OnRecentCallTableWidgetItemClicked(QTableWidgetItem*)));

    // history_page
    QStringList headers = { QString::fromLocal8Bit("超声编号"), QString::fromLocal8Bit("审核状态"),
		QString::fromLocal8Bit("住院号"), 
        QString::fromLocal8Bit("患者姓名"), QString::fromLocal8Bit("性别"), QString::fromLocal8Bit("年龄"),
        QString::fromLocal8Bit("检查日期"), QString::fromLocal8Bit("检查部位"), QString::fromLocal8Bit("基层医生") };
    ui.history_table_widget->setColumnCount(headers.count());
    ui.history_table_widget->setHorizontalHeaderLabels(headers);
    ui.history_table_widget->setRowCount(0);

    ui.history_table_widget->setColumnWidth(0, 1.8 * ui.history_table_widget->columnWidth(0));
    ui.history_table_widget->setColumnWidth(4, 0.25 * ui.history_table_widget->columnWidth(0));
    ui.history_table_widget->setColumnWidth(5, 0.25 * ui.history_table_widget->columnWidth(0));
    ui.history_table_widget->setColumnWidth(6, 0.6 * ui.history_table_widget->columnWidth(0));
    ui.history_table_widget->setColumnWidth(7, 1.5 * ui.history_table_widget->columnWidth(0));

    connect(ui.history_table_widget, SIGNAL(itemClicked(QTableWidgetItem*)), 
        this, SLOT(OnHistoryTableWidgetItemClicked(QTableWidgetItem*)));

    ui.history_start_date_edit->setDate(QDate::currentDate().addMonths(-1));
    ui.history_end_date_edit->setDate(QDate::currentDate().addDays(1));

	ui.history_report_status_combo_box->addItem(QString::fromLocal8Bit("全部"));
	ui.history_report_status_combo_box->addItem(QString::fromLocal8Bit("无需审核"));
	ui.history_report_status_combo_box->addItem(QString::fromLocal8Bit("未审核"));
	ui.history_report_status_combo_box->addItem(QString::fromLocal8Bit("审核未通过"));
	ui.history_report_status_combo_box->addItem(QString::fromLocal8Bit("审核通过"));

    ui.history_find_gif_label->setVisible(false);
    ui.history_find_info_label->setVisible(false);

    ui.history_pre_button->setEnabled(false);
    ui.history_next_button->setEnabled(false);
    ui.history_open_button->setEnabled(false);

    connect(ui.history_pre_button, SIGNAL(clicked()), this, SLOT(PrePage()));
    connect(ui.history_next_button, SIGNAL(clicked()), this, SLOT(NextPage()));
    connect(ui.history_query_button, SIGNAL(clicked()), this, SLOT(QueryReport()));
	connect(ui.history_report_query_wait_button, SIGNAL(clicked()), this, SLOT(QueryWaitReport()));
    connect(ui.history_open_button, SIGNAL(clicked()), this, SLOT(OpenReport()));

    connect(ui.history_play_button, SIGNAL(clicked()), this, SLOT(PlayVideo()));
	connect(ui.history_report_confirm_button, SIGNAL(clicked()), this, SLOT(ConfirmCurrentReport()));

    connect(ui.history_report_image_1_label, SIGNAL(doubleClicked(QImage&)), this, SLOT(ReportImageDoubleClicked(QImage&)));
    connect(ui.history_report_image_2_label, SIGNAL(doubleClicked(QImage&)), this, SLOT(ReportImageDoubleClicked(QImage&)));
    connect(ui.history_report_image_3_label, SIGNAL(doubleClicked(QImage&)), this, SLOT(ReportImageDoubleClicked(QImage&)));

    QMovie *movie = new QMovie(":/UltrasoundConsultant/Resources/loading.gif");
    ui.history_find_gif_label->setMovie(movie);
    movie->start();

    // my_page widgets
    ui.my_sex_combo_box->addItem(QString::fromLocal8Bit("男"));
    ui.my_sex_combo_box->addItem(QString::fromLocal8Bit("女"));
    ui.my_pass_button->setEnabled(false);
    connect(ui.my_old_pass_line_edit, SIGNAL(textChanged(const QString &)),
        this, SLOT(OnMyPassTextChanged(const QString &)));
    connect(ui.my_new_pass_line_edit, SIGNAL(textChanged(const QString &)),
        this, SLOT(OnMyPassTextChanged(const QString &)));
    connect(ui.my_new_pass_confirm_line_edit, SIGNAL(textChanged(const QString &)),
        this, SLOT(OnMyPassTextChanged(const QString &)));
    connect(ui.my_pass_button, SIGNAL(clicked()), this, SLOT(UpdatePassword()));
    connect(ui.my_submit_button, SIGNAL(clicked()), this, SLOT(UpdateProfile()));
    connect(ui.my_sign_button, SIGNAL(clicked()), this, SLOT(UploadSignImage()));


    connect(this, SIGNAL(KickOutSignal()), this, SLOT(KickOutSlot()));
    connect(this, SIGNAL(VChatConnectedSignal()), this, SLOT(VChatConnectedSlot()));
    connect(this, SIGNAL(VChatConnectLogoutSignal()), this, SLOT(VChatConnectLogoutSlot()));
    connect(this, SIGNAL(VChatConnectErrorSignal(const QString&)),
        this, SLOT(VChatConnectErrorSlot(const QString&)));
    connect(this, SIGNAL(ReceivedVChatInviteSignal(const QString&, const QString&)), 
        this, SLOT(ReceivedVChatInviteSlot(const QString&, const QString&)));
    connect(this, SIGNAL(HangupSignal()), this, SLOT(HangupSlot()));
    connect(this, SIGNAL(CapturedVideoDataSignal(const QImage&)),
        this, SLOT(CapturedVideoDataSlot(const QImage&)));
    connect(this, SIGNAL(ReceivedVideoDataSignal(const QImage&)), 
        this, SLOT(ReceivedVideoDataSlot(const QImage&)));
    connect(this, SIGNAL(RealtimeStatsInfoSignal(const QString&, const QString&)), 
        this, SLOT(RealtimeStatsInfoSlot(const QString&, const QString&)));
    connect(this, SIGNAL(AudioVolumeNotifySignal(const int, const int)), 
        this, SLOT(AudioVolumeNotifySlot(const int, const int)));
    connect(this, SIGNAL(VchatSyncNotifySignal(bool)),
        this, SLOT(VchatSyncNotifySlot(bool)));
    connect(this, SIGNAL(GetSuperiorUserInfoSuccessedSignal()), this, SLOT(GetSuperiorUserInfoSuccessedSlot()));
    connect(this, SIGNAL(UpdatePasswordSuccessedSignal()), this, SLOT(UpdatePasswordSuccessedSlot()));
    connect(this, SIGNAL(UpdatePasswordFailedSignal(const QString&)), 
        this, SLOT(UpdatePasswordFailedSlot(const QString&)));
    connect(this, SIGNAL(UpdateSuperiorUserInfoSuccessedSignal()), 
        this, SLOT(UpdateSuperiorUserInfoSuccessedSlot()));
    connect(this, SIGNAL(UpdateSuperiorUserInfoFailedSignal(const QString&)),
        this, SLOT(UpdateSuperiorUserInfoFailedSlot(const QString&)));
    connect(this, SIGNAL(QueryReportSuccessedSignal()), this, SLOT(QueryReportSuccessedSlot()));
    connect(this, SIGNAL(QueryReportFailedSignal()), this, SLOT(QueryReportFailedSlot()));
    connect(this, SIGNAL(DownloadFileSuccessedSignal(const QString&, const QString&, const QImage&)), 
        this, SLOT(DownloadFileSuccessedSlot(const QString&, const QString&, const QImage&)));
    connect(this, SIGNAL(DownloadFileFailedSignal(const QString&, const QString&)),
        this, SLOT(DownloadFileFailedSlot(const QString&, const QString&)));

	connect(this, SIGNAL(ConfirmReportSuccessedSignal()), this, SLOT(ConfirmReportSuccessedSlot()));
	connect(this, SIGNAL(ConfirmReportFailedSignal(const QString&)),
		this, SLOT(ConfirmReportFailedSlot(const QString&)));

	connect(this, SIGNAL(receiveSuperiorMessage(const QString)), this, SLOT(onReceiveSuperiorMessage(const QString)));
}

UltrasoundConsultant::~UltrasoundConsultant(){
    m_io_worker.Stop();

    if (m_timer) {
        delete m_timer;
        m_timer = NULL;
    }

    if (m_player_process_handle != NULL) {
        ::TerminateProcess(m_player_process_handle, 0);
        m_player_process_handle = NULL;
    }
}


void UltrasoundConsultant::Initialize() {
    m_user_path = QStandardPaths::writableLocation(
        QStandardPaths::AppLocalDataLocation);
    std::string current_uid = core::CoreService::GetInstance()->GetCurrentUid();
    m_user_path.append("/users/" + QString::fromStdString(current_uid));

    QDir dir = QDir::root();
    dir.mkpath(m_user_path);

	RecentCall::getInstance().load(m_user_path);

    nim::DeviceEventHandler::DeviceInfo device_info;
    device_info.type = nim::DeviceEventHandler::DeviceTypeVideo;
    nim::NimService::GetInstance()->StartDevice(device_info);

    IOThreadWorker::StartParams params;
    params.handler = this;
    params.keep_log_files_month = 1;
    params.keep_report_files_month = 1;
    params.user_path = m_user_path;
    params.log_path = Config::GetInstance()->GetLogFilePath();
    m_io_worker.Start(params);

    UpdateVChatUI(Idle);
}

void UltrasoundConsultant::closeEvent(QCloseEvent *event) {
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

// nim::ClientEventHandler
void UltrasoundConsultant::OnLogin(const std::list<OtherClientInfo>& clients) {}
void UltrasoundConsultant::OnLogout() {}
void UltrasoundConsultant::OnLoginInfo(Info info) {}
void UltrasoundConsultant::OnLoginError(Error error, int code, const std::string& message) {}

void UltrasoundConsultant::OnKickOut(ClientType client_type, KickReason kick_reason) {
    emit KickOutSignal();
}

// nim::VChatEventHandler
void UltrasoundConsultant::OnVChatChannelCreated(const int64_t channel_id) { }
void UltrasoundConsultant::OnVChatStartError(int code) { 
    emit VChatConnectErrorSignal(QString::fromLocal8Bit("呼叫失败。"));
}

void UltrasoundConsultant::OnVChatConnected(const int64_t channel_id) { 
    emit VChatConnectedSignal();
}

void UltrasoundConsultant::OnVChatConnectLogout() { 
    emit VChatConnectLogoutSignal();
}

void UltrasoundConsultant::OnVChatConnectError(const int code) { 
    emit VChatConnectErrorSignal(QString::fromLocal8Bit("通话链接错误！(code = ") +
        QString::number(code) + QString::fromLocal8Bit(")"));
}

void UltrasoundConsultant::OnCalleeRejected() { 
    emit VChatConnectErrorSignal(QString::fromLocal8Bit("对方拒绝了你的呼叫。"));
}

void UltrasoundConsultant::OnControlNotify(const std::string& uid, ControlType type) { 
}

void UltrasoundConsultant::OnReceivedVChatInvite(const int64_t channel_id, const std::string& uid,
    const std::string& custom_info) { 
    emit ReceivedVChatInviteSignal(QString::fromStdString(uid),
        QString::fromStdString(custom_info));
}

void UltrasoundConsultant::OnHangup() { 
    emit HangupSignal();
}

void UltrasoundConsultant::OnCapturedVideoData(unsigned __int64 time, const char* data,
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

    emit CapturedVideoDataSignal(image);
}

void UltrasoundConsultant::OnReceivedVideoData(unsigned __int64 time, const char* data,
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

    emit ReceivedVideoDataSignal(image);
}

void UltrasoundConsultant::OnRealtimeStatsInfo(const StatsInfo& info) { 
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

    emit RealtimeStatsInfoSignal(videoInfo, audioInfo);
}

void UltrasoundConsultant::OnSessionNetStatus(const std::string& uid,
    const SessionNetStatus status) { }
void UltrasoundConsultant::OnVChatOptError(const OptType type, const int code) { }

void UltrasoundConsultant::OnAudioVolumeNotify(const int32_t self_volume,
    const int32_t other_volume) { 
    emit AudioVolumeNotifySignal(self_volume, other_volume);
}

void UltrasoundConsultant::OnVchatSyncNotify(bool accept) {
    emit VchatSyncNotifySignal(accept);
}

// core::UserEventHandler
void UltrasoundConsultant::OnGetPrimaryUserInfoSuccessed(const core::PrimaryUser& user) {
	m_currentVChatUser = user;

	QMutexLocker locker(&vchat_end_session_mutex);
	
	QString uid = QString::fromStdString(user.GetUid());
	m_current_vchat_userinfo_completed[uid] = true;

	if (vchat_end_sessions.contains(uid)) {
		MakeRecentCallRecord(vchat_end_sessions.take(uid));
	}
}
void UltrasoundConsultant::OnGetPrimaryUserInfoFailed(int error_code, const std::string& message) { }

void UltrasoundConsultant::OnGetSuperiorUserInfoSuccessed(const core::SuperiorUser& user) { 
    emit GetSuperiorUserInfoSuccessedSignal();
}

void UltrasoundConsultant::OnGetSuperiorUserInfoFailed(int error_code, const std::string& message) { }

void UltrasoundConsultant::OnUpdatePasswordSuccessed(const std::string& uid, const std::string& new_password) { 
    Config::GetInstance()->UpdateUserPassword(QString::fromStdString(uid),
        QString::fromStdString(new_password));

    emit UpdatePasswordSuccessedSignal();
}

void UltrasoundConsultant::OnUpdatePasswordFailed(int error_code, const std::string& message) { 
    emit UpdatePasswordFailedSignal(QString::fromStdString(message) + QString::fromLocal8Bit("(error_code = ") +
        QString::number(error_code) + QString::fromLocal8Bit(")"));
}

void UltrasoundConsultant::OnQuerySubscriberSuperiorUserSuccessed(std::list<core::SuperiorUser> users) { }
void UltrasoundConsultant::OnQuerySubscriberSuperiorUserFailed(int error_code, const std::string& message) { }
void UltrasoundConsultant::OnQuerySuperiorUserSuccessed(std::list<core::SuperiorUser> users) { }
void UltrasoundConsultant::OnQuerySuperiorUserFailed(int error_code, const std::string& message) { }
void UltrasoundConsultant::OnUpdatePrimaryUserInfoSuccessed() { }
void UltrasoundConsultant::OnUpdatePrimaryUserInfoFailed(int error_code, const std::string& message) { }

void UltrasoundConsultant::OnUpdateSuperiorUserInfoSuccessed() {
    emit UpdateSuperiorUserInfoSuccessedSignal();
}

void UltrasoundConsultant::OnUpdateSuperiorUserInfoFailed(int error_code, const std::string& message) {
    emit UpdateSuperiorUserInfoFailedSignal(QString::fromStdString(message) + QString::fromLocal8Bit("(error_code = ") +
        QString::number(error_code) + QString::fromLocal8Bit(")"));
}

void UltrasoundConsultant::OnQueryControlPrimaryUserSuccessed(std::list<core::PrimaryUser> users) {
    m_control_primary_users.clear();
    m_control_primary_users = users;

    emit QueryControlPrimaryUserSuccessedSignal();
}

void UltrasoundConsultant::OnQueryControlPrimaryUserFailed(int error_code, const std::string& message) {
    m_control_primary_users.clear();

    emit QueryControlPrimaryUserFailedSignal();
}

// core::ReportEventHandler
void UltrasoundConsultant::OnCreateReportSuccessed() { }
void UltrasoundConsultant::OnCreateReportFailed(int error_code, const std::string& message) { }
void UltrasoundConsultant::OnUpdateReportSuccessed() { }
void UltrasoundConsultant::OnUpdateReportFailed(int error_code, const std::string& message) { }

void UltrasoundConsultant::OnConfirmReportSuccessed(const core::ConfirmReportData& data) {

	QJsonObject message_object;
	message_object["superior_id"] = QString::fromStdString(data.superior_id);
	message_object["superior_name"] = QString::fromStdString(data.superior_name);
	message_object["uid"] = QString::fromStdString(data.uid);
	message_object["content"] = QString::fromStdString(data.content);
	message_object["conclusion"] = QString::fromStdString(data.conclusion);
	message_object["confirm_status"] = data.confirm_status;
	message_object["confirm_msg"] = QString::fromStdString(data.confirm_msg);

	QJsonObject root_object;
	root_object["message_id"] = kUltrasoundMessageIdReportConfirmed;
	root_object.insert("message", message_object);

	QJsonDocument document;
	document.setObject(root_object);
	QByteArray message = document.toJson(QJsonDocument::Compact);

	nim::SystemMessage sys_msg;
	sys_msg.send_uid = core::CoreService::GetInstance()->GetCurrentUid();
	sys_msg.receive_uid = data.primary_id;
	sys_msg.message = message;

	nim::NimService::GetInstance()->SendSystemMessage(sys_msg);

	emit ConfirmReportSuccessedSignal();
}

void UltrasoundConsultant::OnConfirmReportFailed(int error_code, const std::string& message) { 
	emit ConfirmReportFailedSignal(QString::fromStdString(message));
}

void UltrasoundConsultant::OnQueryReportSuccessed(std::list<core::Report> reports) { 
    m_current_report_uid.clear();
    m_reports.clear();
    m_reports = reports;

    emit QueryReportSuccessedSignal();
}

void UltrasoundConsultant::OnQueryReportFailed(int error_code, const std::string& message) { 
    m_current_report_uid.clear();
    m_reports.clear();

    emit QueryReportFailedSignal();
}

void UltrasoundConsultant::OnDownloadFileSuccessed(const IOThreadFileInfo& file_info) {
    QImage image;
    if (image.load(file_info.file_path, "PNG")) {
        emit DownloadFileSuccessedSignal(file_info.report_uid, file_info.file_path, image);
    }
}

void UltrasoundConsultant::OnDownloadFileFailed(const IOThreadFileInfo& file_info) {
    emit DownloadFileFailedSignal(file_info.report_uid, file_info.file_path);
}

void UltrasoundConsultant::OnReceiveMessage(const nim::SystemMessage& sys_msg)
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
}


void UltrasoundConsultant::SetDisturb() {
    if (m_do_not_disturb == 1) {
        ui.disturb_button->setIcon(
          QIcon(":/UltrasoundConsultant/Resources/disturb_disable.png"));
        ui.disturb_button->setStyleSheet("color: rgb(0, 0, 0);");
        ui.avatar_label->setPixmap(
          QPixmap(":/UltrasoundConsultant/Resources/user.png"));
        ui.consultant_video_label->setStyleSheet("background-color: rgb(0, 0, 0); \
          color: rgb(232, 232, 232); ");
        ui.consultant_video_label->setText(QString::fromLocal8Bit("未会诊"));
        m_do_not_disturb = 0;
    }
    else if (m_do_not_disturb == 0) {
        ui.disturb_button->setIcon(
          QIcon(":/UltrasoundConsultant/Resources/disturb_enable.png"));
        ui.disturb_button->setStyleSheet("color: rgb(255, 0, 0);");
        ui.avatar_label->setPixmap(
          QPixmap(":/UltrasoundConsultant/Resources/user_disturb.png"));
        ui.consultant_video_label->setStyleSheet("background-color: rgb(0, 0, 0); \
          color: rgb(255, 0, 0); ");
        ui.consultant_video_label->setText(QString::fromLocal8Bit(
          "您当前处于免打扰模式，基层医生无法联系您进行会诊！"));
        m_do_not_disturb = 1;
    }
}


void UltrasoundConsultant::OnRightStackedWidgetCurrentChanged(int index) {
    switch (index)
    {
    case 0: { // consultant_page

        break;
    }
    case 1: { // recent_page
		UpdateRecentCallPageUI();
        break;
    }
    case 2: { // history_page

        break;
    }
    case 3: { // my_page
        UpdateMyPageUI();

        break;
    }
    case 4: { // settings_page

        break;
    }
    default:
        break;
    }
}

void UltrasoundConsultant::OnConsultantControlItemClicked(QTableWidgetItem *item) {
    int row = item->row();
    QTableWidgetItem* id_item = ui.consultant_control_table_widget->item(row, 0);
    QTableWidgetItem* name_item = ui.consultant_control_table_widget->item(row, 1);
    if (id_item && !id_item->text().isEmpty()) {
        m_current_vchat_uid = id_item->text();
        m_current_vchat_name = name_item ? name_item->text() : "";

        ui.consultant_control_call_button->setEnabled(true);
    }
    else {
        ui.consultant_control_call_button->setEnabled(false);
    }
}

void UltrasoundConsultant::QueryControlPrimaryUserSuccessedSlot() {

    ui.consultant_control_table_widget->setRowCount(0);
    ui.consultant_control_call_button->setEnabled(false);

    ui.consultant_control_table_widget->setRowCount(m_control_primary_users.size());

    int row = 0;
    for (const core::PrimaryUser& user : m_control_primary_users) {
        QTableWidgetItem* id_item = new QTableWidgetItem();
        id_item->setText(QString::fromStdString(user.GetUid()));
        ui.consultant_control_table_widget->setItem(row, 0, id_item);

        QTableWidgetItem* name_item = new QTableWidgetItem();
        name_item->setText(QString::fromStdString(user.GetName()));
        ui.consultant_control_table_widget->setItem(row, 1, name_item);

		QTableWidgetItem* online_item = new QTableWidgetItem();
		if (user.GetOnline() == 0) {
			online_item->setText(QString::fromLocal8Bit("离线"));
			online_item->setForeground(QColor(255, 0, 0));
		}
		else {
			online_item->setText(QString::fromLocal8Bit("在线"));
			online_item->setForeground(QColor(0, 255, 0));
		}
		
		ui.consultant_control_table_widget->setItem(row, 2, online_item);

        QTableWidgetItem* phone_item = new QTableWidgetItem();
        phone_item->setText(QString::fromStdString(user.GetPhone()));
        ui.consultant_control_table_widget->setItem(row, 3, phone_item);

        QTableWidgetItem* department_item = new QTableWidgetItem();
        department_item->setText(QString::fromStdString(user.GetDepartment()));
        ui.consultant_control_table_widget->setItem(row, 4, department_item);

        row++;
    }

#ifdef _DEBUG
    ui.consultant_control_table_widget->setRowCount(m_control_primary_users.size() + 1);
    QTableWidgetItem* id_item = new QTableWidgetItem();
    id_item->setText(QString::fromStdString("pr_victor"));
    ui.consultant_control_table_widget->setItem(row, 0, id_item);

    QTableWidgetItem* name_item = new QTableWidgetItem();
    name_item->setText(QString::fromStdString("victor"));
    ui.consultant_control_table_widget->setItem(row, 1, name_item);

	QTableWidgetItem* online_item = new QTableWidgetItem();
	online_item->setText(QString::fromLocal8Bit("离线"));
	online_item->setBackgroundColor(QColor(255, 0, 0));
	ui.consultant_control_table_widget->setItem(row, 2, online_item);

    QTableWidgetItem* phone_item = new QTableWidgetItem();
    phone_item->setText(QString::fromStdString("43420024420"));
    ui.consultant_control_table_widget->setItem(row, 3, phone_item);

    QTableWidgetItem* department_item = new QTableWidgetItem();
    department_item->setText(QString::fromStdString("test"));
    ui.consultant_control_table_widget->setItem(row, 4, department_item);
#endif
}

void UltrasoundConsultant::QueryControlPrimaryUserFailedSlot() {

}

void UltrasoundConsultant::QueryControlPrimaryUser() {
    core::CoreService::GetInstance()->QueryControlPrimaryUser();
}

void UltrasoundConsultant::VChatStart() {
    m_is_control = true;
    UpdateVChatUI(Waiting);

    Json::Value custom_info;
    custom_info["name"] = core::CoreService::GetInstance()->GetCurrentSuperiorUser().GetName();
    custom_info["uid"] = core::CoreService::GetInstance()->GetCurrentSuperiorUser().GetUid();

	m_beginVChatTime = QDateTime::currentDateTime();
	m_current_vchat_userinfo_completed[m_current_vchat_uid] = false;
	core::CoreService::GetInstance()->GetUserInfo(m_current_vchat_uid.toStdString(), true);

    nim::NimService::GetInstance()->VChatStart("test", custom_info.toStyledString(),
        m_current_vchat_uid.toStdString(), m_current_vchat_uid.toStdString());
}

void UltrasoundConsultant::VChatAnswer() {
    nim::NimService::GetInstance()->VChatCalleeAck(true, "");
}

void UltrasoundConsultant::VChatWait() {
    UpdateVChatUI(Idle);
    nim::NimService::GetInstance()->VChatCalleeAck(false, "");
}

void UltrasoundConsultant::VChatReject() {
	TryToMakeRecentCallRecord(VChatStatus::Reject);
    UpdateVChatUI(Idle);
    nim::NimService::GetInstance()->VChatCalleeAck(false, "");
}

void UltrasoundConsultant::VChatEnd() {
	TryToMakeRecentCallRecord(VChatStatus::End);
    UpdateVChatUI(Idle);
    nim::NimService::GetInstance()->VChatEnd("");
}

void UltrasoundConsultant::UpdateTime() {
    if (ui.consultant_stacked_widget->currentIndex() == Speaking) {
        m_time = m_time.addSecs(1);
        QString text = m_time.toString("mm:ss");
        ui.consultant_lcd_number->display(text);
    }
}

void UltrasoundConsultant::OpenReport() {

    std::string uid = m_current_report_uid.toStdString();
    std::list<core::Report>::iterator it =
        std::find_if(m_reports.begin(), m_reports.end(), [uid](const core::Report& report) {
        return uid == report.GetUid();
    });

    if (it == m_reports.end())
        return;

    core::Report& report = *it;

    ui.history_report_title_label->setText(QString::fromStdString(report.GetTitle()));
    ui.history_report_name_line_edit->setText(QString::fromStdString(report.GetName()));
    ui.history_report_sex_line_edit->setText(QString::fromStdString(report.GetSex()));
    ui.history_report_age_line_edit->setText(QString::number(report.GetAge()));
	ui.history_report_patient_id_line_edit->setText(QString::fromStdString(report.GetPatientId()));
    ui.history_report_check_point_line_edit->setText(QString::fromStdString(report.GetParts()));

	// Add primary hospital to department if ruds_title_request is true.
	if(core::CoreService::GetInstance()->GetCurrentSuperiorUser().GetRudsTitleRequest())
		ui.history_report_department_line_edit->setText(
			QString::fromStdString(report.GetHospital()) + QString::fromStdString(report.GetDepartment()));
	else
	    ui.history_report_department_line_edit->setText(QString::fromStdString(report.GetDepartment()));
	
	ui.history_report_ruds_reason_line_edit->setText(QString::fromStdString(report.GetRudsReason()));
	ui.history_report_ruds_target_line_edit->setText(QString::fromStdString(report.GetRudsTarget()));
	ui.history_report_check_date_line_edit->setText(QString::fromStdString(report.GetCheckDate()));
    ui.history_report_no_line_edit->setText(QString::fromStdString(report.GetUid()));
    
    ui.history_report_description_text_edit->setText(QString::fromStdString(report.GetContent()));
    ui.history_report_conclusion_text_edit->setText(QString::fromStdString(report.GetConclusion()));

	// Widgets related to report status.
	if (report.GetConfirmStatus() == core::CONFIRM_STATUS_NONE || 
		report.GetConfirmStatus() == core::CONFIRM_STATUS_PASS) {

		ui.history_report_confirm_button->setEnabled(false);

		if (report.GetConfirmStatus() == core::CONFIRM_STATUS_NONE) {
			ui.history_report_description_text_edit->setReadOnly(false);
			ui.history_report_conclusion_text_edit->setReadOnly(false);

			ui.history_report_status_label->setText(QString::fromLocal8Bit("无需审核"));
			ui.history_report_status_label->setStyleSheet("color: black");
		}
		else {
			ui.history_report_description_text_edit->setReadOnly(true);
			ui.history_report_conclusion_text_edit->setReadOnly(true);

			ui.history_report_status_label->setText(QString::fromLocal8Bit("审核通过"));
			ui.history_report_status_label->setStyleSheet("color: green");
		}
	}
	else if (report.GetConfirmStatus() == core::CONFIRM_STATUS_WAIT ||
		     report.GetConfirmStatus() == core::CONFIRM_STATUS_REJECT) {
		ui.history_report_description_text_edit->setReadOnly(false);
		ui.history_report_conclusion_text_edit->setReadOnly(false);

		ui.history_report_confirm_button->setEnabled(true);

		if (report.GetConfirmStatus() == core::CONFIRM_STATUS_WAIT) {
			ui.history_report_status_label->setText(QString::fromLocal8Bit("未审核"));
			ui.history_report_status_label->setStyleSheet("color: blue");
		}
		else {
			ui.history_report_status_label->setText(QString::fromLocal8Bit("审核未通过"));
			ui.history_report_status_label->setStyleSheet("color: red");
		}
	}
    
    QFileInfo file_info(QString::fromStdString(report.GetVideo()));
    ui.history_vod_label->setText(file_info.fileName());

    if (ui.history_vod_label->text().isEmpty())
        ui.history_play_button->setEnabled(false);
    else
        ui.history_play_button->setEnabled(true);

    QImage black;
    black.fill(Qt::black);
    ui.history_report_image_1_label->setPixmap(QPixmap::fromImage(black));
    ui.history_report_image_1_label->ClearImage();
    ui.history_report_image_2_label->setPixmap(QPixmap::fromImage(black));
    ui.history_report_image_2_label->ClearImage();
    ui.history_report_image_3_label->setPixmap(QPixmap::fromImage(black));
    ui.history_report_image_3_label->ClearImage();
    m_current_image_label = 1;

    std::list<std::string> images = report.GetImages();
    for (const std::string& image : images) {
        IOThreadFileInfo file_info;
        file_info.report_uid = m_current_report_uid;
        file_info.object_name = QString::fromStdString(image);
        file_info.file_path = m_user_path + "/" + file_info.object_name;

        m_io_worker.DownloadFile(file_info);
    }

    ui.history_stacked_widget->setCurrentIndex(1);
}

void UltrasoundConsultant::QueryReport() {
    m_skip = 0;
	m_query_wait = false;

    QueryReportImpl();
}

void UltrasoundConsultant::QueryWaitReport() {
	m_query_wait = true;

	QueryReportImpl();
}

void UltrasoundConsultant::PrePage() {
    if (m_skip > m_limit)
        m_skip -= m_limit;
    else
        m_skip = 0;

    QueryReportImpl();
}

void UltrasoundConsultant::NextPage() {
    m_skip += m_limit;

    QueryReportImpl();
}

void UltrasoundConsultant::OnHistoryTableWidgetItemClicked(QTableWidgetItem* item) {
    int row = item->row();
    QTableWidgetItem* id_item = ui.history_table_widget->item(row, 0);
    if (id_item && !id_item->text().isEmpty()) {
        m_current_report_uid = id_item->text();
        ui.history_open_button->setEnabled(true);
    }
    else {
        ui.history_open_button->setEnabled(false);
    }
}

void UltrasoundConsultant::OnRecentCallTableWidgetItemClicked(QTableWidgetItem * item)
{
	int row = item->row();
	QTableWidgetItem* id_item = ui.recent_table_widget->item(row, 0);
	if (id_item && !id_item->text().isEmpty()) {
		QString uid = id_item->text();
		std::string send_uid = core::CoreService::GetInstance()->GetCurrentUid();
		std::string username = core::CoreService::GetInstance()->GetCurrentSuperiorUser().GetName();
		QString sendMessage = QString::fromStdString(username).append(QString::fromLocal8Bit(":目前空闲，可以发起远程会诊"));
		
		nim::SystemMessage sys_msg;
		sys_msg.send_uid = send_uid;
		sys_msg.receive_uid = uid.toStdString();
		sys_msg.message = sendMessage.toStdString();

		nim::NimService::GetInstance()->SendSystemMessage(sys_msg);

		QMessageBox::information(this, QString::fromLocal8Bit("多点远程超声"),
			QString::fromLocal8Bit("发送消息成功:目前时间，可以发起远程会诊"),
			QMessageBox::Ok);
	}

}

void UltrasoundConsultant::PlayVideo() {
    std::string uid = m_current_report_uid.toStdString();
    std::list<core::Report>::iterator it =
        std::find_if(m_reports.begin(), m_reports.end(), [uid](const core::Report& report) {
        return uid == report.GetUid();
    });

    if (it == m_reports.end())
        return;

    core::Report& report = *it;
    std::string video_url = report.GetVideo();
    if (video_url.empty())
        return;

    if (m_player_process_handle != NULL) {
        ::TerminateProcess(m_player_process_handle, 0);
        m_player_process_handle = NULL;
    }

    QString player_path = QCoreApplication::applicationDirPath();
    player_path.append("/nls_play.exe file ");
    player_path.append(QString::fromStdString(video_url));

    STARTUPINFO info = { sizeof(info) };
    PROCESS_INFORMATION processInfo;
    if (CreateProcess(NULL, (LPWSTR)player_path.toStdWString().data(), NULL, 
      NULL, TRUE, 0, NULL, NULL, &info, &processInfo))
        m_player_process_handle = processInfo.hProcess;
}

void UltrasoundConsultant::ConfirmCurrentReport() {
	std::string uid = m_current_report_uid.toStdString();
	std::list<core::Report>::iterator it =
		std::find_if(m_reports.begin(), m_reports.end(), [uid](const core::Report& report) {
		return uid == report.GetUid();
	});

	if (it == m_reports.end())
		return;

	core::Report& report = *it;

	QString missing;
	if (ui.history_report_description_text_edit->toPlainText().isEmpty())
		missing = QString::fromLocal8Bit("“描述”");
	else if (ui.history_report_conclusion_text_edit->toPlainText().isEmpty())
		missing = QString::fromLocal8Bit("“诊断提示”");
	if (!missing.isEmpty()) {
		QMessageBox box(
			QMessageBox::Question,
			QString::fromLocal8Bit("多点远程超声"),
			missing + QString::fromLocal8Bit(" 必须填写，不能为空！"),
			QMessageBox::Ok);
		box.exec();
		return;
	}

	if (report.GetConfirmStatus() == core::CONFIRM_STATUS_NONE ||
		report.GetConfirmStatus() == core::CONFIRM_STATUS_PASS) {
		QMessageBox box(
			QMessageBox::Question,
			QString::fromLocal8Bit("多点远程超声"),
			QString::fromLocal8Bit("报告审核状态不正确，请重新查询报告！"),
			QMessageBox::Ok);
		box.exec();
		return;
	}

	report.SetContent(ui.history_report_description_text_edit->toPlainText().toStdString());
	report.SetConclusion(ui.history_report_conclusion_text_edit->toPlainText().toStdString());

	ConfirmReport confirm(report, this);
	int ret = confirm.exec();
	if (ret == QDialog::Accepted) {
		ui.history_report_description_text_edit->setReadOnly(true);
		ui.history_report_conclusion_text_edit->setReadOnly(true);
		ui.history_report_confirm_button->setEnabled(false);

		ui.history_report_status_label->setText(QString::fromLocal8Bit("审核通过"));
		ui.history_report_status_label->setStyleSheet("color: green");
	}
	else if (ret == QDialog::Rejected && report.GetConfirmStatus() == core::CONFIRM_STATUS_REJECT) {
		ui.history_report_status_label->setText(QString::fromLocal8Bit("审核未通过"));
		ui.history_report_status_label->setStyleSheet("color: red");
	}
}

void UltrasoundConsultant::UpdateProfile() {
    if (ui.my_name_line_edit->text().isEmpty()) {
        ui.my_info_label->setText(QString::fromLocal8Bit("必须填写姓名，远程超声报告中会使用！"));
        return;
    }

    ui.my_profile_group_box->setEnabled(false);

    ui.my_info_label->setText(QString::fromLocal8Bit("正在提交 ..."));

    Json::Value data;
    data["uid"] = core::CoreService::GetInstance()->GetCurrentUid();
    data["name"] = ui.my_name_line_edit->text().toStdString();
    data["phone"] = ui.my_phone_line_edit->text().toStdString();
    data["sex"] = ui.my_sex_combo_box->currentText().toStdString();
    data["id"] = ui.my_id_line_edit->text().toStdString();
    data["title"] = ui.my_title_line_edit->text().toStdString();
    data["skill"] = ui.my_skill_line_edit->text().toStdString();
    data["description"] = ui.my_description_line_edit->text().toStdString();

    core::CoreService::GetInstance()->UpdateCurrentUserInfo(data);
}

void UltrasoundConsultant::UploadSignImage() {
  
  QString file_name = QFileDialog::getOpenFileName(this,
                                                  QString::fromLocal8Bit("选择图片文件"),
                                                  "",
                                                   QString::fromLocal8Bit("图片文件 (*.png *.jpg *.bmp)"));

  if (file_name.isNull() || file_name.isEmpty()) {
    return;
  }

  QImage sign_image;
  if (!sign_image.load(file_name))
    return;

  QString sign_image_file = m_user_path + "/sign.png";
  QImage scaledImage = sign_image.scaled(100, 50, Qt::KeepAspectRatio);
  if (scaledImage.isNull() || !scaledImage.save(sign_image_file, "PNG"))
    return;

  ui.my_info_label->setText(QString::fromLocal8Bit("正在上传签名图片 ..."));

  std::string object_name = "sign/" + core::CoreService::GetInstance()->GetCurrentUid() + "/sign.png";
  bool success = oss::OssService::GetInstance()->UploadSignImage(sign_image_file.toStdString(),
                                                              object_name);
  if (!success) {
    ui.my_info_label->setText(QString::fromLocal8Bit("上传签名图片失败！"));
    return;
  }

  int w = ui.my_sign_image_label->minimumWidth();
  int h = ui.my_sign_image_label->minimumHeight();
  ui.my_sign_image_label->setPixmap(
    QPixmap::fromImage(scaledImage).scaled(w, h, Qt::KeepAspectRatio));

  Json::Value data;
  data["uid"] = core::CoreService::GetInstance()->GetCurrentUid();
  std::string url = oss::OssService::GetInstance()->GetSignImageRootUrl() + "/sign/"
    + core::CoreService::GetInstance()->GetCurrentUid() + "/sign.png";
  data["sign_url"] = url;

  core::CoreService::GetInstance()->UpdateCurrentUserInfo(data);
}

void UltrasoundConsultant::UpdatePassword() {
    if (ui.my_new_pass_line_edit->text() != ui.my_new_pass_confirm_line_edit->text()) {
        ui.my_info_label->setText(QString::fromLocal8Bit("密码修改失败，新密码两次输入的内容不一致！"));
        return;
    }

    ui.my_pass_group_box->setEnabled(false);

    ui.my_info_label->setText(QString::fromLocal8Bit("正在提交 ..."));

    core::CoreService::GetInstance()->UpdatePassword(ui.my_old_pass_line_edit->text().toStdString(),
        ui.my_new_pass_line_edit->text().toStdString());
}

void UltrasoundConsultant::OnMyPassTextChanged(const QString &text) {
    if (!ui.my_old_pass_line_edit->text().isEmpty() &&
        !ui.my_new_pass_line_edit->text().isEmpty() &&
        !ui.my_new_pass_confirm_line_edit->text().isEmpty())
        ui.my_pass_button->setEnabled(true);
}

void UltrasoundConsultant::KickOutSlot() {
    m_is_kick_out = true;
    close();
}

void UltrasoundConsultant::VChatConnectedSlot() {
    UpdateVChatUI(Speaking);

    nim::NimService::GetInstance()->StartAudioDevice();
}

void UltrasoundConsultant::VChatConnectLogoutSlot() {
    UpdateVChatUI(Idle);
}

void UltrasoundConsultant::VChatConnectErrorSlot(const QString& message) {
	TryToMakeRecentCallRecord(VChatStatus::Reject);
	UpdateVChatUI(Idle);
	nim::NimService::GetInstance()->VChatEnd("");
    QMessageBox::critical(this, QString::fromLocal8Bit("多点远程超声"), message, QMessageBox::Ok);
}

void UltrasoundConsultant::ReceivedVChatInviteSlot(const QString& uid, const QString& custom_info) {
    if (m_do_not_disturb == 1) {
        nim::NimService::GetInstance()->VChatCalleeAck(false, "");
        return;
    }

    QJsonDocument info_doc(QJsonDocument::fromJson(custom_info.toUtf8()));
    QJsonObject info = info_doc.object();
    m_current_vchat_name = info["name"].toString();
	m_current_vchat_uid = info["uid"].toString();
    m_is_control = false;

	m_beginVChatTime = QDateTime::currentDateTime();
	m_current_vchat_userinfo_completed[m_current_vchat_uid] = false;
	core::CoreService::GetInstance()->GetUserInfo(m_current_vchat_uid.toStdString(), true);

    UpdateVChatUI(Waiting);
}

void UltrasoundConsultant::HangupSlot() {
	TryToMakeRecentCallRecord(VChatStatus::End);
	UpdateVChatUI(Idle);
	nim::NimService::GetInstance()->VChatEnd("");
}

void UltrasoundConsultant::CapturedVideoDataSlot(const QImage& data) {
    int w = ui.consultant_camera_label->minimumWidth();
    int h = ui.consultant_camera_label->minimumHeight();

    ui.consultant_camera_label->setPixmap(
        QPixmap::fromImage(data).scaled(w, h, Qt::KeepAspectRatio));
}

void UltrasoundConsultant::ReceivedVideoDataSlot(const QImage& data) {
    int w = data.width() < ui.consultant_video_label->width() ? data.width() :
        ui.consultant_video_label->width();
    int h = data.height() < ui.consultant_video_label->height() ? data.height() :
        ui.consultant_video_label->height();

    ui.consultant_video_label->setPixmap(
        QPixmap::fromImage(data).scaled(w, h, Qt::KeepAspectRatio));
}

void UltrasoundConsultant::RealtimeStatsInfoSlot(const QString& videoInfo, const QString& audioInfo) {
    QString message;
    message.append(QString::fromLocal8Bit("视频："));
    message.append(videoInfo);
    message.append(QString::fromLocal8Bit("     音频："));
    message.append(audioInfo);

	// TODO: Display realtime info if need
}

void UltrasoundConsultant::AudioVolumeNotifySlot(const int self_volume, const int other_volume) {
    ui.consultant_speaker_frame->resize(other_volume, ui.consultant_speaker_frame->height());
    ui.consultant_mic_frame->resize(self_volume, ui.consultant_mic_frame->height());
}

void UltrasoundConsultant::VchatSyncNotifySlot(bool accept) {
    TryToMakeRecentCallRecord(accept ? VChatStatus::End : VChatStatus::Reject );
    UpdateVChatUI(Idle);
    nim::NimService::GetInstance()->VChatEnd("");
}

void UltrasoundConsultant::GetSuperiorUserInfoSuccessedSlot() {
    UpdateMyPageUI();
}

void UltrasoundConsultant::UpdatePasswordSuccessedSlot() {
    ui.my_info_label->setText(QString::fromLocal8Bit("密码修改成功！"));
    ui.my_old_pass_line_edit->clear();
    ui.my_new_pass_line_edit->clear();
    ui.my_new_pass_confirm_line_edit->clear();

    ui.my_pass_group_box->setEnabled(true);
    ui.my_pass_button->setEnabled(false);
}

void UltrasoundConsultant::UpdatePasswordFailedSlot(const QString& message) {
    ui.my_info_label->setText(QString::fromLocal8Bit("密码修改失败！"));
    ui.my_pass_group_box->setEnabled(true);
}

void UltrasoundConsultant::UpdateSuperiorUserInfoSuccessedSlot() {
    ui.my_info_label->setText(QString::fromLocal8Bit("资料修改成功！"));

    core::CoreService::GetInstance()->GetCurrentUserInfo();

    ui.my_profile_group_box->setEnabled(true);
}

void UltrasoundConsultant::UpdateSuperiorUserInfoFailedSlot(const QString& message) {
    ui.my_info_label->setText(QString::fromLocal8Bit("资料修改失败！") + message);

    ui.my_profile_group_box->setEnabled(true);
}

void UltrasoundConsultant::QueryReportSuccessedSlot() {
    ui.history_find_gif_label->setVisible(false);

    ui.history_query_button->setEnabled(true);
    ui.history_open_button->setEnabled(false);

    if (m_skip < m_limit)
        ui.history_pre_button->setEnabled(false);
    else
        ui.history_pre_button->setEnabled(true);

    if (m_reports.size() <= 0) {
        ui.history_next_button->setEnabled(false);

        ui.history_table_widget->setRowCount(0);

        ui.history_find_info_label->setVisible(true);
        if (m_skip > 0)
            ui.history_find_info_label->setText(QString::fromLocal8Bit("没有更多符合条件的报告了！"));
        else
            ui.history_find_info_label->setText(QString::fromLocal8Bit("没有查询到符合条件的报告！"));
        return;
    }
    else {
        ui.history_find_info_label->setVisible(false);

        if (m_reports.size() % m_limit != 0)
            ui.history_next_button->setEnabled(false);
        else
            ui.history_next_button->setEnabled(true);
    }

    ui.history_table_widget->setRowCount(m_reports.size());

    int row = 0;
    for (const core::Report& report : m_reports) {
        QTableWidgetItem* uid_item = new QTableWidgetItem();
        uid_item->setText(QString::fromStdString(report.GetUid()));
        ui.history_table_widget->setItem(row, 0, uid_item);

		QTableWidgetItem* status_item = new QTableWidgetItem();
		QString status = QString::fromLocal8Bit("无需审核");
		switch (report.GetConfirmStatus())
		{
		case core::CONFIRM_STATUS_WAIT:
			status = QString::fromLocal8Bit("未审核");
			break;
		case core::CONFIRM_STATUS_REJECT:
			status = QString::fromLocal8Bit("审核未通过");
			break;
		case core::CONFIRM_STATUS_PASS:
			status = QString::fromLocal8Bit("审核通过");
			break;
		default:
			break;
		}
		status_item->setText(status);
		ui.history_table_widget->setItem(row, 1, status_item);

		QTableWidgetItem* patient_id_item = new QTableWidgetItem();
		patient_id_item->setText(QString::fromStdString(report.GetPatientId()));
		ui.history_table_widget->setItem(row, 2, patient_id_item);

        QTableWidgetItem* name_item = new QTableWidgetItem();
        name_item->setText(QString::fromStdString(report.GetName()));
        ui.history_table_widget->setItem(row, 3, name_item);

        QTableWidgetItem* sex_item = new QTableWidgetItem();
        sex_item->setText(QString::fromStdString(report.GetSex()));
        ui.history_table_widget->setItem(row, 4, sex_item);

        QTableWidgetItem* age_item = new QTableWidgetItem();
        age_item->setText(QString::number(report.GetAge()));
        ui.history_table_widget->setItem(row, 5, age_item);

        QTableWidgetItem* check_date_item = new QTableWidgetItem();
        check_date_item->setText(QString::fromStdString(report.GetCheckDate()));
        ui.history_table_widget->setItem(row, 6, check_date_item);

        QTableWidgetItem* parts_item = new QTableWidgetItem();
        parts_item->setText(QString::fromStdString(report.GetParts()));
        ui.history_table_widget->setItem(row, 7, parts_item);

        QTableWidgetItem* primary_item = new QTableWidgetItem();
        primary_item->setText(QString::fromStdString(report.GetPrimary()));
        ui.history_table_widget->setItem(row, 8, primary_item);

        row++;
    }
}

void UltrasoundConsultant::QueryReportFailedSlot() {
    ui.history_find_gif_label->setVisible(false);

    m_skip = 0;

    ui.history_query_button->setEnabled(true);
    ui.history_pre_button->setEnabled(false);
    ui.history_next_button->setEnabled(false);
    ui.history_open_button->setEnabled(false);

    ui.history_table_widget->setRowCount(0);

    ui.history_find_info_label->setVisible(true);
    ui.history_find_info_label->setText(QString::fromLocal8Bit("查询报告出错！"));
}

void UltrasoundConsultant::DownloadFileSuccessedSlot(const QString& report_uid, 
    const QString& file_path, const QImage& image) {
    if (m_current_report_uid == report_uid) {

        if (m_current_image_label == 1) {
            int w = ui.history_report_image_1_label->minimumWidth();
            int h = ui.history_report_image_1_label->minimumHeight();
            ui.history_report_image_1_label->SetImage(image);
            ui.history_report_image_1_label->setPixmap(
                QPixmap::fromImage(image).scaled(w, h, Qt::KeepAspectRatio));

            m_current_image_label++;
        }
        else if (m_current_image_label == 2) {
            int w = ui.history_report_image_2_label->minimumWidth();
            int h = ui.history_report_image_2_label->minimumHeight();
            ui.history_report_image_2_label->SetImage(image);
            ui.history_report_image_2_label->setPixmap(
                QPixmap::fromImage(image).scaled(w, h, Qt::KeepAspectRatio));

            m_current_image_label++;
        }
        else if (m_current_image_label == 3) {
            int w = ui.history_report_image_3_label->minimumWidth();
            int h = ui.history_report_image_3_label->minimumHeight();
            ui.history_report_image_3_label->SetImage(image);
            ui.history_report_image_3_label->setPixmap(
                QPixmap::fromImage(image).scaled(w, h, Qt::KeepAspectRatio));

            m_current_image_label++;
        }
    }
}

void UltrasoundConsultant::DownloadFileFailedSlot(const QString& report_uid, const QString& file_path) { }

void UltrasoundConsultant::ConfirmReportSuccessedSlot() {
	QMessageBox::information(this, QString::fromLocal8Bit("多点远程超声"),
		QString::fromLocal8Bit("审核报告成功！\n"), QMessageBox::Ok);
}

void UltrasoundConsultant::ConfirmReportFailedSlot(const QString& message) {
	QMessageBox::critical(this, QString::fromLocal8Bit("多点远程超声"),
		QString::fromLocal8Bit("审核报告出错！\n") + message, QMessageBox::Ok);
}

void UltrasoundConsultant::OnSendMessageClicked(void)
{
	QPushButton *senderObj = qobject_cast<QPushButton*>(sender());
	if (senderObj == nullptr)
	{
		return;
	}
	QModelIndex idx = ui.recent_table_widget->indexAt(QPoint(senderObj->frameGeometry().x(), senderObj->frameGeometry().y()));
	int row = idx.row();
	QTableWidgetItem* id_item = ui.recent_table_widget->item(row, 0);
	if (id_item && !id_item->text().isEmpty()) {
		QString uid = id_item->text();
		std::string send_uid = core::CoreService::GetInstance()->GetCurrentUid();
		std::string username = core::CoreService::GetInstance()->GetCurrentSuperiorUser().GetName();

		QString sendMessage = QString::fromStdString(username).append(QString::fromLocal8Bit(":目前空闲，可以发起远程会诊"));

		nim::SystemMessage sys_msg;
		sys_msg.send_uid = send_uid;
		sys_msg.receive_uid = uid.toStdString();
		sys_msg.message = sendMessage.toStdString();

		nim::NimService::GetInstance()->SendSystemMessage(sys_msg);

		QMessageBox::information(this, QString::fromLocal8Bit("多点远程超声"),
			QString::fromLocal8Bit("发送消息成功:目前空闲，可以发起远程会诊"),
			QMessageBox::Ok);
	}
}

void UltrasoundConsultant::UpdateVChatUI(VChatStatus status) {

    m_current_vchat_status = status;

    switch (status)
    {
    case Idle: {
        m_Sound.stop();
        m_Control_Sound.stop();

		m_time.setHMS(0, 0, 0);

        QImage black;
        black.fill(Qt::black);
        ui.consultant_video_label->setPixmap(QPixmap::fromImage(black));

        ui.status_bar->clearMessage();

        ui.consultant_speaker_frame->resize(0, ui.consultant_speaker_frame->height());
        ui.consultant_mic_frame->resize(0, ui.consultant_mic_frame->height());

        if (m_do_not_disturb == 0) {
            ui.consultant_video_label->setStyleSheet("background-color: rgb(0, 0, 0); \
          color: rgb(232, 232, 232); ");
            ui.consultant_video_label->setText(QString::fromLocal8Bit("未会诊"));
        }
        else if (m_do_not_disturb == 1) {
            ui.consultant_video_label->setStyleSheet("background-color: rgb(0, 0, 0); \
          color: rgb(255, 0, 0); ");
            ui.consultant_video_label->setText(QString::fromLocal8Bit(
                "您当前处于免打扰模式，基层医生无法联系您进行会诊！"));
        }
        ui.consultant_stacked_widget->setCurrentIndex(0);
		QueryControlPrimaryUser();
        break;
    }
    case Waiting: {
        if (m_is_control)
            m_Control_Sound.play();
        else
            m_Sound.play();

        // Bring window to front
        setWindowState(windowState() & ~Qt::WindowMinimized | Qt::WindowActive);

        ui.status_bar->clearMessage();

        ui.consultant_speaker_frame->resize(0, ui.consultant_speaker_frame->height());
        ui.consultant_mic_frame->resize(0, ui.consultant_mic_frame->height());

        if (m_is_control) {
            ui.consultant_video_label->setText(QString::fromLocal8Bit(" 正在呼叫 ") + m_current_vchat_name + QString::fromLocal8Bit(" 进行质控 ..."));
            ui.consultant_answer_button->setVisible(false);
            ui.consultant_wait_button->setVisible(false);

            ui.consultant_reject_button->setText(QString::fromLocal8Bit("挂断"));
        }
        else {
            ui.consultant_video_label->setText(m_current_vchat_name + QString::fromLocal8Bit(" 正在邀请您进行会诊 ..."));
            ui.consultant_answer_button->setVisible(true);
            ui.consultant_wait_button->setVisible(true);

            ui.consultant_reject_button->setText(QString::fromLocal8Bit("拒绝"));
        }

        ui.consultant_calling_name_label->setText(m_current_vchat_name);
        ui.consultant_stacked_widget->setCurrentIndex(1);

        ui.button_list_widget->setCurrentRow(0);
        
        break;
    }
    case Speaking: {
        m_Sound.stop();
        m_Control_Sound.stop();

        ui.consultant_speaker_frame->resize(0, ui.consultant_speaker_frame->height());
        ui.consultant_mic_frame->resize(0, ui.consultant_mic_frame->height());

        if(m_is_control)
            ui.consultant_video_label->setText(QString::fromLocal8Bit("质控进行中 ..."));
        else
            ui.consultant_video_label->setText(QString::fromLocal8Bit("会诊进行中 ..."));
        
        ui.consultant_speaking_name_label->setText(m_current_vchat_name);
        ui.consultant_stacked_widget->setCurrentIndex(2);
		
        m_time.setHMS(0, 0, 0);
        QString text = m_time.toString("mm:ss");
        ui.consultant_lcd_number->display(text);

        if(m_is_control)
            ui.consultant_end_button->setText(QString::fromLocal8Bit("结束质控"));
        else
            ui.consultant_end_button->setText(QString::fromLocal8Bit("结束会诊"));

        break;
    }
    default:
        break;
    }
}

void UltrasoundConsultant::UpdateConsultantPageUI() {

}

void UltrasoundConsultant::UpdateRecentPageUI() {

}

void UltrasoundConsultant::UpdateHistoryPageUI() {

}

void UltrasoundConsultant::UpdateMyPageUI() {
    
    ui.my_uid_value_label->setText(QString::fromStdString(
        core::CoreService::GetInstance()->GetCurrentSuperiorUser().GetUid()));

    QString region = QString::fromStdString(
        core::CoreService::GetInstance()->GetCurrentSuperiorUser().GetProvince()) +
        QString::fromLocal8Bit(" ") +
        QString::fromStdString(
            core::CoreService::GetInstance()->GetCurrentSuperiorUser().GetCity()) +
        QString::fromLocal8Bit(" ") +
        QString::fromStdString(
            core::CoreService::GetInstance()->GetCurrentSuperiorUser().GetCountry());
    ui.my_region_value_label->setText(region);

    ui.my_hospital_value_label->setText(QString::fromStdString(
        core::CoreService::GetInstance()->GetCurrentSuperiorUser().GetHospital()));

    ui.my_name_line_edit->setText(QString::fromStdString(
        core::CoreService::GetInstance()->GetCurrentSuperiorUser().GetName()));

    ui.my_phone_line_edit->setText(QString::fromStdString(
        core::CoreService::GetInstance()->GetCurrentSuperiorUser().GetPhone()));

    ui.my_sex_combo_box->setCurrentText(QString::fromStdString(
        core::CoreService::GetInstance()->GetCurrentSuperiorUser().GetSex()));

    ui.my_id_line_edit->setText(QString::fromStdString(
        core::CoreService::GetInstance()->GetCurrentSuperiorUser().GetId()));

    ui.my_title_line_edit->setText(QString::fromStdString(
        core::CoreService::GetInstance()->GetCurrentSuperiorUser().GetTitle()));

    ui.my_skill_line_edit->setText(QString::fromStdString(
        core::CoreService::GetInstance()->GetCurrentSuperiorUser().GetSkill()));

    ui.my_description_line_edit->setText(QString::fromStdString(
        core::CoreService::GetInstance()->GetCurrentSuperiorUser().GetDescription()));

    QString sign_image_file = m_user_path + "/sign.png";
    QImage image;
    if(image.load(sign_image_file)) {
      int w = ui.my_sign_image_label->minimumWidth();
      int h = ui.my_sign_image_label->minimumHeight();
      ui.my_sign_image_label->setPixmap(
        QPixmap::fromImage(image).scaled(w, h, Qt::KeepAspectRatio));
    }
}

void UltrasoundConsultant::UpdateSettingsPageUI() {

}

void UltrasoundConsultant::UpdateRecentCallPageUI()
{
	QList<RecentCallItem> recentCallItems = RecentCall::getInstance().getCallItems();
	ui.recent_table_widget->setRowCount(recentCallItems.size());
	ui.recent_table_widget->setColumnHidden(0, true);
	int row = 0;
	for (RecentCallItem& callItem : recentCallItems) {
		QTableWidgetItem* id_item = new QTableWidgetItem();
		id_item->setText(callItem.getUid());
		ui.recent_table_widget->setItem(row, 0, id_item);

		QTableWidgetItem* name_item = new QTableWidgetItem();
		name_item->setText(callItem.getName());
		ui.recent_table_widget->setItem(row, 1, name_item);

		QTableWidgetItem* sex_item = new QTableWidgetItem();
		sex_item->setText(callItem.getSex());
		ui.recent_table_widget->setItem(row,2, sex_item);

		QTableWidgetItem* receive_time_item = new QTableWidgetItem();
		receive_time_item->setText(callItem.getReceiveTime());
		ui.recent_table_widget->setItem(row, 3, receive_time_item);

		QTableWidgetItem* receive_state_item = new QTableWidgetItem();
		receive_state_item->setText(callItem.getReceiveState());
		ui.recent_table_widget->setItem(row, 4, receive_state_item);

		QTableWidgetItem* talk_time_item = new QTableWidgetItem();
		talk_time_item->setText(callItem.getTalkTime());
		ui.recent_table_widget->setItem(row, 5, talk_time_item);

		QTableWidgetItem* telephone_item = new QTableWidgetItem();
		telephone_item->setText(callItem.getTelephone());
		ui.recent_table_widget->setItem(row, 6, telephone_item);

		QTableWidgetItem* hospital_item = new QTableWidgetItem();
		hospital_item->setText(callItem.getHospital());
		ui.recent_table_widget->setItem(row, 7, hospital_item);

		QPushButton *pSendMsg = new QPushButton();
		pSendMsg->setIcon(QIcon(":/UltrasoundConsultant/Resources/remind.png"));
		connect(pSendMsg, SIGNAL(clicked()), this, SLOT(OnSendMessageClicked()));
		ui.recent_table_widget->setCellWidget(row, 8, pSendMsg);

		row++;
	}
	
}

void UltrasoundConsultant::TryToMakeRecentCallRecord(VChatStatus status)
{
	QMutexLocker locker(&vchat_end_session_mutex);
	if (m_current_vchat_userinfo_completed[m_current_vchat_uid] == false) {
		vchat_end_sessions.insert(m_current_vchat_uid, {status, m_time});
		return;
	}
	MakeRecentCallRecord({ status, m_time });
}
void UltrasoundConsultant::MakeRecentCallRecord(VChatSession session)
{
	RecentCallItem callItem;

	callItem.setUid(QString::fromStdString(m_currentVChatUser.GetUid()));
	//姓名
	callItem.setName(m_current_vchat_name);

	//性别
	callItem.setSex(QString::fromStdString(m_currentVChatUser.GetSex()));

	//开始时间
	callItem.setReceiveTime(m_beginVChatTime.toString("yyyy-MM-dd hh:mm:ss"));

	// 通话时长
	callItem.setTalkTime(session.talk_time.toString("mm:ss"));

	//通话状态
	if (session.status == VChatStatus::Reject) {
		callItem.setReceiveState(QString::fromLocal8Bit("拒绝"));
	}
	else {
		callItem.setReceiveState(QString::fromLocal8Bit("接通"));
	}

	//电话号码
	callItem.setTelephone(QString::fromStdString(m_currentVChatUser.GetPhone()));

	//基层医院
	callItem.setHospital(QString::fromStdString(m_currentVChatUser.GetHospital()));

	RecentCall::getInstance().addCallItem(callItem);
	RecentCall::getInstance().flush();
	
	m_current_vchat_userinfo_completed[m_current_vchat_uid] = false;
}

void UltrasoundConsultant::QueryReportImpl() {
    ui.history_stacked_widget->setCurrentIndex(0);

    ui.history_query_button->setEnabled(false);
    ui.history_pre_button->setEnabled(false);
    ui.history_next_button->setEnabled(false);
    ui.history_open_button->setEnabled(false);
    ui.history_table_widget->setRowCount(0);

    ui.history_find_gif_label->setVisible(true);
    ui.history_find_info_label->setText(QString::fromLocal8Bit("正在查询，请稍后 ..."));
    ui.history_find_info_label->setVisible(true);

    std::map<std::string, std::string> conditions;
    conditions["skip"] = std::to_string(m_skip);
    conditions["limit"] = std::to_string(m_limit);
    conditions["superior_id"] = core::CoreService::GetInstance()->GetCurrentUid();

	if (m_query_wait) {
		conditions["confirm_status"] = QUrl::toPercentEncoding(
			QString::number(core::CONFIRM_STATUS_WAIT)).constData();
	}
	else {
		if (!ui.history_uid_line_edit->text().isEmpty()) {
			// Report uid should be unique, so don't add other conditions
			conditions["uid"] = QUrl::toPercentEncoding(ui.history_uid_line_edit->text()).constData();
		}
		else {
			QDateTime start = ui.history_start_date_edit->dateTime();
			start = start.addSecs(8 * 60 * 60); // UTC + 8
			conditions["begin"] = std::to_string(start.toMSecsSinceEpoch() / 1000);

			QDateTime end = ui.history_end_date_edit->dateTime();
			end = end.addSecs(8 * 60 * 60);
			conditions["end"] = std::to_string(end.toMSecsSinceEpoch() / 1000);

			if (!ui.history_primary_line_edit->text().isEmpty())
				conditions["primary"] = QUrl::toPercentEncoding(ui.history_primary_line_edit->text()).constData();

			if (!ui.history_name_line_edit->text().isEmpty())
				conditions["name"] = QUrl::toPercentEncoding(ui.history_name_line_edit->text()).constData();

			if (!ui.history_hospital_line_edit->text().isEmpty())
				conditions["hospital"] = QUrl::toPercentEncoding(ui.history_hospital_line_edit->text()).constData();

			if (!ui.history_patient_id_line_edit->text().isEmpty())
				conditions["patient_id"] = QUrl::toPercentEncoding(ui.history_patient_id_line_edit->text()).constData();

			if (ui.history_report_status_combo_box->currentIndex() > 0)
				conditions["confirm_status"] = QUrl::toPercentEncoding(
					QString::number(ui.history_report_status_combo_box->currentIndex() - 1)).constData();
		}
	}

    core::CoreService::GetInstance()->QueryReport(conditions);
}

void UltrasoundConsultant::OnQuerySubscriberSuperiorUserOnlineStatusSuccessed(std::list<core::SuperiorUserStatus> users)
{
}

void UltrasoundConsultant::OnQuerySubscriberSuperiorUserOnlineStatusFailed(int error_code, const std::string & message)
{
}

void UltrasoundConsultant::ReportImageDoubleClicked(QImage& image) {
    if (m_image_viewer == nullptr) {
        m_image_viewer = new ImageViewer(this);
    }

    m_image_viewer->setImage(image);
    m_image_viewer->show();
}

void UltrasoundConsultant::onReceiveSuperiorMessage(const QString message)
{
	ShowSystemMessageInStatusBar(message);
}

void UltrasoundConsultant::OpenSystemMessageWindow() {
	ui.status_bar->clearMessage();
	m_system_message_viewer->show();
}

void UltrasoundConsultant::ShowSystemMessageInStatusBar(const QString& message, bool add_ts) {
	QString display_msg = QString::fromLocal8Bit("系统消息：");;
	QString ts;
	if (add_ts) {
		QDateTime now = QDateTime::currentDateTime();
		ts = now.toString("yyyy-MM-dd hh:mm:ss ");
		display_msg += ts;
	}
	display_msg += message;
	ui.status_bar->showMessage(display_msg);

	m_system_message_viewer->AddMessage(ts + message);
}