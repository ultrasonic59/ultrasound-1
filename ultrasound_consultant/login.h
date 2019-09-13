#pragma once

#include <QDialog>
#include "ui_login.h"

#include "core_service/login_event_handler.h"
#include "nim_service/client_event_handler.h"

class login : public QDialog,
              public nim::ClientEventHandler,
              public core::LoginEventHandler
{
    Q_OBJECT

public:
    login(QWidget *parent = Q_NULLPTR);
    ~login();

    // nim::LoginEventHandler
    void OnLogin(const std::list<OtherClientInfo>& clients) override;
    void OnLogout() override;
    void OnLoginInfo(Info info) override;
    void OnLoginError(Error error, int code, const std::string& message) override;
    void OnKickOut(ClientType client_type, KickReason kick_reason) override;

    //core::CoreServiceEventHandler
    void OnCoreLoginSuccessed() override;
    void OnCoreLoginFailed(int error_code, const std::string& message) override;
    void OnFetchTokenSuccessed() override;
    void OnFetchTokenFailed(int error_code, const std::string& message) override;

signals:
    void LoginedSignal();
    void LogoutedSignal();
    void LoginInfoSignal(const QString& info);
    void LoginErrorSignal(const QString& error);

public slots:
    void Login();

    void Logined();
    void Logouted();
    void LoginInfo(const QString& info);
    void LoginError(const QString& error);

    void OnIdTextEdited(const QString&);
    void OnPassTextEdited(const QString&);

    void OnCurrentIndexChanged(const QString &text);

private:
    Ui::login ui;
};
