#pragma once

#include <QDialog>
#include <QString>
#include <QTimer>
#include "ui_call.h"

#include "core_service/user_event_handler.h"

class call : public QDialog, public core::UserEventHandler
{
    Q_OBJECT

public:
    call(QWidget *parent = Q_NULLPTR);
    ~call();

    const core::SuperiorUser& GetCallee() { return m_callee; }

    // core::UserEventHandler
    void OnGetPrimaryUserInfoSuccessed(const core::PrimaryUser& user) override;
    void OnGetPrimaryUserInfoFailed(int error_code, const std::string& message) override;
    void OnGetSuperiorUserInfoSuccessed(const core::SuperiorUser& user) override;
    void OnGetSuperiorUserInfoFailed(int error_code, const std::string& message) override;
    void OnUpdatePasswordSuccessed(const std::string& uid, const std::string& new_password) override;
    void OnUpdatePasswordFailed(int error_code, const std::string& message) override;
    void OnQuerySubscriberSuperiorUserSuccessed(std::list<core::SuperiorUser> users) override;
    void OnQuerySubscriberSuperiorUserFailed(int error_code, const std::string& message) override;
    void OnQuerySuperiorUserSuccessed(std::list<core::SuperiorUser> users) override;
    void OnQuerySuperiorUserFailed(int error_code, const std::string& message) override;
    void OnUpdatePrimaryUserInfoSuccessed() override;
    void OnUpdatePrimaryUserInfoFailed(int error_code, const std::string& message) override;
    void OnUpdateSuperiorUserInfoSuccessed() override;
    void OnUpdateSuperiorUserInfoFailed(int error_code, const std::string& message) override;
    void OnQuerySubscriberSuperiorUserOnlineStatusSuccessed(std::list<core::SuperiorUserStatus> users) override;
    void OnQuerySubscriberSuperiorUserOnlineStatusFailed(int error_code, const std::string & message) override;
    void OnQueryControlPrimaryUserSuccessed(std::list<core::PrimaryUser> users) override;
    void OnQueryControlPrimaryUserFailed(int error_code, const std::string& message) override;
signals:
     void QuerySubscriberSuperiorUserSuccessedSignal();
     void QuerySubscriberSuperiorUserFailedSignal();
     void QuerySubscriberSuperiorUserStatusSuccessedSignal();


private slots:
    void OnItemClicked(QTableWidgetItem *item);
    void stoptimer();

    void QuerySubscriberSuperiorUserSuccessedSlot();
    void QuerySubscriberSuperiorUserFailedSlot();
    void QuerySubscriberSuperiorUserStatusSuccessedSlot();
private:
    void SecheduleQuerySubscriberSuperiorUserOnlineStatus(std::list<core::SuperiorUser> superior_users);

    Ui::call ui;
    core::SuperiorUser m_callee;
    std::list<core::SuperiorUser> m_subscriber_superior_users;
    std::list<core::SuperiorUserStatus> m_subscriber_superior_users_status;
    QTimer *m_scheduleTimer;    
};
