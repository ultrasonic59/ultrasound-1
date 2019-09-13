#include "login.h"

#include <QCryptographicHash>

#include "config.h"
#include "core_service/core_service.h"
#include "nim_service/nim_service.h"

login::login(QWidget *parent)
    : QDialog(parent)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui.setupUi(this);

    ui.idComboBox->addItems(Config::GetInstance()->GetLoginUserIdList());
    if (!ui.idComboBox->currentText().isEmpty())
    {
        Config::LoginUserInfo user_info = Config::GetInstance()->GetLoginUserInfo(ui.idComboBox->currentText());
        if (!user_info.id.isEmpty() && !user_info.password.isEmpty())
        {
            ui.rememberCheckBox->setChecked(true);
            ui.passLineEdit->setText(user_info.password);
        }
    }

    setFixedSize(width(), height());

    connect(ui.loginPushButton, SIGNAL(clicked()), this, SLOT(Login()));

    connect(this, SIGNAL(LoginedSignal()), this, SLOT(Logined()));
    connect(this, SIGNAL(LogoutedSignal()), this, SLOT(Logouted()));
    connect(this, SIGNAL(LoginInfoSignal(const QString&)), this, SLOT(LoginInfo(const QString&)));
    connect(this, SIGNAL(LoginErrorSignal(const QString&)), this, SLOT(LoginError(const QString&)));

    connect(ui.idComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(OnCurrentIndexChanged(const QString&)));

    connect(ui.idComboBox->lineEdit(), SIGNAL(textEdited(const QString&)), this, SLOT(OnIdTextEdited(const QString&)));
    connect(ui.passLineEdit, SIGNAL(textEdited(const QString&)), this, SLOT(OnPassTextEdited(const QString&)));
}

login::~login()
{
}

void login::OnLogin(const std::list<OtherClientInfo>& clients) {
    emit LoginedSignal();
}

void login::OnLogout() { }

void login::OnLoginInfo(ClientEventHandler::Info info) {
    switch (info) {
    case ClientEventHandler::InfoLinking : {
        emit LoginInfoSignal(QString::fromLocal8Bit("正在连接音视频服务器..."));
    }
    case ClientEventHandler::InfoLink : {
        emit LoginInfoSignal(QString::fromLocal8Bit("已连接音视频服务器..."));
    }
    case ClientEventHandler::InfoLogining : {
        emit LoginInfoSignal(QString::fromLocal8Bit("正在登录音视频服务器..."));
    }
    default:
        break;
    }
}

void login::OnLoginError(Error error, int code, const std::string& message) {
    switch (error)
    {
    case ClientEventHandler::ERROR_USER_TOKEN: {
        if (message.empty())
            emit LoginErrorSignal(QString::fromLocal8Bit("获取用户Token失败！(code = ") +
                QString::number(code) + QString::fromLocal8Bit(")"));
        else
            emit LoginErrorSignal(QString::fromStdString(message) + QString::fromLocal8Bit("(code = ") +
                QString::number(code) + QString::fromLocal8Bit(")"));
        break;
    }
    case ClientEventHandler::ERROR_USER_PASS: {
        if (message.empty())
            emit LoginErrorSignal(QString::fromLocal8Bit("用户名密码或错误！(code = ") +
                QString::number(code) + QString::fromLocal8Bit(")"));
        else
            emit LoginErrorSignal(QString::fromStdString(message) + QString::fromLocal8Bit("(code = ") +
                QString::number(code) + QString::fromLocal8Bit(")"));
        break;
    }
    default: {
        if (message.empty())
            if(code == core::LoginEventHandler::ErrorRole)
                emit LoginErrorSignal(QString::fromLocal8Bit("登录失败！当前账户不是基层医生账户！(code = ") +
                    QString::number(code) + QString::fromLocal8Bit(")"));
            else
                emit LoginErrorSignal(QString::fromLocal8Bit("登录失败！(code = ") +
                    QString::number(code) + QString::fromLocal8Bit(")"));
        else
            emit LoginErrorSignal(QString::fromStdString(message) + QString::fromLocal8Bit("(code = ") +
                QString::number(code) + QString::fromLocal8Bit(")"));
        break;
    }
    }
}

void login::OnKickOut(ClientType client_type, KickReason kick_reason) {
    return;
}

void login::OnCoreLoginSuccessed() {
    emit LoginInfoSignal(QString::fromLocal8Bit("主服务器登录成功. 正在获取token...！"));
}

void login::OnCoreLoginFailed(int error_code, const std::string& message) {
    OnLoginError(ClientEventHandler::ERROR_UNKONWN, error_code, message);
}

void login::OnFetchTokenSuccessed() {
    
    nim::NimService::GetInstance()->ClientLogin(
      core::CoreService::GetInstance()->GetCurrentUid(), 
      core::CoreService::GetInstance()->GetCurrentNimToken());
}

void login::OnFetchTokenFailed(int error_code, const std::string& message) {
    OnLoginError(ClientEventHandler::ERROR_USER_TOKEN, error_code, message);
}

void login::Login() 
{
    if (ui.idComboBox->currentText().isEmpty() || ui.passLineEdit->text().isEmpty())
        return;

    Config::LoginUserInfo user_info;
    QString id = ui.idComboBox->currentText();
    if (!id.startsWith("pr")) {
        id = "pr" + id;
    }

    user_info.id = id;
    if(ui.rememberCheckBox->isChecked())
        user_info.password = ui.passLineEdit->text();
    user_info.login_time = QDateTime::currentDateTime();
    Config::GetInstance()->AddLoginUserInfo(user_info);

    ui.loginPushButton->setEnabled(false);
    std::string uid = id.toStdString();
    std::string password = ui.passLineEdit->text().toStdString();

    if (nim::NimService::GetInstance()->IsDemo()) {
        QByteArray pass_byte_array;
        pass_byte_array.append(ui.passLineEdit->text());
        std::string pass = QCryptographicHash::hash(
            pass_byte_array, QCryptographicHash::Md5).toHex();
    
        nim::NimService::GetInstance()->ClientLogin(uid, pass);
    }
    else {
        core::CoreService::GetInstance()->AddLoginEventHandler(this);
        core::CoreService::GetInstance()->Login(core::CoreService::Primary, uid, password);

        emit LoginInfoSignal(QString::fromLocal8Bit("正在登录主服务器..."));
    }
}

void login::Logined() {
    accept();
}

void login::Logouted() {

}

void login::LoginInfo(const QString& info) {
    ui.statusLabel->setText(info);
}

void login::LoginError(const QString& info) {
    ui.statusLabel->setText(info);
    ui.loginPushButton->setEnabled(true);
}

void login::OnIdTextEdited(const QString&) {
    ui.rememberCheckBox->setChecked(false);
    ui.passLineEdit->clear();
    ui.statusLabel->setText(QString::fromLocal8Bit("请输入登录信息"));
}

void login::OnPassTextEdited(const QString&) {
    ui.statusLabel->setText(QString::fromLocal8Bit("请输入登录信息"));
}

void login::OnCurrentIndexChanged(const QString &text) {
    Config::LoginUserInfo user_info = Config::GetInstance()->GetLoginUserInfo(text);
    if (!user_info.id.isEmpty() && !user_info.password.isEmpty()) {
        ui.rememberCheckBox->setChecked(true);
        ui.passLineEdit->setText(user_info.password);
    }
    else {
        ui.rememberCheckBox->setChecked(false);
        ui.passLineEdit->clear();
    }
}
