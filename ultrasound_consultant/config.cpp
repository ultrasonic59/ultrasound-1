#include "config.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QList>
#include <QStandardPaths>
#include <QDir>

Config::Config() {
    m_crypt.setKey(KEY);
}


bool Config::Load(const QString& file_path) {
    QFile loadFile(file_path);
    if (!loadFile.open(QIODevice::ReadOnly)) {
        return false;
    }

    QByteArray saveData = loadFile.readAll();
    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
    QJsonObject root = loadDoc.object();

    QJsonObject audio_in_device_info = root["audio_in_device_info"].toObject();
	m_audio_in_device_info.friendly_name = audio_in_device_info["name"].toString().toStdString();
	m_audio_in_device_info.device_path = audio_in_device_info["path"].toString().toStdString();
	m_audio_in_device_info.type = (nim::DeviceEventHandler::DeviceType)(audio_in_device_info["type"].toInt());
	m_audio_input_volume = audio_in_device_info["volume"].toInt();
	m_audio_input_auto_volume = audio_in_device_info["auto"].toBool();

    QJsonObject audio_out_device_info = root["audio_out_device_info"].toObject();
	m_audio_out_device_info.friendly_name = audio_out_device_info["name"].toString().toStdString();
	m_audio_out_device_info.device_path = audio_out_device_info["path"].toString().toStdString();
	m_audio_out_device_info.type = (nim::DeviceEventHandler::DeviceType)(audio_out_device_info["type"].toInt());
	m_audio_output_volume = audio_out_device_info["volume"].toInt();
    
	QJsonObject net_proxy_info = root["net_proxy_info"].toObject();
	QJsonObject nim_net_proxy_info = net_proxy_info["nim"].toObject();
	m_net_proxy_info.nim.enabled = nim_net_proxy_info["enabled"].toBool();
	m_net_proxy_info.nim.host = nim_net_proxy_info["host"].toString().toStdString();
	m_net_proxy_info.nim.port = nim_net_proxy_info["port"].toInt();
	m_net_proxy_info.nim.username = nim_net_proxy_info["username"].toString().toStdString();
	m_net_proxy_info.nim.password = nim_net_proxy_info["password"].toString().toStdString();

	QJsonObject http_net_proxy_info = net_proxy_info["http"].toObject();
	m_net_proxy_info.http.enabled = http_net_proxy_info["enabled"].toBool();
	m_net_proxy_info.http.host = http_net_proxy_info["host"].toString().toStdString();
	m_net_proxy_info.http.port = http_net_proxy_info["port"].toInt();
	m_net_proxy_info.http.username = http_net_proxy_info["username"].toString().toStdString();
	m_net_proxy_info.http.password = http_net_proxy_info["password"].toString().toStdString();

    if (root.contains("log_level")) {
        m_log_level = (base::LOG_LEVEL)root["log_level"].toInt();
    }

    QJsonArray login_user_info_list = root["login_user_info_list"].toArray();
    for (int i = 0; i < login_user_info_list.count(); i++) 
    {
        QJsonObject json_user_info = login_user_info_list[i].toObject();
        LoginUserInfo user_info;
        user_info.id = json_user_info["id"].toString();

        QString password_encrypt = json_user_info["password"].toString();
        user_info.password = m_crypt.decryptToString(password_encrypt);
        
        user_info.login_time = QDateTime::fromString(json_user_info["login_time"].toString(), "yyyy-MM-dd hh:mm:ss");
        if (!user_info.id.isEmpty() && !m_login_user_info_list.contains(user_info))
            m_login_user_info_list.push_back(user_info);
    }

    qSort(m_login_user_info_list.begin(), m_login_user_info_list.end(),
        [](const LoginUserInfo& user_info1, const LoginUserInfo& user_info2) { return user_info1.login_time > user_info2.login_time; });

    return true;
}

bool Config::Save(const QString& file_path) {
    QJsonObject audio_in_device_info;
    audio_in_device_info["name"] = QString::fromStdString(m_audio_in_device_info.friendly_name);
	audio_in_device_info["path"] = QString::fromStdString(m_audio_in_device_info.device_path);
	audio_in_device_info["type"] = (int)m_audio_in_device_info.type;
	audio_in_device_info["volume"] = m_audio_input_volume;
	audio_in_device_info["auto"] = m_audio_input_auto_volume;

    QJsonObject audio_out_device_info;
    audio_out_device_info["name"] = QString::fromStdString(m_audio_out_device_info.friendly_name);
    audio_out_device_info["path"] = QString::fromStdString(m_audio_out_device_info.device_path);
    audio_out_device_info["type"] = (int)m_audio_out_device_info.type;
    audio_out_device_info["volume"] = m_audio_output_volume;

	QJsonObject nim_net_proxy_info;
	nim_net_proxy_info["enabled"] = m_net_proxy_info.nim.enabled;
	nim_net_proxy_info["host"] = QString::fromStdString(m_net_proxy_info.nim.host);
	nim_net_proxy_info["port"] = m_net_proxy_info.nim.port;
	nim_net_proxy_info["username"] = QString::fromStdString(m_net_proxy_info.nim.username);
	nim_net_proxy_info["password"] = QString::fromStdString(m_net_proxy_info.nim.password);

	QJsonObject http_net_proxy_info;
	http_net_proxy_info["enabled"] = m_net_proxy_info.http.enabled;
	http_net_proxy_info["host"] = QString::fromStdString(m_net_proxy_info.http.host);
	http_net_proxy_info["port"] = m_net_proxy_info.http.port;
	http_net_proxy_info["username"] = QString::fromStdString(m_net_proxy_info.http.username);
	http_net_proxy_info["password"] = QString::fromStdString(m_net_proxy_info.http.password);

	QJsonObject net_proxy_info;
	net_proxy_info["nim"] = nim_net_proxy_info;
	net_proxy_info["http"] = http_net_proxy_info;

    QJsonArray login_user_info_list;
    for (LoginUserInfo user_info : m_login_user_info_list) {
        QJsonObject json_user_info;
        json_user_info["id"] = user_info.id;

        QString password_encrypt = m_crypt.encryptToString(user_info.password);
        json_user_info["password"] = password_encrypt;

        json_user_info["login_time"] = user_info.login_time.toString("yyyy-MM-dd hh:mm:ss");
        login_user_info_list.append(json_user_info);
    }

    QJsonObject root;
    root["audio_in_device_info"] = audio_in_device_info;
    root["audio_out_device_info"] = audio_out_device_info;

    root["login_user_info_list"] = login_user_info_list;
	root["net_proxy_info"] = net_proxy_info;

    QFile saveFile(file_path);

    if (!saveFile.open(QIODevice::WriteOnly)) {
        return false;
    }

    QJsonDocument saveDoc(root);
    saveFile.write(saveDoc.toJson());

    return true;
}

base::LOG_LEVEL Config::GetLogLevel() {
    return m_log_level;
}

std::string Config::GetLogFilePathLocal8Bit() {
    return GetLogFilePath().toLocal8Bit().constData();
}

std::string Config::GetLogFilePathUtf8() {
    return GetLogFilePath().toUtf8().constData();
}

nim::DeviceEventHandler::DeviceInfo Config::GetAudioInDeviceInfo() {
	return m_audio_in_device_info; 
}

void Config::SetAudioInDeviceInfo(const nim::DeviceEventHandler::DeviceInfo& device_info) {
	m_audio_in_device_info = device_info;
}

nim::DeviceEventHandler::DeviceInfo Config::GetAudioOutDeviceInfo() { 
	return m_audio_out_device_info; 
}

void Config::SetAudioOutDeviceInfo(const nim::DeviceEventHandler::DeviceInfo& device_info) {
	m_audio_out_device_info = device_info;
}

void Config::AddLoginUserInfo(const LoginUserInfo& user_info)
{
    if (user_info.id.isEmpty())
        return;

    if (m_login_user_info_list.contains(user_info)) {
        m_login_user_info_list.removeAll(user_info);
    }

    m_login_user_info_list.push_back(user_info);
}

QStringList Config::GetLoginUserIdList()
{
    QStringList string_list;
    for (LoginUserInfo user_info : m_login_user_info_list) 
        string_list.append(user_info.id);

    return string_list;
}

Config::LoginUserInfo Config::GetLoginUserInfo(const QString& id)
{
    for (LoginUserInfo user_info : m_login_user_info_list) {
        if (user_info.id == id)
            return user_info;
    }

    return LoginUserInfo();
}

void Config::UpdateUserPassword(const QString& id, const QString& password) {
    for (LoginUserInfo& user_info : m_login_user_info_list) {
        if (user_info.id == id) {
            user_info.password = password;
            return;
        }
    }
}

nim::ClientEventHandler::NetProxyInfo Config::GetNetProxyInfo()
{
	return this->m_net_proxy_info;
}

QString Config::GetLogFilePath() {
    QString log_file_path = QStandardPaths::writableLocation(
        QStandardPaths::AppLocalDataLocation);
    log_file_path += "/logs";

    QDir dir = QDir::root();
    dir.mkpath(log_file_path);

    return log_file_path;
}