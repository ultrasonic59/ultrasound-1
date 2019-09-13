#include "login.h"
#include "ultrasound_consultant.h"
#include <QtWidgets/QApplication>
#include <QStandardPaths>
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>

#include <windows.h>
#include "base/logger.h"
#include "config.h"
#include "core_service/core_service.h"
#include "nim_service/nim_service.h"
#include "oss_service/oss_service.h"

int main(int argc, char *argv[])
{
    HANDLE hMutex = CreateMutex(NULL, TRUE, TEXT("ultrasound_system_designed_by_victor"));
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        return 0;
    }

    QApplication a(argc, argv);
    QTranslator translator;
    bool res = translator.load(":/UltrasoundConsultant/Resources/qtbase_zh_CN");
    a.installTranslator(&translator);
	a.setWindowIcon(QIcon(":/UltrasoundConsultant/ultrasound_consultant.ico"));

    base::Logger::GetInstance()->Initialize(Config::GetInstance());

    LOG_INFO_M("APP", "App start.");

    QString config_file_path = QStandardPaths::writableLocation(
        QStandardPaths::AppLocalDataLocation);
    config_file_path += "/config.json";

    Config::GetInstance()->Load(config_file_path);

    core::CoreService::GetInstance()->Initialize();
	core::CoreService::GetInstance()->SetProxyInfo(
		Config::GetInstance()->GetNetProxyInfo().http.host,
		Config::GetInstance()->GetNetProxyInfo().http.port,
		Config::GetInstance()->GetNetProxyInfo().http.username,
		Config::GetInstance()->GetNetProxyInfo().http.password,
		Config::GetInstance()->GetNetProxyInfo().http.enabled);

    nim::NimService::InitParams params;
    params.is_demo = false;
    params.custom_video = false;
    params.r_host_speaker = false;
    params.video_record = false;
    nim::NimService::GetInstance()->Initialize(Config::GetInstance(), params);

    login l;
    nim::NimService::GetInstance()->AddClientEventHandler(&l);

    if (l.exec() == QDialog::Accepted) {

        oss::OssService::GetInstance()->Initialize(
            core::CoreService::GetInstance()->GetCurrentOssKey(),
            core::CoreService::GetInstance()->GetCurrentOssSecret());

        core::CoreService::GetInstance()->GetCurrentUserInfo();

        UltrasoundConsultant u;
        u.Initialize();

        nim::NimService::GetInstance()->AddClientEventHandler(&u);
        nim::NimService::GetInstance()->AddVChatEventHandler(&u);
		nim::NimService::GetInstance()->AddSysMsgEventHandler(&u);

        core::CoreService::GetInstance()->AddUserEventHandler(&u);
        core::CoreService::GetInstance()->AddReportEventHandler(&u);

        u.showMaximized();

        int ret = a.exec();

        oss::OssService::GetInstance()->Shutdown();

        nim::NimService::GetInstance()->Shutdown();

        core::CoreService::GetInstance()->Shutdown();

        Config::GetInstance()->Save(config_file_path);

        LOG_INFO_M("APP", "App exit.");

        base::Logger::GetInstance()->Shutdown();

        return ret;
    }
	else {
		nim::NimService::GetInstance()->Shutdown();

		core::CoreService::GetInstance()->Shutdown();

		Config::GetInstance()->Save(config_file_path);

		LOG_INFO_M("APP", "App exit.");

		base::Logger::GetInstance()->Shutdown();
	}

    a.exit();
}
