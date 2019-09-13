#pragma once

#include <QString>
#include <windows.h>

#include <list>
#include <mutex>
#include <string>

struct IOThreadFileInfo {
    QString report_uid;
    QString file_path;
    QString object_name;
};

class IOThreadWorkerEventHandler {
public:
    virtual ~IOThreadWorkerEventHandler() {}

    virtual void OnDownloadFileSuccessed(const IOThreadFileInfo& file_info) = 0;
    virtual void OnDownloadFileFailed(const IOThreadFileInfo& file_info) = 0;
};

class IOThreadWorker {
public:
    struct StartParams {
        IOThreadWorkerEventHandler* handler;
        QString user_path;
        QString log_path;
        int keep_log_files_month = 1;
        int keep_report_files_month = 6;
    };

    void Start(const StartParams& params);
    void Stop();
    void DownloadFile(const IOThreadFileInfo& file_info);

private:
    static DWORD WINAPI WorkerThread(void* param);

    IOThreadFileInfo PopFileInfo();
    void PushFileInfo(IOThreadFileInfo file_info);

    void DeleteOldLogFiles();
    void DeleteOldReportFiles();
    void DownloadOssFile(const IOThreadFileInfo& file_info);
    void DownloadUserSignImage();

    StartParams m_start_params;
    HANDLE m_worker_thread;
    HANDLE m_stop_event;

    std::mutex m_file_list_mutex;
    std::list<IOThreadFileInfo> m_file_list;
};