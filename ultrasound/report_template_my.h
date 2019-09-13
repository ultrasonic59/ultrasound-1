#pragma once
#include <QObject>
#include <QSqlTableModel>
#include "report_template_base.h"
#include "core_service/report_template.h"

class ReportTemplate;
class MyReportTemplate : public ReportTemplateBase
{
    Q_OBJECT
public:
    explicit MyReportTemplate(ReportTemplate* temlate);

    virtual void Init();

    QString& GetDescription() { return m_description; }
    QString& GetConclusion() { return m_conclusion; }
    
public slots:
    void OnDiseaseTypeTableCurrentRowChanged(
        const QModelIndex &current, const QModelIndex &previous);

    void OnDiseaseNameTableCurrentRowChanged(
        const QModelIndex &current, const QModelIndex &previous);

    void OnDiseaseTypeAddItem();
    void OnDiseaseTypeRemoveItem();
    void OnDiseaseNameAddItem();
    void OnDiseaseNameRemoveItem();

    void OnApplyReportTemplate(core::ReportTemplate report_template);
signals:
    void CreateTemplate(core::ReportTemplate);
private:
    QString m_description;
    QString m_conclusion;

    QSqlTableModel * m_disease_type_model;
    QSqlTableModel * m_disease_name_model;

    ReportTemplate* m_temlate;

    int m_current_disease_type_edit_row;
    int m_current_disease_name_edit_row;
    QString m_current_disease_type_id;
};
