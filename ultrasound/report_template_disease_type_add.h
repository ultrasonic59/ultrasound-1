#pragma once
#include <QDialog>
#include <ui_report_template_disease_type_add.h>

class ReportTemplateDiseaseTypeAddDialog : public QDialog
{
    Q_OBJECT
public:
    ReportTemplateDiseaseTypeAddDialog(QWidget *parent = Q_NULLPTR);
    ~ReportTemplateDiseaseTypeAddDialog();

    QString& GetDiseaseTypeName() { return m_disease_type_name; }
public slots:
    void onOk();
private:
    Ui::ReportTemplateDiseaseTypeAdd ui;

    QString m_disease_type_name;
};
