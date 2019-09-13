#pragma once

#include <QDialog>
#include "ui_report_query.h"

#include "core_service/report_event_handler.h"

class ReportQuery : public QDialog, public core::ReportEventHandler
{
    Q_OBJECT

public:
    ReportQuery(QWidget *parent = Q_NULLPTR);
    ~ReportQuery();

    bool GetSelectedReport(core::Report& report);

    // core::ReportEventHandler
    void OnCreateReportSuccessed() override;
    void OnCreateReportFailed(int error_code, const std::string& message) override;
    void OnUpdateReportSuccessed() override;
    void OnUpdateReportFailed(int error_code, const std::string& message) override;
	void OnConfirmReportSuccessed(const core::ConfirmReportData& data) override;
	void OnConfirmReportFailed(int error_code, const std::string& message) override;
    void OnQueryReportSuccessed(std::list<core::Report> reports) override;
    void OnQueryReportFailed(int error_code, const std::string& message) override;

signals:
    void QueryReportSuccessedSignal();
    void QueryReportFailedSignal();

private slots:
    void OnItemClicked(QTableWidgetItem *item);

    void Query();
    void PrePage();
    void NextPage();

    void QueryReportSuccessedSlot();
    void QueryReportFailedSlot();

private:
    Ui::ReportQuery ui;

    void QueryImpl();

    int m_skip = 0;
    int m_limit = 20;

    std::list<core::Report> m_reports;
    QString m_current_report_uid;
};
