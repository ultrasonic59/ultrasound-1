#pragma once
#include <QDialog>
#include "ui_report_template_cloud_view.h"
#include "core_service/report_template.h"

class ReportTemplateCloudViewDialog : public QDialog
{
    Q_OBJECT
public:
    ReportTemplateCloudViewDialog(core::ReportTemplate& report_template, QWidget *parent = Q_NULLPTR);
    ~ReportTemplateCloudViewDialog();

    QString& GetDescription() { return m_description; }
    QString& GetConclusion() { return m_conclusion; }
    bool IsSyncTemplate() { return m_b_sync_template; }
 public slots:
    void onOk();
private:
    Ui::ReportTemplateCloudView ui;

    QString m_description;
    QString m_conclusion;
    bool m_b_sync_template;
};
