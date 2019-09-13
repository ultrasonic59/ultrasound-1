#include "report_template_disease_type_add.h"

ReportTemplateDiseaseTypeAddDialog::ReportTemplateDiseaseTypeAddDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    ui.setupUi(this);

    connect(ui.ok_button, SIGNAL(clicked()), this, SLOT(onOk()));
    connect(ui.cancel_button, SIGNAL(clicked()), this, SLOT(reject()));
}

ReportTemplateDiseaseTypeAddDialog::~ReportTemplateDiseaseTypeAddDialog()
{
    
}

void ReportTemplateDiseaseTypeAddDialog::onOk()
{
    m_disease_type_name = ui.disease_type_name_edit->text();
    this->accept();
}
