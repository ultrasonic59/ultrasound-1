#pragma once
#include <QObject>
#include "report_template_base.h"
#include "report_template_cloud_model.h"

class ReportTemplate;
class CloudReportTemplate : public ReportTemplateBase
{
    Q_OBJECT
public:
    explicit CloudReportTemplate(ReportTemplate* report_template);
    virtual void Init();

    QString& GetName() { return m_name; }
    QString& GetType() { return m_type; }
    QString& GetDescription() { return m_description; }
    QString& GetConclusion() { return m_conclusion; }
public slots:
    virtual void OnDiseaseTemplateTableRowDoubleClicked(const QModelIndex index);
    virtual void OnPrev();
    virtual void OnNext();
    virtual void OnQuery();

    void OnCreateTemplate(core::ReportTemplate report_template);
    void OnModifyReportTemplate(core::ReportTemplate report_template);
signals:
    void ApplyReportTemplate(core::ReportTemplate);
    void ModifyReportTemplate(core::ReportTemplate);
protected:
private:
    ReportTemplate* m_temlate;

    CloudDiseaseTableModel * m_disease_template_model;

    QString m_name;
    QString m_type;
    QString m_description;
    QString m_conclusion;
};
