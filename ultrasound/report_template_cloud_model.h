#pragma once
#include <QModelIndex>
#include "core_service/template_event_handler.h"
#include <unordered_map>
#include <map>
#include <mutex>
#include <QMutex>
#include <QWaitCondition>


class CloudDiseaseTableModel : public QAbstractTableModel, public core::TemplateEventHandler
{
public:
    CloudDiseaseTableModel();
    ~CloudDiseaseTableModel();

    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    virtual bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    virtual QVariant headerData(int section, Qt::Orientation orientation,
        int role = Qt::DisplayRole) const;
    virtual bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value,
        int role = Qt::EditRole);
    virtual bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    void do_wait();

    virtual bool select(std::map<std::string,std::string> conditions = std::map<std::string, std::string>());
    virtual void prev();
    virtual void next();
    bool submitAll();
    int fieldIndex(const QString &fieldName) const;
protected:
    virtual void OnGetTemplateSuccessed(const std::list<core::ReportTemplate>& list);
    virtual void OnGetTemplateFailed(int error_code, const std::string& cs);
    virtual void OnQueryTemplateSuccessed(const std::list<core::ReportTemplate>& list);
    virtual void OnQueryTemplateFailed(int error_code, const std::string& cs);
    virtual void OnCreateTemplateSuccessed();
    virtual void OnCreateTemplateFailed(int error_code, const std::string& message);
    virtual void OnUpdateTemplateSuccessed();
    virtual void OnUpdateTemplateFailed(int error_code, const std::string& message);
    virtual void OnDeleteTemplateSuccessed();
    virtual void OnDeleteTemplateFailed(int error_code, const std::string& message);
private:
    void Reset()
    {
        m_skip = 0;
        m_limit = 20;
    }
    int m_skip = 0;
    int m_limit = 20;
    QVector<QHash<int, QVariant> > headers;
    std::vector<core::ReportTemplate> m_templates;
    std::unordered_map<int, core::ReportTemplate> m_modify_templates;
    QWaitCondition m_action_conditon;
    QMutex m_action_mutex;
    std::mutex m_template_lock;
    std::map<std::string, std::string> m_query_conditions;
};
