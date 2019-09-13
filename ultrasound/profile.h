#pragma once

#include <QDialog>
#include "ui_profile.h"
#include "core_service/user_event_handler.h"

class Profile : public QDialog, public core::UserEventHandler
{
    Q_OBJECT

public:
    Profile(QWidget *parent = Q_NULLPTR);
    ~Profile();

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
    void UpdatePrimaryUserInfoSuccessedSignal();
    void UpdatePrimaryUserInfoFailedSignal(const QString& message);

private slots:
    void UpdateProfile();
    void ReturnSlot();

    void UpdatePrimaryUserInfoSuccessedSlot();
    void UpdatePrimaryUserInfoFailedSlot(const QString& message);

private:
    Ui::Profile ui;
  
};
