#include "report_template.h"

#include <QModelIndex>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QLineEdit>
#include <qstandarditemmodel.h>
#include <QMessageBox>
#include <QListWidget>

ReportTemplate::ReportTemplate(QWidget *parent)
    : QDialog(parent)
    , m_my_report_template(this)
    , m_cloud_report_template(this)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    ui.setupUi(this);

    m_my_report_template.Init();
    m_cloud_report_template.Init();

    connect(&m_my_report_template, &MyReportTemplate::CreateTemplate, &m_cloud_report_template, &CloudReportTemplate::OnCreateTemplate);
    connect(&m_cloud_report_template, &CloudReportTemplate::ApplyReportTemplate, &m_my_report_template, &MyReportTemplate::OnApplyReportTemplate);

    //template_source_list_widget
    QListWidgetItem *my_template_item = new QListWidgetItem;
    my_template_item->setText(QString::fromLocal8Bit("我的模板"));
    my_template_item->setIcon(QIcon(":/ultrasound/res/local-template.png"));
    ui.template_source_list_widget->addItem(my_template_item);

    QListWidgetItem *cloud_template_item = new QListWidgetItem;
    cloud_template_item->setText(QString::fromLocal8Bit("云模板"));
    cloud_template_item->setIcon(QIcon(":/ultrasound/res/cloud-template.png"));
    ui.template_source_list_widget->addItem(cloud_template_item);

    connect(ui.template_source_list_widget, SIGNAL(currentRowChanged(int)),
        ui.right_stacked_widget, SLOT(setCurrentIndex(int)));

    ui.template_source_list_widget->setCurrentItem(my_template_item);

    // right_stacked_widget
    connect(ui.right_stacked_widget, SIGNAL(currentChanged(int)),
        this, SLOT(OnRightStackedWidgetCurrentChanged(int)));
}

ReportTemplate::~ReportTemplate()
{
}

QString & ReportTemplate::GetDescription()
{
    // TODO: 在此处插入 return 语句
    return m_my_report_template.GetDescription();
}

QString & ReportTemplate::GetConclusion()
{
    // TODO: 在此处插入 return 语句
    return m_my_report_template.GetConclusion();
}

void ReportTemplate::OnRightStackedWidgetCurrentChanged(int index)
{
    switch (index)
    {
    case 0: {
        break;
    }
    case 1: {
        break;
    }

    default:
        break;
    }
}

