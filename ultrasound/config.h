#pragma once

#include <QJsonObject>
#include <QString>
#include <QDateTime>
#include <QMap>

#include "base/macros.h"
#include "base/logger.h"
#include "dshow_input/dshow_input.h"
#include "nim_service/device_event_handler.h"
#include "nim_service/client_event_handler.h"
#include "nim_service/nim_settings.h"
#include "third_party/simplecrypt/simplecrypt.h"
#include "core_service/core_service.h"


class Config : public base::LoggerSettings,
               public nim::NimSettings {
public:
    SINGLETON_DEFINE(Config);

    struct LoginUserInfo {
        QString id;
        QString password;
        QDateTime login_time;

        bool operator==(const LoginUserInfo &other) const
        {
            return id == other.id;
        }
    };

    bool Load(const QString& file_path);
    bool Save(const QString& file_path);

	DShow::VideoConfig GetVideoConfig(const std::vector<DShow::VideoDevice>& devices);
	void SetVideoConfig(const DShow::VideoConfig& config);

    QString GetLogFilePath();

    // LoggerSettings
    base::LOG_LEVEL GetLogLevel() override;
    std::string GetLogFilePathLocal8Bit() override;
    std::string GetLogFilePathUtf8() override;

    // NimSettings
	nim::DeviceEventHandler::DeviceInfo GetAudioInDeviceInfo() override;
	void SetAudioInDeviceInfo(const nim::DeviceEventHandler::DeviceInfo& device_info) override;

	nim::DeviceEventHandler::DeviceInfo GetAudioOutDeviceInfo() override;
	void SetAudioOutDeviceInfo(const nim::DeviceEventHandler::DeviceInfo& device_info) override;

	int GetAudioOutputVolume() override { return m_audio_output_volume; }
	void SetAudioOutputVolume(int volume) override { m_audio_output_volume = volume; }

	int GetAudioInputVolume() override { return m_audio_input_volume; }
	void SetAudioInputVolume(int volume) override { m_audio_input_volume = volume; }

	bool GetAudioInputAutoVolume() override { return m_audio_input_auto_volume; }
	void SetAudioInputAutoVolume(bool auto_volume) override { m_audio_input_auto_volume = auto_volume; }

    void AddDepartment(const QString& department);
    void AddSubmittingDoctor(const QString& doctor);
    void AddLoginUserInfo(const LoginUserInfo& user_info);

    QStringList GetDepartmentList();
    QStringList GetSubmittingDoctorList();
    QStringList GetLoginUserIdList();
    LoginUserInfo GetLoginUserInfo(const QString& id);
    void UpdateUserPassword(const QString& id, const QString& password);
	// Í¨¹ý NimSettings ¼Ì³Ð
	virtual nim::ClientEventHandler::NetProxyInfo GetNetProxyInfo() override;
private:
    Config();
    ~Config() = default;

	void SetVideoFormatAndFPS(const DShow::VideoInfo& video_info);

	DShow::VideoConfig m_video_config;
	nim::DeviceEventHandler::DeviceInfo m_audio_in_device_info;
	nim::DeviceEventHandler::DeviceInfo m_audio_out_device_info;
	nim::ClientEventHandler::NetProxyInfo m_net_proxy_info;

	int m_audio_output_volume = 255;
	int m_audio_input_volume = 255;
	bool m_audio_input_auto_volume = false;

    QStringList m_department_list;
    QStringList m_submitting_doctor_list;
    QList<LoginUserInfo> m_login_user_info_list;
    base::LOG_LEVEL m_log_level = base::LOG_LEVEL_INFO;

    SimpleCrypt m_crypt;

	const int PREFERRED_VIDEO_WIDTH = 720;
	const int PREFERRED_VIDEO_HEIGHT = 576;
	const long long DEFAULT_FPS = 333333;

    const quint64 KEY = 0xa7bbccbc3b9ae514;


};
