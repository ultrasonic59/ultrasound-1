#include "report_template.h"
#include "report_template_my.h"
#include "report_template_disease_type_add.h"
#include <QSqlRecord>
#include <QLineEdit>
#include <QMessageBox>
#include <QSqlQuery>
#include "report_template_disease_name_add.h"

MyReportTemplate::MyReportTemplate(ReportTemplate* temlate)
    : m_temlate(temlate)
{

}

void MyReportTemplate::Init()
{
    connect(m_temlate->ui.ok_button, SIGNAL(clicked()), m_temlate, SLOT(accept()));
    connect(m_temlate->ui.cancel_button, SIGNAL(clicked()), m_temlate, SLOT(reject()));

    connect(m_temlate->ui.disease_type_add_button, SIGNAL(clicked()), this, SLOT(OnDiseaseTypeAddItem()));
    connect(m_temlate->ui.disease_type_remove_button, SIGNAL(clicked()), this, SLOT(OnDiseaseTypeRemoveItem()));

    connect(m_temlate->ui.disease_name_add_button, SIGNAL(clicked()), this, SLOT(OnDiseaseNameAddItem()));
    connect(m_temlate->ui.disease_name_remove_button, SIGNAL(clicked()), this, SLOT(OnDiseaseNameRemoveItem()));

    m_temlate->ui.disease_type_add_button->setIcon(QIcon(":/ultrasound/add"));
    m_temlate->ui.disease_type_remove_button->setIcon(QIcon(":/ultrasound/remove"));
    m_temlate->ui.disease_name_add_button->setIcon(QIcon(":/ultrasound/add"));
    m_temlate->ui.disease_name_remove_button->setIcon(QIcon(":/ultrasound/remove"));

    m_disease_type_model = new QSqlTableModel;
    m_disease_type_model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    m_disease_type_model->setTable("disease_type");
    m_disease_type_model->setHeaderData(0, Qt::Horizontal, QString::fromLocal8Bit("序号"),
        Qt::DisplayRole);
    m_disease_type_model->setHeaderData(1, Qt::Horizontal, QString::fromLocal8Bit("疾病类别"),
        Qt::DisplayRole);

    m_disease_type_model->select();

    m_temlate->ui.disease_type_table_view->setModel(m_disease_type_model);

    m_temlate->ui.disease_type_table_view->setColumnWidth(0, 80);
    m_temlate->ui.disease_type_table_view->setColumnWidth(1, 200);
    m_temlate->ui.disease_type_table_view->setStyleSheet(
        "QHeaderView::section { border: 1px solid black}");
    m_temlate->ui.disease_type_table_view->horizontalHeader()->setFixedHeight(30);

    QItemSelectionModel *sm = m_temlate->ui.disease_type_table_view->selectionModel();
    connect(sm, SIGNAL(currentRowChanged(const QModelIndex&, const QModelIndex&)),
        this, SLOT(OnDiseaseTypeTableCurrentRowChanged(const QModelIndex&, const QModelIndex)));

    m_temlate->ui.disease_type_table_view->selectRow(0);
}

void MyReportTemplate::OnDiseaseTypeTableCurrentRowChanged(const QModelIndex& current, const QModelIndex& previous)
{
    int row = current.row();
    m_current_disease_type_edit_row = row;
    QString id = current.sibling(row, 0).data().toString();
    m_current_disease_type_id = id;
    m_temlate->ui.disease_type_remove_button->setEnabled(true);
    m_temlate->ui.disease_name_remove_button->setEnabled(false);
    m_temlate->Clear();

    m_disease_name_model = new QSqlTableModel;
    m_disease_name_model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    m_disease_name_model->setTable("disease_name");
    m_disease_name_model->setFilter("type_id = " + id);
    int id_idx = m_disease_name_model->fieldIndex("id");
    m_disease_name_model->setHeaderData(id_idx, Qt::Horizontal, QString::fromLocal8Bit("序号"),
        Qt::DisplayRole);
    int disease_name_idx = m_disease_name_model->fieldIndex("disease_name");
    m_disease_name_model->setHeaderData(disease_name_idx, Qt::Horizontal, QString::fromLocal8Bit("疾病名称"),
        Qt::DisplayRole);
    m_disease_name_model->select();

    m_temlate->ui.disease_name_table_view->setModel(m_disease_name_model);
    m_temlate->ui.disease_name_table_view->setColumnWidth(0, 80);
    m_temlate->ui.disease_name_table_view->setColumnWidth(2, 200);
    m_temlate->ui.disease_name_table_view->setStyleSheet(
        "QHeaderView::section { border: 1px solid black}");
    m_temlate->ui.disease_name_table_view->horizontalHeader()->setFixedHeight(30);

    int type_id_idx = m_disease_name_model->fieldIndex("type_id");
    m_temlate->ui.disease_name_table_view->hideColumn(type_id_idx);
    int description_idx = m_disease_name_model->fieldIndex("description");
    m_temlate->ui.disease_name_table_view->hideColumn(description_idx);
    int conclusion_idx = m_disease_name_model->fieldIndex("conclusion");
    m_temlate->ui.disease_name_table_view->hideColumn(conclusion_idx);

    QItemSelectionModel *sm = m_temlate->ui.disease_name_table_view->selectionModel();
    connect(sm, SIGNAL(currentRowChanged(const QModelIndex&, const QModelIndex&)),
        this, SLOT(OnDiseaseNameTableCurrentRowChanged(const QModelIndex&, const QModelIndex)));

    //Clear();
}

void MyReportTemplate::OnDiseaseNameTableCurrentRowChanged(const QModelIndex & current, const QModelIndex & previous)
{
    int row = current.row();
    m_current_disease_name_edit_row = row;
    QString id = current.sibling(row, 0).data().toString();

    m_temlate->ui.disease_name_remove_button->setEnabled(true);

    m_description.clear();
    m_conclusion.clear();
    m_temlate->Clear();

    QSqlQuery query("SELECT description, conclusion FROM disease_name WHERE id=" + id);
    if (query.exec()) {
        query.next();
        if (m_description.isEmpty())
        {
            m_description = query.value("description").toString();
        }
        else
        {
            m_description = m_description + QString::fromLocal8Bit("\n\n") + query.value("description").toString();
        }
        if (m_conclusion.isEmpty())
        {
            m_conclusion = query.value("conclusion").toString();
        }
        else
        {
            m_conclusion = m_conclusion + QString::fromLocal8Bit("\n\n") + query.value("conclusion").toString();
        }

    }
    m_temlate->ui.description_edit->setText(m_description);
    m_temlate->ui.conclusion_edit->setText(m_conclusion);
}

void MyReportTemplate::OnDiseaseTypeAddItem()
{
    ReportTemplateDiseaseTypeAddDialog dialog;
    if (dialog.exec() == QDialog::Accepted) {
        QSqlRecord record = m_disease_type_model->record();
        record.setValue("disease_type", dialog.GetDiseaseTypeName());
        m_disease_type_model->insertRecord(m_disease_type_model->rowCount(), record);
        m_disease_type_model->submitAll();
    }
}

void MyReportTemplate::OnDiseaseTypeRemoveItem()
{
    QMessageBox::StandardButton rb = QMessageBox::information(NULL, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("确认删除该类型?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    if (rb == QMessageBox::Yes)
    {
        m_disease_type_model->removeRow(m_current_disease_type_edit_row);
        m_disease_type_model->submitAll();
        m_temlate->Clear();
    }
}

void MyReportTemplate::OnDiseaseNameAddItem()
{
    ReportTemplateDiseaseNameAddDialog dialog;
    if (dialog.exec() == QDialog::Accepted) {
        QSqlRecord record = m_disease_name_model->record();
        record.setValue("type_id", m_current_disease_type_id);
        record.setValue("disease_name", dialog.GetName());
        record.setValue("description", dialog.GetDescription());
        record.setValue("conclusion", dialog.GetConclusion());
        m_disease_name_model->insertRecord(m_disease_name_model->rowCount(), record);
        m_disease_name_model->submitAll();

        int disease_type_idx = m_disease_type_model->fieldIndex("disease_type");
        QModelIndex disease_type_index = m_disease_type_model->index(m_current_disease_type_edit_row, disease_type_idx);
        QString disease_type = m_disease_type_model->data(disease_type_index).toString();
        core::ReportTemplate report_template;
        report_template.Disease_type(disease_type.toStdString());
        report_template.Disease_name(dialog.GetName().toStdString());
        report_template.Disease_description(dialog.GetDescription().toStdString());
        report_template.Disease_conclusion(dialog.GetConclusion().toStdString());
        emit CreateTemplate(report_template);
    }
}

void MyReportTemplate::OnDiseaseNameRemoveItem()
{
    QMessageBox::StandardButton rb = QMessageBox::information(NULL, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("确认删除该类型?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    if (rb == QMessageBox::Yes)
    {
        m_disease_name_model->removeRow(m_current_disease_name_edit_row);
        m_disease_name_model->submitAll();
        m_temlate->Clear();
    }
}

void MyReportTemplate::OnApplyReportTemplate(core::ReportTemplate report_template)
{
    auto disease_type = report_template.Disease_type();
    auto disease_name = report_template.Disease_name();
    auto disease_description = report_template.Disease_description();
    auto disease_conclusion = report_template.Disease_conclusion();

    QSqlQuery query("SELECT * FROM disease_type WHERE disease_type='" + QString::fromStdString(disease_type) + "'");
    if (!query.exec())
    {
        return;
    }
    QString type_id = "";
    if (query.record().count() == 0)
    {
        QSqlRecord type_record = m_disease_type_model->record();
        type_record.setValue("disease_type", QString::fromStdString(disease_type));
        m_disease_type_model->insertRecord(m_disease_type_model->rowCount(), type_record);
        m_disease_type_model->submitAll();
        type_id = type_record.value("id").toString();
    }
    else
    {
        query.next();
        type_id = query.value("id").toString();
    }

    QSqlRecord name_record = m_disease_name_model->record();
    name_record.setValue("type_id", type_id);
    name_record.setValue("disease_name", QString::fromStdString(disease_name));
    name_record.setValue("description", QString::fromStdString(disease_description));
    name_record.setValue("conclusion", QString::fromStdString(disease_conclusion));
    m_disease_name_model->insertRecord(m_disease_name_model->rowCount(), name_record);
    m_disease_name_model->submitAll();



}
