#include "report_template_cloud_view.h"
#include "core_service/core_service.h"

ReportTemplateCloudViewDialog::ReportTemplateCloudViewDialog(core::ReportTemplate& report_template, QWidget* parent)
    : QDialog(parent)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    ui.setupUi(this);

    connect(ui.ok_button, SIGNAL(clicked()), this, SLOT(onOk()));
    connect(ui.cancel_button, SIGNAL(clicked()), this, SLOT(reject()));

    ui.is_upload_to_cloud->setCheckState(Qt::CheckState::Unchecked);
    core::PrimaryUser primary_user = core::CoreService::GetInstance()->GetCurrentPrimaryUser();
    std::string doctor_id = report_template.Doctor_id();
    if (doctor_id == primary_user.GetUid())
    {
        ui.description_edit->setEnabled(true);
        ui.conclusion_edit->setEnabled(true);
        ui.is_upload_to_cloud->setEnabled(true);
    }
    else
    {
        ui.description_edit->setEnabled(false);
        ui.conclusion_edit->setEnabled(false);
        ui.is_upload_to_cloud->setEnabled(false);
    }

    ui.hospital_edit->setText(QString::fromStdString(report_template.Hospital()));
    ui.doctor_edit->setText(QString::fromStdString(report_template.Doctor()));
    ui.createtime_edit->setText(QString::fromStdString(report_template.Create_time()));
    ui.description_edit->setText(QString::fromStdString(report_template.Disease_description()));
    ui.conclusion_edit->setText(QString::fromStdString(report_template.Disease_conclusion()));
}

ReportTemplateCloudViewDialog::~ReportTemplateCloudViewDialog()
{
}

void ReportTemplateCloudViewDialog::onOk()
{
    m_description = ui.description_edit->toPlainText();
    m_conclusion = ui.conclusion_edit->toPlainText();
    if(Qt::CheckState::Checked == ui.is_upload_to_cloud->checkState())
    {
        m_b_sync_template = true;
    }
    else
    {
        m_b_sync_template = false;
    }
    this->accept();
}