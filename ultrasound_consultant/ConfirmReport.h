#pragma once

#include "core_service/report.h"

#include <QDialog>
#include "ui_confirm_report.h"

class ConfirmReport : public QDialog
{
	Q_OBJECT

public:
	ConfirmReport(core::Report& report, QWidget *parent = Q_NULLPTR);
	~ConfirmReport();

private slots:
	void Pass();
	void Reject();
	void CheckMaxLength();

private:
	Ui::ConfirmReport ui;

	core::Report& m_report;
};
