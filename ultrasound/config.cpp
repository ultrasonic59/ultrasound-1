#include "config.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QList>
#include <QStandardPaths>
#include <QDir>
#include "base/logger.h"

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

    QJsonObject video_config = root["video_config"].toObject();
	int name_count = video_config["name"].toString().count();
	if (name_count > 0) {
        name_count += 1;
		wchar_t* name_array = new wchar_t[name_count];
		wmemset(name_array, 0, name_count);
		int ret_count = video_config["name"].toString().toWCharArray(name_array);
		m_video_config.name = name_array;
		delete[] name_array;
	}
	else {
		m_video_config.name.clear();
	}

	int path_count = video_config["path"].toString().count();
	if (path_count > 0) {
        path_count += 1;
		wchar_t* path_array = new wchar_t[path_count];
		wmemset(path_array, 0, path_count);
		int ret_count = video_config["path"].toString().toWCharArray(path_array);
		m_video_config.path = path_array;
		delete[] path_array;
	}
	else {
		m_video_config.path.clear();
	}

	m_video_config.format = (DShow::VideoFormat)(video_config["video_format"].toInt());
	m_video_config.cx = video_config["video_width"].toInt();
	m_video_config.cy = video_config["video_height"].toInt();
	m_video_config.frameInterval = video_config["video_fps"].toInt();

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

    m_department_list = root["department_list"].toString().split(",");
    m_department_list.removeAll("");
    m_submitting_doctor_list = root["m_submitting_doctor_list"].toString().split(",");
    m_submitting_doctor_list.removeAll("");
    
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
    QJsonObject video_config;
	video_config["name"] = QString::fromWCharArray(m_video_config.name.c_str());
	video_config["path"] = QString::fromWCharArray(m_video_config.path.c_str());
	video_config["video_format"] = (int)(m_video_config.format);
	video_config["video_width"] = m_video_config.cx;
	video_config["video_height"] = m_video_config.cy;
	video_config["video_fps"] = m_video_config.frameInterval;

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
    root["video_config"] = video_config;
    root["audio_in_device_info"] = audio_in_device_info;
    root["audio_out_device_info"] = audio_out_device_info;

    root["department_list"] = m_department_list.join(",");
    root["m_submitting_doctor_list"] = m_submitting_doctor_list.join(",");
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

DShow::VideoConfig Config::GetVideoConfig(const std::vector<DShow::VideoDevice>& devices) {
	if (!m_video_config.name.empty() && !m_video_config.path.empty()) {
		for (const DShow::VideoDevice& video_device : devices) {
			if (video_device.name == m_video_config.name &&
				video_device.path == m_video_config.path) {
                m_video_config.useDefaultConfig = false;
				return m_video_config;
			}
		}
	}
	
	const DShow::VideoDevice& device = devices[0];
	m_video_config.name = device.name;
	m_video_config.path = device.path;
	m_video_config.useDefaultConfig = true;

	for (const DShow::VideoInfo& video_info : device.caps) {
		if (video_info.maxCX == PREFERRED_VIDEO_WIDTH && 
			video_info.maxCY == PREFERRED_VIDEO_HEIGHT &&
			(video_info.format == DShow::VideoFormat::I420 || 
			 video_info.format == DShow::VideoFormat::UYVY || 
			 video_info.format == DShow::VideoFormat::XRGB || 
			 video_info.format == DShow::VideoFormat::YUY2)) {

			m_video_config.useDefaultConfig = false;
			m_video_config.cx = video_info.maxCX;
			m_video_config.cy = video_info.maxCY;

			if (m_video_config.format == DShow::VideoFormat::Any) {
				SetVideoFormatAndFPS(video_info);
			}
			else {
				// Prefer not mirrored
				if (m_video_config.format == DShow::VideoFormat::XRGB
					&& video_info.format != DShow::VideoFormat::XRGB) {
					SetVideoFormatAndFPS(video_info);
				}
				
				// Prefer I420
				if (m_video_config.format != DShow::VideoFormat::I420
					&& video_info.format == DShow::VideoFormat::I420) {
					SetVideoFormatAndFPS(video_info);
				}
			}
		}
	}

	return m_video_config;
}

void Config::SetVideoFormatAndFPS(const DShow::VideoInfo& video_info) {
	m_video_config.format = video_info.format;
	if (video_info.minInterval == video_info.maxInterval) {
		m_video_config.frameInterval = video_info.minInterval;
	}
	else {
		if (DEFAULT_FPS >= video_info.minInterval
			&& DEFAULT_FPS <= video_info.maxInterval) {
			m_video_config.frameInterval = DEFAULT_FPS;
		}
		else {
			if (DEFAULT_FPS < video_info.minInterval) {
				m_video_config.frameInterval = video_info.minInterval;
			}
			else {
				m_video_config.frameInterval = video_info.maxInterval;
			}
		}
	}
}

nim::ClientEventHandler::NetProxyInfo Config::GetNetProxyInfo()
{
	return this->m_net_proxy_info;
}

void Config::SetVideoConfig(const DShow::VideoConfig& config) {
	m_video_config = config;
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

void Config::AddDepartment(const QString& department)
{
    if (department.isEmpty() || m_department_list.contains(department))
        return;

    m_department_list.push_front(department);
}

void Config::AddSubmittingDoctor(const QString& doctor)
{
    if (doctor.isEmpty() || m_submitting_doctor_list.contains(doctor))
        return;

    m_submitting_doctor_list.push_front(doctor);
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

QStringList Config::GetDepartmentList()
{
    return m_department_list;
}

QStringList Config::GetSubmittingDoctorList()
{
    return m_submitting_doctor_list;
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

QString Config::GetLogFilePath() {
    QString log_file_path = QStandardPaths::writableLocation(
        QStandardPaths::AppLocalDataLocation);
    log_file_path += "/logs";

    QDir dir = QDir::root();
    dir.mkpath(log_file_path);

    return log_file_path;
}