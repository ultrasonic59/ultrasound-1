#include "ConfirmReport.h"

#include <QJsonDocument>
#include <QJsonObject>

#include "app/ultrasound_system_message_id.h"
#include "core_service/core_service.h"
#include "nim_service/nim_service.h"

ConfirmReport::ConfirmReport(core::Report& report, QWidget *parent)
	: m_report(report), QDialog(parent)
{
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	ui.setupUi(this);

	connect(ui.confirm_msg_text_edit, SIGNAL(textChanged()), this, SLOT(CheckMaxLength()));

	connect(ui.pass_button, SIGNAL(clicked()), this, SLOT(Pass()));
	connect(ui.reject_button, SIGNAL(clicked()), this, SLOT(Reject()));
	connect(ui.cancel_button, SIGNAL(clicked()), this, SLOT(close()));
}

ConfirmReport::~ConfirmReport()
{
}

void ConfirmReport::Pass() {
	QString msg = ui.confirm_msg_text_edit->toPlainText();
	if (msg.isEmpty()) {
		msg = QString::fromLocal8Bit("审核通过。");
	}
	m_report.SetConfirmMsg(msg.toStdString());
	m_report.SetConfirmStatus(core::CONFIRM_STATUS_PASS);

	core::CoreService::GetInstance()->ConfirmReport(m_report);

	accept();
}

void ConfirmReport::Reject() {
	QString msg = ui.confirm_msg_text_edit->toPlainText();
	if (msg.isEmpty()) {
		msg = QString::fromLocal8Bit("审核未通过。");
	}
	m_report.SetConfirmMsg(msg.toStdString());
	m_report.SetConfirmStatus(core::CONFIRM_STATUS_REJECT);

	core::CoreService::GetInstance()->ConfirmReport(m_report);

	reject();
}

void ConfirmReport::CheckMaxLength() {
	QString text = ui.confirm_msg_text_edit->toPlainText();
	if (text.length() >= 100) {
		ui.confirm_msg_text_edit->setText(text);
	}
}