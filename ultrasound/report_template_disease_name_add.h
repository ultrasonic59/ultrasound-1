#pragma once
#include <QDialog>
#include <ui_report_template_disease_name_add.h>

class ReportTemplateDiseaseNameAddDialog : public QDialog
{
    Q_OBJECT
public:
    ReportTemplateDiseaseNameAddDialog(QWidget *parent = Q_NULLPTR);
    ~ReportTemplateDiseaseNameAddDialog();

    QString& GetName() { return m_name; }
    QString& GetDescription() { return m_description; }
    QString& GetConclusion() { return m_conclusion; }
public slots:
    void onOk();
private:
    Ui::ReportTemplateDiseaseNameAdd ui;

    QString m_name;
    QString m_description;
    QString m_conclusion;
};