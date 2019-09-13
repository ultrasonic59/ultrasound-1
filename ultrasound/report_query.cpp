#include "report_query.h"

#include <QDate>
#include <QTableWidgetItem>
#include <QUrl>

#include "core_service/core_service.h"

ReportQuery::ReportQuery(QWidget *parent)
    : QDialog(parent)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    ui.setupUi(this);

    QStringList headers = { QString::fromLocal8Bit("超声编号"), QString::fromLocal8Bit("审核状态"),
        QString::fromLocal8Bit("患者姓名"), QString::fromLocal8Bit("性别"), QString::fromLocal8Bit("年龄"),
        QString::fromLocal8Bit("门诊号"), QString::fromLocal8Bit("检查日期"),
        QString::fromLocal8Bit("检查部位"), QString::fromLocal8Bit("送检医生"), 
        QString::fromLocal8Bit("会诊医生") };

    ui.table_widget->setColumnCount(headers.count());
    ui.table_widget->setHorizontalHeaderLabels(headers);
    ui.table_widget->setRowCount(0);

    ui.table_widget->setColumnWidth(0, 1.5 * ui.table_widget->columnWidth(0));
    ui.table_widget->setColumnWidth(3, 0.25 * ui.table_widget->columnWidth(0));
    ui.table_widget->setColumnWidth(4, 0.25 * ui.table_widget->columnWidth(0));
    ui.table_widget->setColumnWidth(5, 0.6 * ui.table_widget->columnWidth(0));
    ui.table_widget->setColumnWidth(7, 0.8 * ui.table_widget->columnWidth(0));

    ui.start_date_edit->setDate(QDate::currentDate().addMonths(-1));
    ui.end_date_edit->setDate(QDate::currentDate().addDays(1));

	ui.report_status_combo_box->addItem(QString::fromLocal8Bit("全部"));
	ui.report_status_combo_box->addItem(QString::fromLocal8Bit("无需审核"));
	ui.report_status_combo_box->addItem(QString::fromLocal8Bit("未审核"));
	ui.report_status_combo_box->addItem(QString::fromLocal8Bit("审核未通过"));
	ui.report_status_combo_box->addItem(QString::fromLocal8Bit("审核通过"));

    ui.result_label->setVisible(false);
    ui.pre_button->setEnabled(false);
    ui.next_button->setEnabled(false);
    ui.open_button->setEnabled(false);

    connect(ui.table_widget, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(OnItemClicked(QTableWidgetItem*)));

    connect(ui.pre_button, SIGNAL(clicked()), this, SLOT(PrePage()));
    connect(ui.next_button, SIGNAL(clicked()), this, SLOT(NextPage()));
    connect(ui.query_button, SIGNAL(clicked()), this, SLOT(Query()));
    connect(ui.open_button, SIGNAL(clicked()), this, SLOT(accept()));

    connect(this, SIGNAL(QueryReportSuccessedSignal()),
        this, SLOT(QueryReportSuccessedSlot()));
    connect(this, SIGNAL(QueryReportFailedSignal()),
        this, SLOT(QueryReportFailedSlot()));
}

ReportQuery::~ReportQuery()
{
}

bool ReportQuery::GetSelectedReport(core::Report& report) {
    std::string uid = m_current_report_uid.toStdString();
    std::list<core::Report>::iterator it =
        std::find_if(m_reports.begin(), m_reports.end(), [uid](const core::Report& report) {
        return uid == report.GetUid();
    });

    if (it != m_reports.end()) {
        report = *it;
        return true;
    }

    return false;
}

void ReportQuery::OnCreateReportSuccessed() {}
void ReportQuery::OnCreateReportFailed(int error_code, const std::string& message) {}
void ReportQuery::OnUpdateReportSuccessed() {}
void ReportQuery::OnUpdateReportFailed(int error_code, const std::string& message) {}
void ReportQuery::OnConfirmReportSuccessed(const core::ConfirmReportData& data) {}
void ReportQuery::OnConfirmReportFailed(int error_code, const std::string& message) {}

void ReportQuery::OnQueryReportSuccessed(std::list<core::Report> reports) {
    m_current_report_uid.clear();
    m_reports.clear();
    m_reports = reports;

    emit QueryReportSuccessedSignal();
}

void ReportQuery::OnQueryReportFailed(int error_code, const std::string& message) {
    m_current_report_uid.clear();
    m_reports.clear();

    emit QueryReportFailedSignal();
}

void ReportQuery::Query() {
    m_skip = 0;

    QueryImpl();
}

void ReportQuery::QueryImpl() {
    ui.query_button->setEnabled(false);
    ui.pre_button->setEnabled(false);
    ui.next_button->setEnabled(false);
    ui.open_button->setEnabled(false);
    ui.table_widget->setRowCount(0);
    ui.result_label->setVisible(false);

    std::map<std::string, std::string> conditions;
    conditions["skip"] = std::to_string(m_skip);
    conditions["limit"] = std::to_string(m_limit);
    conditions["primary_id"] = core::CoreService::GetInstance()->GetCurrentUid();
    
    if (!ui.uid_line_edit->text().isEmpty()) {
        conditions["uid"] = QUrl::toPercentEncoding(ui.uid_line_edit->text()).constData();
    }
    else {
        QDateTime start = ui.start_date_edit->dateTime();
        start = start.addSecs(8 * 60 * 60); // UTC + 8
        conditions["begin"] = std::to_string(start.toMSecsSinceEpoch() / 1000);

        QDateTime end = ui.end_date_edit->dateTime();
        end = end.addSecs(8 * 60 * 60);
        conditions["end"] = std::to_string(end.toMSecsSinceEpoch() / 1000);

        if (!ui.name_line_edit->text().isEmpty())
            conditions["name"] = QUrl::toPercentEncoding(ui.name_line_edit->text()).constData();

        if (!ui.dialog_id_line_edit->text().isEmpty())
            conditions["dialog_id"] = QUrl::toPercentEncoding(ui.dialog_id_line_edit->text()).constData();

        if (!ui.dia_doctor_line_edit->text().isEmpty())
            conditions["dia_doctor"] = QUrl::toPercentEncoding(ui.dia_doctor_line_edit->text()).constData();

        if (!ui.superior_line_edit->text().isEmpty())
            conditions["superior"] = QUrl::toPercentEncoding(ui.superior_line_edit->text()).constData();

		if(ui.report_status_combo_box->currentIndex() > 0)
			conditions["confirm_status"] = QUrl::toPercentEncoding(
				QString::number(ui.report_status_combo_box->currentIndex() - 1)).constData();
    }

    core::CoreService::GetInstance()->QueryReport(conditions);
}

void ReportQuery::PrePage() {
    if (m_skip > m_limit) 
        m_skip -= m_limit;
    else
        m_skip = 0;

    QueryImpl();
}

void ReportQuery::NextPage() {
    m_skip += m_limit;

    QueryImpl();
}

void ReportQuery::QueryReportSuccessedSlot() {
    ui.query_button->setEnabled(true);
    ui.open_button->setEnabled(false);

    if (m_skip < m_limit) 
        ui.pre_button->setEnabled(false);
    else
        ui.pre_button->setEnabled(true);

    if (m_reports.size() <= 0) {
        ui.next_button->setEnabled(false);

        ui.table_widget->setRowCount(0);
        ui.result_label->setVisible(true);
        if(m_skip > 0)
            ui.result_label->setText(QString::fromLocal8Bit("没有更多符合条件的报告了！"));
        else
            ui.result_label->setText(QString::fromLocal8Bit("没有查询到符合条件的报告！"));
        return;
    }
    else {
        if(m_reports.size() % m_limit != 0) 
            ui.next_button->setEnabled(false);
        else
            ui.next_button->setEnabled(true);
    }

    ui.table_widget->setRowCount(m_reports.size());

    int row = 0;
    for (const core::Report& report : m_reports) {
        QTableWidgetItem* uid_item = new QTableWidgetItem();
        uid_item->setText(QString::fromStdString(report.GetUid()));
        ui.table_widget->setItem(row, 0, uid_item);

		QTableWidgetItem* status_item = new QTableWidgetItem();
		QString status = QString::fromLocal8Bit("无需审核");
		switch (report.GetConfirmStatus())
		{
		case core::CONFIRM_STATUS_WAIT :
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
		ui.table_widget->setItem(row, 1, status_item);

        QTableWidgetItem* name_item = new QTableWidgetItem();
        name_item->setText(QString::fromStdString(report.GetName()));
        ui.table_widget->setItem(row, 2, name_item);

        QTableWidgetItem* sex_item = new QTableWidgetItem();
        sex_item->setText(QString::fromStdString(report.GetSex()));
        ui.table_widget->setItem(row, 3, sex_item);

        QTableWidgetItem* age_item = new QTableWidgetItem();
        age_item->setText(QString::number(report.GetAge()));
        ui.table_widget->setItem(row, 4, age_item);

        QTableWidgetItem* dialog_id_item = new QTableWidgetItem();
        dialog_id_item->setText(QString::fromStdString(report.GetDialogId()));
        ui.table_widget->setItem(row, 5, dialog_id_item);

        QTableWidgetItem* check_date_item = new QTableWidgetItem();
        check_date_item->setText(QString::fromStdString(report.GetCheckDate()));
        ui.table_widget->setItem(row, 6, check_date_item);

        QTableWidgetItem* parts_item = new QTableWidgetItem();
        parts_item->setText(QString::fromStdString(report.GetParts()));
        ui.table_widget->setItem(row, 7, parts_item);

        QTableWidgetItem* dia_doctor_item = new QTableWidgetItem();
        dia_doctor_item->setText(QString::fromStdString(report.GetDiaDoctor()));
        ui.table_widget->setItem(row, 8, dia_doctor_item);

        QTableWidgetItem* superior_item = new QTableWidgetItem();
        superior_item->setText(QString::fromStdString(report.GetSuperior()));
        ui.table_widget->setItem(row, 9, superior_item);

        row++;
    }
}

void ReportQuery::QueryReportFailedSlot() {
    m_skip = 0;

    ui.query_button->setEnabled(true);
    ui.pre_button->setEnabled(false);
    ui.next_button->setEnabled(false);
    ui.open_button->setEnabled(false);

    ui.table_widget->setRowCount(0);
    ui.result_label->setVisible(true);
    ui.result_label->setText(QString::fromLocal8Bit("查询报告出错！"));
}

void ReportQuery::OnItemClicked(QTableWidgetItem* item) {
    int row = item->row();
    QTableWidgetItem* id_item = ui.table_widget->item(row, 0);
    if (id_item && !id_item->text().isEmpty()) {
        m_current_report_uid = id_item->text();
        ui.open_button->setEnabled(true);
    }
    else {
        ui.open_button->setEnabled(false);
    }
}