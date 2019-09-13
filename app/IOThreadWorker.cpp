#include "app/IOThreadWorker.h"

#include <QFileInfo>
#include <QDir>
#include <QDateTime>

#include "core_service/core_service.h"
#include "oss_service/oss_service.h"

void IOThreadWorker::Start(const StartParams& params) {
    m_start_params = params;

    m_stop_event = CreateEvent(NULL, TRUE, FALSE, NULL);

    m_worker_thread = CreateThread(NULL, 0, WorkerThread, this, 0, NULL);
}

void IOThreadWorker::Stop() {
    SetEvent(m_stop_event);

    WaitForSingleObject(m_worker_thread, INFINITE);
    CloseHandle(m_stop_event);
    CloseHandle(m_worker_thread);
}

void IOThreadWorker::DownloadFile(const IOThreadFileInfo& file_info) {
    PushFileInfo(file_info);
}

DWORD IOThreadWorker::WorkerThread(void* param) {
    IOThreadWorker* worker = static_cast<IOThreadWorker*>(param);

    worker->DeleteOldLogFiles();
    worker->DeleteOldReportFiles();
    worker->DownloadUserSignImage();

    while (true) {
        DWORD  ret = WaitForSingleObject(worker->m_stop_event, 50);
        if (ret == WAIT_OBJECT_0)
            return 0;

        IOThreadFileInfo file_info = worker->PopFileInfo();
        if (!file_info.file_path.isEmpty()) {
            worker->DownloadOssFile(file_info);
        }
    }
}

IOThreadFileInfo IOThreadWorker::PopFileInfo() {
    std::lock_guard<std::mutex> guard(m_file_list_mutex);
    if (m_file_list.size() > 0) {
        IOThreadFileInfo file_info = m_file_list.front();
        m_file_list.pop_front();
        return file_info;
    }

    return IOThreadFileInfo();
}

void IOThreadWorker::PushFileInfo(IOThreadFileInfo file_info) {
    std::lock_guard<std::mutex> guard(m_file_list_mutex);
    m_file_list.push_back(file_info);
}

void IOThreadWorker::DeleteOldLogFiles() {
    QDir report_dir(m_start_params.log_path);
    if (!report_dir.exists())
        return;

    QStringList filters;
    filters << "*.txt";
    report_dir.setNameFilters(filters);
    report_dir.setSorting(QDir::Time);

    QDateTime keep_time = QDateTime::currentDateTime().addMonths(0 - m_start_params.keep_log_files_month);

    QFileInfoList fileInfoList = report_dir.entryInfoList();
    for (const QFileInfo& fileInfo : fileInfoList) {
        QString file_base_name = fileInfo.baseName();
        QDateTime log_file_time = QDateTime::fromString(file_base_name, "yyyy-MM-dd_hh-mm-ss");
        if (log_file_time.isValid() && log_file_time < keep_time) {
            QFile::remove(fileInfo.filePath());
        }
    }
}

void IOThreadWorker::DeleteOldReportFiles() {
    QDir report_dir(m_start_params.user_path);
    if (!report_dir.exists())
        return;

    QDateTime keep_time = QDateTime::currentDateTime().addMonths(0 - m_start_params.keep_report_files_month);

    QFileInfoList fileInfoList = report_dir.entryInfoList(QDir::Dirs, QDir::Time);
    for (const QFileInfo& fileInfo : fileInfoList) {
        QString file_base_name = fileInfo.baseName();
        QDateTime report_file_time = QDateTime::fromString(file_base_name, "yyyyMMddhhmmsszzz");
        if (report_file_time.isValid() && report_file_time < keep_time) {
			QDir dir(fileInfo.filePath());
			dir.removeRecursively();
        }
    }
}

void IOThreadWorker::DownloadOssFile(const IOThreadFileInfo& file_info) {

    QString report_path = m_start_params.user_path + "/" + file_info.report_uid;
    QDir dir = QDir::root();
    dir.mkpath(report_path);
    
    bool success = true;
    QFileInfo fileInfo(file_info.file_path);
    if (fileInfo.exists() == false) {
        success = oss::OssService::GetInstance()->DownloadReportImage(
            file_info.file_path.toStdString(), file_info.object_name.toStdString());
    }

    if (m_start_params.handler) {
        if (success)
            m_start_params.handler->OnDownloadFileSuccessed(file_info);
        else
            m_start_params.handler->OnDownloadFileFailed(file_info);
    }
}

void IOThreadWorker::DownloadUserSignImage() {
  QString sign_image_file = m_start_params.user_path + "/sign.png";
  std::string object_name = "sign/" + core::CoreService::GetInstance()->GetCurrentUid() + "/sign.png";

  bool success = oss::OssService::GetInstance()->DownloadSignImage(sign_image_file.toStdString(),
                                                                object_name);
}