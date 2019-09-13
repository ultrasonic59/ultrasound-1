#include "report_template.h"
#include "report_template_cloud.h"
#include <QSqlRecord>
#include <QLineEdit>
#include <QMessageBox>
#include <QSqlQuery>
#include "report_template_disease_type_add.h"
#include "report_template_disease_name_add.h"
#include "report_template_cloud_view.h"
#include "core_service/core_service.h"

CloudReportTemplate::CloudReportTemplate(ReportTemplate* report_template)
    : m_temlate(report_template)
{

}

void CloudReportTemplate::Init()
{
    m_disease_template_model = new CloudDiseaseTableModel;
    int index_idx = m_disease_template_model->fieldIndex("index");
    m_disease_template_model->setHeaderData(index_idx, Qt::Horizontal, QString::fromLocal8Bit("序号"),
        Qt::DisplayRole);
    int disease_type_idx = m_disease_template_model->fieldIndex("disease_type");
    m_disease_template_model->setHeaderData(disease_type_idx, Qt::Horizontal, QString::fromLocal8Bit("疾病类别"),
        Qt::DisplayRole);
    int disease_name_idx = m_disease_template_model->fieldIndex("disease_name");
    m_disease_template_model->setHeaderData(disease_name_idx, Qt::Horizontal, QString::fromLocal8Bit("疾病名称"),
        Qt::DisplayRole);
    int disease_description_idx = m_disease_template_model->fieldIndex("disease_description");
    m_disease_template_model->setHeaderData(disease_description_idx, Qt::Horizontal, QString::fromLocal8Bit("疾病描述"),
        Qt::DisplayRole);
    int disease_conclusion_idx = m_disease_template_model->fieldIndex("disease_conclusion");
    m_disease_template_model->setHeaderData(disease_conclusion_idx, Qt::Horizontal, QString::fromLocal8Bit("疾病结论"),
        Qt::DisplayRole);

    int doctor_idx = m_disease_template_model->fieldIndex("doctor");
    m_disease_template_model->setHeaderData(doctor_idx, Qt::Horizontal, QString::fromLocal8Bit("医生"),
        Qt::DisplayRole);
    m_disease_template_model->select();

    m_temlate->ui.cloud_template_table_view->setModel(m_disease_template_model);
    m_temlate->ui.cloud_template_table_view->setColumnWidth(index_idx, 40);
    m_temlate->ui.cloud_template_table_view->setColumnWidth(disease_type_idx, 80);
    m_temlate->ui.cloud_template_table_view->setColumnWidth(disease_name_idx, 80);
    m_temlate->ui.cloud_template_table_view->setColumnWidth(disease_description_idx, 150);
    m_temlate->ui.cloud_template_table_view->setColumnWidth(disease_conclusion_idx, 210);
    m_temlate->ui.cloud_template_table_view->setColumnWidth(doctor_idx, 80);
    m_temlate->ui.cloud_template_table_view->setStyleSheet(
        "QHeaderView::section { border: 1px solid black}");
    m_temlate->ui.cloud_template_table_view->horizontalHeader()->setFixedHeight(30);

    int id_idx = m_disease_template_model->fieldIndex("id");
    m_temlate->ui.cloud_template_table_view->hideColumn(id_idx);

    int hospital_idx = m_disease_template_model->fieldIndex("hospital");
    m_temlate->ui.cloud_template_table_view->hideColumn(hospital_idx);

    int create_time_idx = m_disease_template_model->fieldIndex("create_time");
    m_temlate->ui.cloud_template_table_view->hideColumn(create_time_idx);

    int hospital_id_idx = m_disease_template_model->fieldIndex("hospital_id");
    m_temlate->ui.cloud_template_table_view->hideColumn(hospital_id_idx);

    int doctor_id_idx = m_disease_template_model->fieldIndex("doctor_id");
    m_temlate->ui.cloud_template_table_view->hideColumn(doctor_id_idx);

    connect(m_temlate->ui.cloud_template_table_view, SIGNAL(doubleClicked(const QModelIndex &)), 
        this, SLOT(OnDiseaseTemplateTableRowDoubleClicked(const QModelIndex &)));

    connect(m_temlate->ui.cloud_prev_button, SIGNAL(clicked()), this, SLOT(OnPrev()));
    connect(m_temlate->ui.cloud_next_button, SIGNAL(clicked()), this, SLOT(OnNext()));
    connect(m_temlate->ui.cloud_query_button, SIGNAL(clicked()), this, SLOT(OnQuery()));
    connect(this, SIGNAL(ModifyReportTemplate(core::ReportTemplate)), this, SLOT(OnModifyReportTemplate(core::ReportTemplate)));
}

void CloudReportTemplate::OnDiseaseTemplateTableRowDoubleClicked(const QModelIndex index)
{
    int row = index.row();
    QString id = index.sibling(row, 1).data().toString();

    core::ReportTemplate report_template;
    int disease_id_idx = m_disease_template_model->fieldIndex("id");
    QModelIndex disease_id_index = m_disease_template_model->index(row, disease_id_idx);
    QString disease_id = m_disease_template_model->data(disease_id_index).toString();
    report_template.Id(disease_id.toStdString());

    int disease_type_idx = m_disease_template_model->fieldIndex("disease_type");
    QModelIndex disease_type_index = m_disease_template_model->index(row, disease_type_idx);
    QString disease_type = m_disease_template_model->data(disease_type_index).toString();
    report_template.Disease_type(disease_type.toStdString());

    int disease_name_idx = m_disease_template_model->fieldIndex("disease_name");
    QModelIndex disease_name_index = m_disease_template_model->index(row, disease_name_idx);
    QString disease_name = m_disease_template_model->data(disease_name_index).toString();
    report_template.Disease_name(disease_name.toStdString());

    int disease_description_idx = m_disease_template_model->fieldIndex("disease_description");
    QModelIndex disease_description_index = m_disease_template_model->index(row, disease_description_idx);
    QString disease_description = m_disease_template_model->data(disease_description_index).toString();
    report_template.Disease_description(disease_description.toStdString());

    int disease_conclusion_idx = m_disease_template_model->fieldIndex("disease_conclusion");
    QModelIndex disease_conclusion_index = m_disease_template_model->index(row, disease_conclusion_idx);
    QString disease_conclusion = m_disease_template_model->data(disease_conclusion_index).toString();
    report_template.Disease_conclusion(disease_conclusion.toStdString());

    int hospital_idx = m_disease_template_model->fieldIndex("hospital");
    QModelIndex hospital_index = m_disease_template_model->index(row, hospital_idx);
    QString hospital = m_disease_template_model->data(hospital_index).toString();
    report_template.Hospital(hospital.toStdString());

    int doctor_idx = m_disease_template_model->fieldIndex("doctor");
    QModelIndex doctor_index = m_disease_template_model->index(row, doctor_idx);
    QString doctor = m_disease_template_model->data(doctor_index).toString();
    report_template.Doctor(doctor.toStdString());

    int create_time_idx = m_disease_template_model->fieldIndex("create_time");
    QModelIndex create_time_index = m_disease_template_model->index(row, create_time_idx);
    QString create_time = m_disease_template_model->data(create_time_index).toString();
    report_template.Create_time(create_time.toStdString());

    int doctor_id_idx = m_disease_template_model->fieldIndex("doctor_id");
    QModelIndex doctor_id_index = m_disease_template_model->index(row, doctor_id_idx);
    QString doctor_id = m_disease_template_model->data(doctor_id_index).toString();
    report_template.Doctor_id(doctor_id.toStdString());

    ReportTemplateCloudViewDialog dialog(report_template);
    if (dialog.exec() == QDialog::Accepted) {
        //应用，添加到本地库
        report_template.Disease_description(dialog.GetDescription().toStdString());
        report_template.Disease_conclusion(dialog.GetConclusion().toStdString());
        if (dialog.IsSyncTemplate())
        {
            emit ModifyReportTemplate(report_template);
        }
        emit ApplyReportTemplate(report_template);
    }
}

void CloudReportTemplate::OnPrev()
{
    m_disease_template_model->prev();
}

void CloudReportTemplate::OnNext()
{
    m_disease_template_model->next();
}

void CloudReportTemplate::OnQuery()
{
    QString doctor = m_temlate->ui.cloud_doctor_edit->text();
    QString disease_description = m_temlate->ui.cloud_description_edit->text();
    std::map<std::string, std::string> query_conditions;
    if(!doctor.isEmpty())
    {
        query_conditions.insert({ "doctor", QUrl::toPercentEncoding(doctor).constData() });
    }
    if (!disease_description.isEmpty())
    {
        query_conditions.insert({ "disease_description", QUrl::toPercentEncoding(disease_description).constData() });
    }
    m_disease_template_model->select(query_conditions);
}

void CloudReportTemplate::OnCreateTemplate(core::ReportTemplate report_template)
{
    int row = m_disease_template_model->rowCount();
    if(!m_disease_template_model->insertRow(row))
    {
        return;
    }

    int id_idx = m_disease_template_model->fieldIndex("id");
    QModelIndex idx_index = m_disease_template_model->index(row, id_idx);
    m_disease_template_model->setData(idx_index, "");

    int disease_type_idx = m_disease_template_model->fieldIndex("disease_type");
    QModelIndex disease_type_index = m_disease_template_model->index(row, disease_type_idx);
    m_disease_template_model->setData(disease_type_index, QString::fromStdString(report_template.Disease_type()));

    int disease_name_idx = m_disease_template_model->fieldIndex("disease_name");
    QModelIndex disease_name_index = m_disease_template_model->index(row, disease_name_idx);
    m_disease_template_model->setData(disease_name_index, QString::fromStdString(report_template.Disease_name()));

    int disease_description_idx = m_disease_template_model->fieldIndex("disease_description");
    QModelIndex disease_description_index = m_disease_template_model->index(row, disease_description_idx);
    m_disease_template_model->setData(disease_description_index, QString::fromStdString(report_template.Disease_description()));

    int disease_conclusion_idx = m_disease_template_model->fieldIndex("disease_conclusion");
    QModelIndex disease_conclusion_index = m_disease_template_model->index(row, disease_conclusion_idx);
    m_disease_template_model->setData(disease_conclusion_index, QString::fromStdString(report_template.Disease_conclusion()));

    core::PrimaryUser primary_user = core::CoreService::GetInstance()->GetCurrentPrimaryUser();

    int hospital_idx = m_disease_template_model->fieldIndex("hospital");
    QModelIndex hospital_index = m_disease_template_model->index(row, hospital_idx);
    m_disease_template_model->setData(hospital_index, QString::fromStdString(primary_user.GetHospital()));

    int doctor_idx = m_disease_template_model->fieldIndex("doctor");
    QModelIndex doctor_index = m_disease_template_model->index(row, doctor_idx);
    m_disease_template_model->setData(doctor_index, QString::fromStdString(primary_user.GetName()));

    int hospital_id_idx = m_disease_template_model->fieldIndex("hospital_id");
    QModelIndex dhospital_id_index = m_disease_template_model->index(row, hospital_id_idx);
    m_disease_template_model->setData(dhospital_id_index, QString::number(primary_user.GetHospitalId()));

    int doctor_id_idx = m_disease_template_model->fieldIndex("doctor_id");
    QModelIndex doctor_id_index = m_disease_template_model->index(row, doctor_id_idx);
    m_disease_template_model->setData(doctor_id_index, QString::fromStdString(primary_user.GetUid()));

    m_disease_template_model->submitAll();
    m_disease_template_model->select();
}

void CloudReportTemplate::OnModifyReportTemplate(core::ReportTemplate report_template)
{
    QModelIndex index = m_temlate->ui.cloud_template_table_view->selectionModel()->currentIndex();
    int row = index.row();

    int id_idx = m_disease_template_model->fieldIndex("id");
    QModelIndex idx_index = m_disease_template_model->index(row, id_idx);
    m_disease_template_model->setData(idx_index, QString::fromStdString(report_template.Id()));

    int disease_type_idx = m_disease_template_model->fieldIndex("disease_type");
    QModelIndex disease_type_index = m_disease_template_model->index(row, disease_type_idx);
    m_disease_template_model->setData(disease_type_index, QString::fromStdString(report_template.Disease_type()));

    int disease_name_idx = m_disease_template_model->fieldIndex("disease_name");
    QModelIndex disease_name_index = m_disease_template_model->index(row, disease_name_idx);
    m_disease_template_model->setData(disease_name_index, QString::fromStdString(report_template.Disease_name()));

    int disease_description_idx = m_disease_template_model->fieldIndex("disease_description");
    QModelIndex disease_description_index = m_disease_template_model->index(row, disease_description_idx);
    m_disease_template_model->setData(disease_description_index, QString::fromStdString(report_template.Disease_description()));

    int disease_conclusion_idx = m_disease_template_model->fieldIndex("disease_conclusion");
    QModelIndex disease_conclusion_index = m_disease_template_model->index(row, disease_conclusion_idx);
    m_disease_template_model->setData(disease_conclusion_index, QString::fromStdString(report_template.Disease_conclusion()));

    core::PrimaryUser primary_user = core::CoreService::GetInstance()->GetCurrentPrimaryUser();

    int hospital_idx = m_disease_template_model->fieldIndex("hospital");
    QModelIndex hospital_index = m_disease_template_model->index(row, hospital_idx);
    m_disease_template_model->setData(hospital_index, QString::fromStdString(primary_user.GetHospital()));

    int doctor_idx = m_disease_template_model->fieldIndex("doctor");
    QModelIndex doctor_index = m_disease_template_model->index(row, doctor_idx);
    m_disease_template_model->setData(doctor_index, QString::fromStdString(primary_user.GetName()));

    int hospital_id_idx = m_disease_template_model->fieldIndex("hospital_id");
    QModelIndex dhospital_id_index = m_disease_template_model->index(row, hospital_id_idx);
    m_disease_template_model->setData(dhospital_id_index, QString::number(primary_user.GetHospitalId()));

    int doctor_id_idx = m_disease_template_model->fieldIndex("doctor_id");
    QModelIndex doctor_id_index = m_disease_template_model->index(row, doctor_id_idx);
    m_disease_template_model->setData(doctor_id_index, QString::fromStdString(primary_user.GetUid()));

    m_disease_template_model->submitAll();
    m_disease_template_model->select();
}
