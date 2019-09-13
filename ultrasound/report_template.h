#pragma once

#include <QDialog>
#include "ui_report_template.h"
#include "report_template_cloud.h"
#include "report_template_my.h"
#include <memory>

class QSqlTableModel;

class ReportTemplate : public QDialog
{
    Q_OBJECT

public:
    friend class MyReportTemplate;
    friend class CloudReportTemplate;

    ReportTemplate(QWidget *parent = Q_NULLPTR);
    ~ReportTemplate();

    QString& GetDescription();
    QString& GetConclusion();
public slots:
    void OnRightStackedWidgetCurrentChanged(int index);
private:
    void Clear() {
        //m_description.clear();
        //m_conclusion.clear();

        ui.description_edit->clear();
        ui.conclusion_edit->clear();
    }

    Ui::ReportTemplate ui;

    MyReportTemplate m_my_report_template;
    CloudReportTemplate m_cloud_report_template;
};
