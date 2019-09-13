#include "report_thread_worker.h"

#include <QFileInfo>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>

#include <assert.h>

#include "core_service/core_service.h"
#include "nim_service/nim_service.h"
#include "oss_service/oss_service.h"
#include "qr_code_generator.h"
#include "report_html.h"
#include "ultrasound.h"

ReportThreadWorker::ReportThreadWorker(ultrasound* u, bool auto_save)
    : m_u(u), m_auto_save(auto_save) {
    assert(m_u);

    m_report_save_event = CreateEvent(NULL, TRUE, TRUE, NULL);
}

ReportThreadWorker::~ReportThreadWorker() {
    CloseHandle(m_report_save_event);
}

// core::ReportEventHandler
void ReportThreadWorker::OnCreateReportSuccessed() {
    SetEvent(m_report_save_event);
}

void ReportThreadWorker::OnCreateReportFailed(int error_code, const std::string& message) {
    SetEvent(m_report_save_event);
    m_has_error = true;
    m_error_code = error_code;
    m_message = QString::fromStdString(message);
}

void ReportThreadWorker::OnUpdateReportSuccessed() {
    SetEvent(m_report_save_event);
}

void ReportThreadWorker::OnUpdateReportFailed(int error_code, const std::string& message) {
    SetEvent(m_report_save_event);
    m_has_error = true;
    m_error_code = error_code;
    m_message = QString::fromStdString(message);
}

void ReportThreadWorker::OnConfirmReportSuccessed(const core::ConfirmReportData& data) {}
void ReportThreadWorker::OnConfirmReportFailed(int error_code, const std::string& message) {}

void ReportThreadWorker::OnQueryReportSuccessed(std::list<core::Report> reports) {}
void ReportThreadWorker::OnQueryReportFailed(int error_code, const std::string& message) {}

void ReportThreadWorker::Upload() {
    ResetEvent(m_report_save_event);

    core::Report& report = core::CoreService::GetInstance()->GetCurrentReport();

    int saved = 0;
    int total = m_u->m_capture_image_list.size() + 3; // 1 for qrcode, 1 for pdf, 1 for report json
	bool success = false;

	QString pdf_file_name = m_u->ui.ultrasound_number_edit->text() + ".pdf";
	QString currentMonth = QDate::currentDate().toString("yyyyMM");
	QString current_primary_user_uid = QString::fromStdString(
		core::CoreService::GetInstance()->GetCurrentPrimaryUser().GetUid());
	QString pdf_object_name = QString::fromStdString(oss::OssService::GetInstance()->GetReportPdfDirectory())
		+ "/" + currentMonth + "/" + current_primary_user_uid + "/" + pdf_file_name;

	QString pdf_file_url = QString::fromStdString(report.GetReportUrl());
	if (pdf_file_url.isEmpty() || !pdf_file_url.startsWith("http")) {
		// Build pdf url
		pdf_file_url = QString::fromStdString(oss::OssService::GetInstance()->GetReportPdfRootUrl()) 
			+ "/" + pdf_object_name;

		report.SetReportUrl(pdf_file_url.toStdString());
	}
	else {
		QString root_url = QString::fromStdString(oss::OssService::GetInstance()->GetReportPdfRootUrl());
		pdf_object_name = pdf_file_url.right(pdf_file_url.size() - root_url.size() - 1);
	}

	// Save QR Code image
	QImage qr_code_image(110, 110, QImage::Format_ARGB32);

	if (QrCodeGenerator::GenerateImage(pdf_file_url, &qr_code_image))
	{
		QString qr_code_image_file_name = m_u->m_report_path + "/" + "qr_code.png";

		if (QFileInfo(qr_code_image_file_name).exists() == false) {
			qr_code_image.save(qr_code_image_file_name, "PNG");
		}
	}

	saved++;
	int process_value = saved * 100 / total;

    std::list<std::string> images;

    // Save images to local file and oss.
    std::map<QString, QImage>::iterator it = m_u->m_capture_image_list.begin();
    for (; it != m_u->m_capture_image_list.end(); it++) {
        QImage& image = it->second;
        QString file_name = m_u->m_report_path + "/" + it->first + ".png";

        if (QFileInfo(file_name).exists() == false) {
            image.save(file_name, "PNG");
        }

        if (std::find(m_u->m_checked_image_list.begin(), 
            m_u->m_checked_image_list.end(), it->first) != m_u->m_checked_image_list.end()) {
            
            std::string object_name = m_u->ui.ultrasound_number_edit->text().toStdString() +
                "/" + it->first.toStdString() + ".png";

            if(report.HasImage(object_name)){
                continue;
            }

            success = oss::OssService::GetInstance()->UploadReportImage(
              m_u->ui.ultrasound_number_edit->text().toStdString(), 
              file_name.toUtf8().constData(), object_name);

            if (success) {
                images.push_back(object_name);
            }
            else {
                m_has_error = true;
            }
        }

        saved++;
        process_value = saved * 100 / total;
        if (process_value >= 100)
            process_value = 99; // Prevent thread exit here.
        emit ProgressValueSignal(process_value);
    }

    if (images.size() > 0) {
        report.SetEndpoint(oss::OssService::GetInstance()->GetCurrentEndpoint());
        report.SetBucket(oss::OssService::GetInstance()->GetReportBucket());
        
        report.AddImages(images);
    }

	// Save report pdf
	QString local_pdf_file_path = m_u->m_report_path + "/" + pdf_file_name;
	QPrinter printer(QPrinter::HighResolution);
	printer.setPageSize(QPrinter::A4);
	printer.setPageMargins(2, 2, 2, 2, QPrinter::Millimeter);
	printer.setOutputFileName(local_pdf_file_path);

	Json::Value data;
	report.SaveToJson(data, false);
	QJsonDocument loadDoc(QJsonDocument::fromJson(data.toStyledString().c_str()));
	QJsonObject json_report = loadDoc.object();

	if (!json_report.isEmpty()) {
		QTextDocument document;
		document.setHtml(ReportHtml::BuildHtml(json_report, m_u->m_report_path, report.GetHospitalId(), report.GetSuperiorHospitalId()));
		document.print(&printer);
	}


	success = oss::OssService::GetInstance()->UploadReportPdf(
		m_u->ui.ultrasound_number_edit->text().toStdString(),
		local_pdf_file_path.toUtf8().constData(), pdf_object_name.toStdString());

	if (!success) {
		m_has_error = true;
	}

	saved++;
	process_value = saved * 100 / total;
	if (process_value >= 100)
		process_value = 99; // Prevent thread exit here.
	emit ProgressValueSignal(process_value);

	// Save report json
    core::CoreService::GetInstance()->SaveReport();

	if (report.GetConfirmStatus() == core::CONFIRM_STATUS_WAIT) {

	}

	if (!m_auto_save && report.GetChannelCreated() && 
		(report.GetConfirmStatus() == core::CONFIRM_STATUS_REJECT ||
		 report.GetConfirmStatus() == core::CONFIRM_STATUS_WAIT)) {
		QString status = QString::fromLocal8Bit("审核未通过");
		if(report.GetConfirmStatus() == core::CONFIRM_STATUS_WAIT)
			status = QString::fromLocal8Bit("未审核");

		QString sendMessage = QString::fromStdString(
			core::CoreService::GetInstance()->GetCurrentPrimaryUser().GetName()) +
			QString::fromLocal8Bit(" 更新了 *") + status + QString::fromLocal8Bit("* 报告（超声编号：") + 
				QString::fromStdString(report.GetUid()) + QString::fromLocal8Bit("）。");

		nim::SystemMessage sys_msg;
		sys_msg.send_uid = core::CoreService::GetInstance()->GetCurrentPrimaryUser().GetUid();
		sys_msg.receive_uid = report.GetSuperiorId();
		sys_msg.message = sendMessage.toStdString();

		nim::NimService::GetInstance()->SendSystemMessage(sys_msg);
	}

    WaitForSingleObject(m_report_save_event, INFINITE);

    // Download supervior sign image if necessary
    std::string superior_id = report.GetSuperiorId();
    if (m_auto_save && !superior_id.empty()) {
      std::string image_file = m_u->m_report_path.toStdString() + "/sign.png";
      std::string object_name = "sign/" + superior_id + "/sign.png";
      oss::OssService::GetInstance()->DownloadSignImage(image_file, object_name);
    }

    emit ProgressValueSignal(100);
}

void ReportThreadWorker::Download() {
    core::Report& report = core::CoreService::GetInstance()->GetCurrentReport();
    std::list<std::string> images = report.GetImages();
    
    int downloaded = 0;
    int total_downloaded = images.size();
    for (const std::string& image : images) {
        std::string image_file = m_u->m_user_path.toStdString() + "/" + image;

        QFileInfo fileInfo(QString::fromStdString(image_file));
        m_u->m_checked_image_list.push_back(fileInfo.baseName());
        if (fileInfo.exists() == false) {
            bool success = oss::OssService::GetInstance()->DownloadReportImage(image_file, image);

            if (!success) 
                m_has_error = true;
        }

        downloaded++;
        int process_value = (downloaded * 100 / total_downloaded) / 2;
        if (process_value >= 100)
            process_value = 99; // Prevent thread exit here.
        emit ProgressValueSignal(process_value);
    }

    QFileInfoList fileInfoList = m_u->GetReportCapturedImages();
    int loaded = 0;
    int total_loaded = fileInfoList.count();
    int max = 1;
    for (const QFileInfo& fileInfo : fileInfoList) {
        QImage image;
        QString file_base_name = fileInfo.baseName();
        if (!image.load(fileInfo.filePath(), "PNG")) {
            m_has_error = true;
        }
        else {
            m_u->m_capture_image_list[file_base_name] = image;
        }

        int file_name_number = fileInfo.baseName().toInt();
        if (file_name_number > max) {
            max = file_name_number;
        }

        loaded++;
        int process_value = (loaded * 100 / total_loaded) / 2 + 50;
        if (process_value >= 100)
            process_value = 99; // Prevent thread exit here.
        emit ProgressValueSignal(process_value);
    }

    m_u->m_capture_filename_index.store(++max);

    // Download supervior sign image if necessary
    std::string superior_id = report.GetSuperiorId();
    if (!superior_id.empty()) {
      std::string image_file = m_u->m_report_path.toStdString() + "/sign.png";
      std::string object_name = "sign/" + superior_id + "/sign.png";
      oss::OssService::GetInstance()->DownloadSignImage(image_file, object_name);
    }
    
    emit ProgressValueSignal(100);
}