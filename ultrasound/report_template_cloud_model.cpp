#include "report_template_cloud_model.h"
#include "core_service/core_service.h"

#include <string>
#include <unordered_map>
#include <QWaitCondition>
#include <QMutex>
#include <QDebug>


CloudDiseaseTableModel::CloudDiseaseTableModel()
{
    core::CoreService::GetInstance()->AddTemplateEventHandler(this);
}

CloudDiseaseTableModel::~CloudDiseaseTableModel()
{
    core::CoreService::GetInstance()->RemoveTemplateEventHandler(this);
}

int CloudDiseaseTableModel::rowCount(const QModelIndex & parent) const
{
    return m_templates.size() + m_modify_templates.size();
}

int CloudDiseaseTableModel::columnCount(const QModelIndex & parent) const
{
    return 11;
}

QVariant CloudDiseaseTableModel::data(const QModelIndex & index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::TextAlignmentRole) {
        return int(Qt::AlignRight | Qt::AlignVCenter);
    }
    else if (role == Qt::DisplayRole) {
        auto row_template = m_templates[index.row()];
        if (index.column() == 0)
        {
            return QString("%1").arg(index.row());
        }

        if (index.column() == 1)
        {
            return QString::fromStdString(row_template.Id());
        }

        if (index.column() == 2)
        {
            return QString::fromStdString(row_template.Disease_type());
        }

        if (index.column() == 3)
        {
            return QString::fromStdString(row_template.Disease_name());
        }
        if (index.column() == 4)
        {
            return QString::fromStdString(row_template.Disease_description());
        }
        if (index.column() == 5)
        {
            return QString::fromStdString(row_template.Disease_conclusion());
        }
        if (index.column() == 6)
        {
            return QString::fromStdString(row_template.Hospital());
        }
        if (index.column() == 7)
        {
            return QString::fromStdString(row_template.Doctor());
        }
        if (index.column() == 8)
        {
            return QString::fromStdString(row_template.Create_time());
        }
        if (index.column() == 9)
        {
            return QString("%1").arg(row_template.Hospital_id());
        }
        if (index.column() == 10)
        {
            return QString::fromStdString(row_template.Doctor_id());
        }
        return QString("");
    }
    return QVariant();
}

bool CloudDiseaseTableModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
    int row = index.row();
    core::ReportTemplate& report_template =m_modify_templates[row];
    if (index.column() == 1)
    {
        // id
        QString id = value.toString();
        report_template.Id(id.toStdString());
    }
    else if (index.column() == 2)
    {
        // disease_type
        QString disease_type = value.toString();
        report_template.Disease_type(disease_type.toStdString());
    }
    else if (index.column() == 3)
    {
        // disease_name
        QString disease_name = value.toString();
        report_template.Disease_name(disease_name.toStdString());
    }
    else if (index.column() == 4)
    {
        // disease_description
        QString disease_description = value.toString();
        report_template.Disease_description(disease_description.toStdString());
    }
    else if (index.column() == 5)
    {
        // disease_conclusion
        QString disease_conclusion = value.toString();
        report_template.Disease_conclusion(disease_conclusion.toStdString());
    }
    else if (index.column() == 6)
    {
        // hospital
        QString hospital = value.toString();
        report_template.Hospital(hospital.toStdString());
    }
    else if (index.column() == 7)
    {
        // doctor
        QString doctor = value.toString();
        report_template.Doctor(doctor.toStdString());
    }
    else if (index.column() ==8)
    {
        // create time
        QString create_time = value.toString();
        report_template.Create_time(create_time.toStdString());
    }
    else if (index.column() == 9)
    {
        // hospital id
        QString hospital_id = value.toString();
        report_template.Hospital_id(hospital_id.toLong());
    }
    else if (index.column() == 10)
    {
        // doctor id
        QString doctor_id = value.toString();
        report_template.Doctor_id(doctor_id.toStdString());
    }
   
    emit dataChanged(index, index);
    return true;
}

bool CloudDiseaseTableModel::removeRows(int row, int count, const QModelIndex & parent)
{
    for (int i = row; i < row + count; ++i)
    {
        std::map<std::string, std::string> conditions ={
            { std::string("_id"),m_templates[i].Id() }
        };
        core::CoreService::GetInstance()->AddTemplateEventHandler(this);
        core::CoreService::GetInstance()->DeleteTemplate(conditions);
        emit headerDataChanged(Qt::Vertical, i, i);
    }

    return true;
}

QVariant CloudDiseaseTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal) {
        QVariant val = headers.value(section).value(role);
        if (role == Qt::DisplayRole && !val.isValid())
            val = headers.value(section).value(Qt::EditRole);
        if (val.isValid())
            return val;
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}

bool CloudDiseaseTableModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant & value, int role)
{
    if (orientation != Qt::Horizontal || section < 0 || columnCount() <= section)
        return false;

    if (headers.size() <= section)
        headers.resize(qMax(section + 1, 16));
    headers[section][role] = value;
    emit headerDataChanged(orientation, section, section);
    return true;
}

bool CloudDiseaseTableModel::insertRows(int row, int count, const QModelIndex & parent)
{
    beginInsertRows(parent, row, row + count - 1);
    for(int i= row; i< row+count; ++i)
    {
        m_modify_templates[i];
    }
    endInsertRows();
    return true;
}

void CloudDiseaseTableModel::do_wait()
{
    m_action_mutex.lock();
    m_action_conditon.wait(&m_action_mutex);
    m_action_mutex.unlock();
}

bool CloudDiseaseTableModel::select(std::map<std::string, std::string> conditions)
{
    m_templates.clear();
    if(conditions.size()>0)
    {
        m_query_conditions.clear();
    }
    
    if(conditions.size()>0)
    {
        std::copy(conditions.begin(), conditions.end(), std::inserter(m_query_conditions, m_query_conditions.begin()));
        m_query_conditions["skip"] = std::to_string(m_skip);
        m_query_conditions["limit"] = std::to_string(m_limit);
        core::CoreService::GetInstance()->QueryTemplate(m_query_conditions);
    }
    else
    {
        m_query_conditions["skip"] = std::to_string(m_skip);
        m_query_conditions["limit"] = std::to_string(m_limit);
        core::CoreService::GetInstance()->GetTemplates(m_query_conditions);
    }

    qDebug() << "wait select return";
    do_wait();
    qDebug() << "the select has been return";
    emit dataChanged(createIndex(0, 1), createIndex(1, 1));
    return true;
}

void CloudDiseaseTableModel::prev()
{
    if(m_skip >= m_limit)
    {
        m_skip -= m_limit;
    }
    select();
}

void CloudDiseaseTableModel::next()
{
    m_skip += m_limit;
    select();
}

bool CloudDiseaseTableModel::submitAll()
{
    for (auto it: m_modify_templates)
    {
        Json::Value report_template_json;
        auto report_template = it.second;
        if(report_template.Id().empty())
        {
            // create
            report_template_json["hospital"] = report_template.Hospital();
            report_template_json["doctor"] = report_template.Doctor();
            report_template_json["hospital_id"] = report_template.Hospital_id();
            report_template_json["doctor_id"] = report_template.Doctor_id();
            report_template_json["disease_type"] = report_template.Disease_type();
            report_template_json["disease_name"] = report_template.Disease_name();
            report_template_json["disease_description"] = report_template.Disease_description();
            report_template_json["disease_conclusion"] = report_template.Disease_conclusion();
            
            core::CoreService::GetInstance()->CreateTemplate(report_template_json);
            do_wait();
        }
        else
        {
            // update
            report_template_json["_id"] = report_template.Id();
            report_template_json["disease_type"] = report_template.Disease_type();
            report_template_json["disease_name"] = report_template.Disease_name();
            report_template_json["disease_description"] = report_template.Disease_description();
            report_template_json["disease_conclusion"] = report_template.Disease_conclusion();
            core::CoreService::GetInstance()->AddTemplateEventHandler(this);
            core::CoreService::GetInstance()->UpdateTemplate(report_template_json);
            do_wait();
        }
        
    }
    m_modify_templates.clear();
    return true;
}

int CloudDiseaseTableModel::fieldIndex(const QString & fieldName) const
{
    if (fieldName.compare(fieldName, "index", Qt::CaseInsensitive) == 0)
    {
        return 0;
    }
    if (fieldName.compare(fieldName, "id", Qt::CaseInsensitive) == 0)
    {
        return 1;
    }
    if (fieldName.compare(fieldName, "disease_type", Qt::CaseInsensitive) == 0)
    {
        return 2;
    }
    if (fieldName.compare(fieldName, "disease_name", Qt::CaseInsensitive) == 0)
    {
        return 3;
    }
    if (fieldName.compare(fieldName, "disease_description", Qt::CaseInsensitive) == 0)
    {
        return 4;
    }
    if (fieldName.compare(fieldName, "disease_conclusion", Qt::CaseInsensitive) == 0)
    {
        return 5;
    }
    if (fieldName.compare(fieldName, "hospital", Qt::CaseInsensitive) == 0)
    {
        return 6;
    }
    if (fieldName.compare(fieldName, "doctor", Qt::CaseInsensitive) == 0)
    {
        return 7;
    }
    if (fieldName.compare(fieldName, "create_time", Qt::CaseInsensitive) == 0)
    {
        return 8;
    }
    if (fieldName.compare(fieldName, "hospital_id", Qt::CaseInsensitive) == 0)
    {
        return 9;
    }
    if (fieldName.compare(fieldName, "doctor_id", Qt::CaseInsensitive) == 0)
    {
        return 10;
    }
    return -1;
}

void CloudDiseaseTableModel::OnGetTemplateSuccessed(const std::list<core::ReportTemplate>& report_templates)
{
    m_templates.clear();
    for (core::ReportTemplate report_template : report_templates)
    {
        std::unique_lock<std::mutex> locker(m_template_lock);
        m_templates.push_back(report_template);
    }
    qDebug() << "call OnGetTemplateSuccessed";
    m_action_mutex.lock();
    m_action_conditon.wakeAll();
    m_action_mutex.unlock();
    qDebug() << "call OnGetTemplateSuccessed return";
}
void CloudDiseaseTableModel::OnGetTemplateFailed(int error_code, const std::string& cs)
{
    qDebug() << "call OnGetTemplateFailed";
    m_action_mutex.lock();
    m_action_conditon.wakeAll();
    m_action_mutex.unlock();
    qDebug() << "call OnGetTemplateFailed return";
}
void CloudDiseaseTableModel::OnQueryTemplateSuccessed(const std::list<core::ReportTemplate>& report_templates)
{
    m_templates.clear();
    for (core::ReportTemplate report_template : report_templates)
    {
        std::unique_lock<std::mutex> locker(m_template_lock);
        m_templates.push_back(report_template);
    }
    qDebug() << "call OnQueryTemplateSuccessed";
    m_action_mutex.lock();
    m_action_conditon.wakeAll();
    m_action_mutex.unlock();
    qDebug() << "call OnQueryTemplateSuccessed return";
}
void CloudDiseaseTableModel::OnQueryTemplateFailed(int error_code, const std::string& cs)
{
    qDebug() << "call OnQueryTemplateFailed";
    m_action_mutex.lock();
    m_action_conditon.wakeAll();
    m_action_mutex.unlock();
    qDebug() << "call OnQueryTemplateFailed return";
}

void CloudDiseaseTableModel::OnCreateTemplateSuccessed()
{
    qDebug() << "call OnCreateTemplateSuccessed";
    m_action_mutex.lock();
    m_action_conditon.wakeAll();
    m_action_mutex.unlock();
    qDebug() << "call OnCreateTemplateSuccessed return";
}

void CloudDiseaseTableModel::OnCreateTemplateFailed(int error_code, const std::string & message)
{
    qDebug() << "call OnCreateTemplateFailed";
    m_action_mutex.lock();
    m_action_conditon.wakeAll();
    m_action_mutex.unlock();
    qDebug() << "call OnCreateTemplateFailed return";
}

void CloudDiseaseTableModel::OnUpdateTemplateSuccessed()
{
    qDebug() << "call OnDeleteTemplateFailed";
    m_action_mutex.lock();
    m_action_conditon.wakeAll();
    m_action_mutex.unlock();
    qDebug() << "call OnQueryTemplateSuccessed return";
}

void CloudDiseaseTableModel::OnUpdateTemplateFailed(int error_code, const std::string & message)
{
    qDebug() << "call OnUpdateTemplateFailed";
    m_action_mutex.lock();
    m_action_conditon.wakeAll();
    m_action_mutex.unlock();
    qDebug() << "call OnQueryTemplateSuccessedOnUpdateTemplateFailed return";
}

void CloudDiseaseTableModel::OnDeleteTemplateSuccessed()
{
    qDebug() << "call OnDeleteTemplateSuccessed";
    m_action_mutex.lock();
    m_action_conditon.wakeAll();
    m_action_mutex.unlock();
    qDebug() << "call OnDeleteTemplateSuccessed return";
}

void CloudDiseaseTableModel::OnDeleteTemplateFailed(int error_code, const std::string & message)
{
    qDebug() << "call OnDeleteTemplateFailed";
    m_action_mutex.lock();
    m_action_conditon.wakeAll();
    m_action_mutex.unlock();
    qDebug() << "call OnDeleteTemplateFailed return";
}
