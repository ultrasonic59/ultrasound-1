#pragma once

#include <QObject>
#include <windows.h>

#include "core_service/report_event_handler.h"

class ultrasound;

class ReportThreadWorker : public QObject, public core::ReportEventHandler {
    Q_OBJECT

public:
    ReportThreadWorker(ultrasound* u, bool auto_save);
    ~ReportThreadWorker();
    bool HasError() { return m_has_error; }
    int GetErrorCode() { return m_error_code; }
    QString GetErrorMessage() { return m_message; }

    // core::ReportEventHandler
    void OnCreateReportSuccessed() override;
    void OnCreateReportFailed(int error_code, const std::string& message) override;
    void OnUpdateReportSuccessed() override;
    void OnUpdateReportFailed(int error_code, const std::string& message) override;
	void OnConfirmReportSuccessed(const core::ConfirmReportData& data) override;
	void OnConfirmReportFailed(int error_code, const std::string& message) override;
    void OnQueryReportSuccessed(std::list<core::Report> reports) override;
    void OnQueryReportFailed(int error_code, const std::string& message) override;

public slots:
    void Upload();
    void Download();

signals:
    void ProgressValueSignal(int);

private:
    ultrasound* m_u;
	bool m_auto_save = false;
    bool m_has_error = false;
    int m_error_code = 0;
    QString m_message;
    HANDLE m_report_save_event;
};
