#include "call.h"

#include <QDebug>
#include <QThread>
#include <QTableWidgetItem>

#include "core_service/core_service.h"

call::call(QWidget *parent)
    : QDialog(parent)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    ui.setupUi(this);

    QStringList headers = { "uid", 
      QString::fromLocal8Bit("姓名"),
      QString::fromLocal8Bit("性别"),
      QString::fromLocal8Bit("在线"),
      QString::fromLocal8Bit("职称"), QString::fromLocal8Bit("专长"), 
      QString::fromLocal8Bit("所属医院"), QString::fromLocal8Bit("简介") };

    ui.tableWidget->clear();
    ui.tableWidget->setColumnCount(headers.count());
    ui.tableWidget->setHorizontalHeaderLabels(headers);
    ui.tableWidget->setColumnHidden(0, true);

    ui.call_button->setEnabled(false);
    connect(ui.call_button, SIGNAL(clicked()), this, SLOT(accept()));
    connect(ui.cancel_button, SIGNAL(clicked()), this, SLOT(close()));

    connect(ui.call_button, SIGNAL(clicked()), this, SLOT(stoptimer()));
    connect(ui.cancel_button, SIGNAL(clicked()), this, SLOT(stoptimer()));

    connect(ui.tableWidget, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(OnItemClicked(QTableWidgetItem*)));

    connect(this, SIGNAL(QuerySubscriberSuperiorUserSuccessedSignal()), 
      this, SLOT(QuerySubscriberSuperiorUserSuccessedSlot()));
    connect(this, SIGNAL(QuerySubscriberSuperiorUserFailedSignal()),
      this, SLOT(QuerySubscriberSuperiorUserFailedSlot()));

    connect(this, SIGNAL(QuerySubscriberSuperiorUserStatusSuccessedSignal()), 
      this, SLOT(QuerySubscriberSuperiorUserStatusSuccessedSlot()));

    m_scheduleTimer = new QTimer(this);
}

call::~call()
{
    qDebug() << "thread id" << QThread::currentThreadId();
    if (m_scheduleTimer != Q_NULLPTR) {
        m_scheduleTimer->stop();
        delete m_scheduleTimer;
        m_scheduleTimer = Q_NULLPTR;
    }
}

void call::OnGetPrimaryUserInfoSuccessed(const core::PrimaryUser& user) {}
void call::OnGetPrimaryUserInfoFailed(int error_code, const std::string& message) {}
void call::OnGetSuperiorUserInfoSuccessed(const core::SuperiorUser& user) {}
void call::OnGetSuperiorUserInfoFailed(int error_code, const std::string& message) {}
void call::OnUpdatePasswordSuccessed(const std::string& uid, const std::string& new_password) {}
void call::OnUpdatePasswordFailed(int error_code, const std::string& message) {}

void call::OnQuerySubscriberSuperiorUserSuccessed(std::list<core::SuperiorUser> users) {
    m_subscriber_superior_users.clear();
    m_subscriber_superior_users = users;

    emit QuerySubscriberSuperiorUserSuccessedSignal();
}

void call::OnQuerySubscriberSuperiorUserFailed(int error_code, const std::string& message) {
    m_subscriber_superior_users.clear();
    emit QuerySubscriberSuperiorUserFailedSignal();
}

void call::OnQuerySuperiorUserSuccessed(std::list<core::SuperiorUser> users) {}
void call::OnQuerySuperiorUserFailed(int error_code, const std::string& message) {}
void call::OnUpdatePrimaryUserInfoSuccessed() {}
void call::OnUpdatePrimaryUserInfoFailed(int error_code, const std::string& message) {}
void call::OnUpdateSuperiorUserInfoSuccessed() {}
void call::OnUpdateSuperiorUserInfoFailed(int error_code, const std::string& message) {}
void call::OnQuerySubscriberSuperiorUserOnlineStatusSuccessed(std::list<core::SuperiorUserStatus> users_status){
    m_subscriber_superior_users_status.clear();
    m_subscriber_superior_users_status = users_status;
    qDebug() << "query users status";
    emit QuerySubscriberSuperiorUserStatusSuccessedSignal();
}
void call::OnQuerySubscriberSuperiorUserOnlineStatusFailed(int error_code, const std::string & message)
{
}
void call::OnQueryControlPrimaryUserSuccessed(std::list<core::PrimaryUser> users) {}
void call::OnQueryControlPrimaryUserFailed(int error_code, const std::string& message) {}

void call::OnItemClicked(QTableWidgetItem* item) {
    int row = item->row();
    QTableWidgetItem* id_item = ui.tableWidget->item(row, 0);
    QTableWidgetItem* name_item = ui.tableWidget->item(row, 1);
    if (id_item && !id_item->text().isEmpty()) {

		std::string callee_uid = id_item->text().toStdString();
		std::list<core::SuperiorUser>::iterator it = std::find_if(m_subscriber_superior_users.begin(), m_subscriber_superior_users.end(),
			[&callee_uid](const core::SuperiorUser& user) { return user.GetUid() == callee_uid; });

		if(it != m_subscriber_superior_users.end()) {
			m_callee = *it;
			ui.call_button->setEnabled(true);
		}
		else {
			ui.call_button->setEnabled(false);
		}
    }
    else {
        ui.call_button->setEnabled(false);
    }
}

void call::stoptimer()
{


}

void call::QuerySubscriberSuperiorUserSuccessedSlot() {
    QStringList headers = { "uid", 
      QString::fromLocal8Bit("姓名"), 
      QString::fromLocal8Bit("性别"),
      QString::fromLocal8Bit("在线"),
      QString::fromLocal8Bit("职称"), QString::fromLocal8Bit("专长"),
      QString::fromLocal8Bit("所属医院"), QString::fromLocal8Bit("简介") };

    ui.tableWidget->clear();
    ui.tableWidget->setColumnCount(headers.count());
    ui.tableWidget->setHorizontalHeaderLabels(headers);
    ui.tableWidget->setColumnHidden(0, true);
    ui.tableWidget->setRowCount(m_subscriber_superior_users.size());

    int row = 0;
    for (const core::SuperiorUser& user : m_subscriber_superior_users) {
        QTableWidgetItem* id_item = new QTableWidgetItem();
        id_item->setText(QString::fromStdString(user.GetUid()));
        ui.tableWidget->setItem(row, 0, id_item);

        QTableWidgetItem* name_item = new QTableWidgetItem();
        name_item->setText(QString::fromStdString(user.GetName()));
        ui.tableWidget->setItem(row, 1, name_item);

        QTableWidgetItem* sex_item = new QTableWidgetItem();
        sex_item->setText(QString::fromStdString(user.GetSex()));
        ui.tableWidget->setItem(row, 2, sex_item);

        QTableWidgetItem* online_item = new QTableWidgetItem();
        online_item->setText(QString::fromLocal8Bit("离线"));
		online_item->setBackgroundColor(QColor(255, 0, 0));
        ui.tableWidget->setItem(row, 3, online_item);

        QTableWidgetItem* title_item = new QTableWidgetItem();
        title_item->setText(QString::fromStdString(user.GetTitle()));
        ui.tableWidget->setItem(row, 4, title_item);

        QTableWidgetItem* skill_item = new QTableWidgetItem();
        skill_item->setText(QString::fromStdString(user.GetSkill()));
        ui.tableWidget->setItem(row, 5, skill_item);

        QTableWidgetItem* hospital_item = new QTableWidgetItem();
        hospital_item->setText(QString::fromStdString(user.GetHospital()));
        ui.tableWidget->setItem(row, 6, hospital_item);

        QTableWidgetItem* description_item = new QTableWidgetItem();
        description_item->setText(QString::fromStdString(user.GetDescription()));
        ui.tableWidget->setItem(row, 7, description_item);

        row++;
    }

    ui.tableWidget->setColumnWidth(2, 0.5 * ui.tableWidget->columnWidth(1));
    ui.tableWidget->setColumnWidth(4, 2 * ui.tableWidget->columnWidth(3));
    ui.tableWidget->setColumnWidth(5, 1.25 * ui.tableWidget->columnWidth(3));
    ui.tableWidget->setColumnWidth(6, 3 * ui.tableWidget->columnWidth(3));

    //开始获取在线状态
    
    core::CoreService::GetInstance()->QuerySubscriberSuperiorUserOnlineStatus(m_subscriber_superior_users);

}

void call::QuerySubscriberSuperiorUserFailedSlot() {}

void call::QuerySubscriberSuperiorUserStatusSuccessedSlot(){
    for(int row = 0; row < ui.tableWidget->rowCount(); ++row ){
        for (const core::SuperiorUserStatus& user_status : m_subscriber_superior_users_status) {
            QTableWidgetItem* uid_item = ui.tableWidget->item(row,0);
            QString uid = uid_item->text();
            if(0==uid.compare(QString::fromStdString(user_status.GetUid()),Qt::CaseSensitivity::CaseInsensitive)){
                QTableWidgetItem* online_item = ui.tableWidget->item(row,3);
                if (QString::fromStdString(user_status.GetOnlineAOS()).compare("1", Qt::CaseSensitivity::CaseInsensitive) == 0 ||
					(!QString::fromStdString(user_status.GetOnlinePC()).isEmpty()&&QString::fromStdString(user_status.GetOnlinePC()).compare("1", Qt::CaseSensitivity::CaseInsensitive) == 0)) {
					online_item->setText(QString::fromLocal8Bit("在线"));
					online_item->setBackgroundColor(QColor(0, 255, 0));
                }
                else {
					online_item->setText(QString::fromLocal8Bit("离线"));
					online_item->setBackgroundColor(QColor(255, 0, 0));
                }
            }
        }
        
    }
    SecheduleQuerySubscriberSuperiorUserOnlineStatus(m_subscriber_superior_users);
}



void call::SecheduleQuerySubscriberSuperiorUserOnlineStatus(std::list<core::SuperiorUser> superior_users){
    if (!m_scheduleTimer->isActive()) {
        qDebug() << "thread id:" << QThread::currentThreadId();
        connect(m_scheduleTimer, &QTimer::timeout, [=]() {
            core::CoreService::GetInstance()->QuerySubscriberSuperiorUserOnlineStatus(superior_users);
        });
        m_scheduleTimer->start(5000);
    }
}
