#include "report_template_disease_name_add.h"

ReportTemplateDiseaseNameAddDialog::ReportTemplateDiseaseNameAddDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    ui.setupUi(this);

    connect(ui.ok_button, SIGNAL(clicked()), this, SLOT(onOk()));
    connect(ui.cancel_button, SIGNAL(clicked()), this, SLOT(reject()));
}

ReportTemplateDiseaseNameAddDialog::~ReportTemplateDiseaseNameAddDialog()
{
}

void ReportTemplateDiseaseNameAddDialog::onOk()
{
    m_name = ui.name_edit->text();
    m_description = ui.description_edit->toPlainText();
    m_conclusion = ui.conclusion_edit->toPlainText();
    this->accept();
}
