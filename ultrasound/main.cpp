#include "login.h"
#include "ultrasound.h"
#include <QtWidgets/QApplication>
#include <QStandardPaths>
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>

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
    bool res = translator.load(":/ultrasound/res/qtbase_zh_CN");
    a.installTranslator(&translator);
	a.setWindowIcon(QIcon(":/ultrasound/ultrasound.ico"));

    bool is_demo = false;
    if (QCoreApplication::arguments().count() > 1) {
        QString demo = QCoreApplication::arguments().at(1);
        if (demo.contains("demo", Qt::CaseInsensitive))
            is_demo = true;
    }

    base::Logger::GetInstance()->Initialize(Config::GetInstance());

    LOG_INFO_M("APP", "App start, is_demo=%d.", is_demo);

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

	QString sdk_log_file_path = QStandardPaths::writableLocation(
		QStandardPaths::AppLocalDataLocation);
	sdk_log_file_path += "/sdklogs";
    nim::NimService::InitParams params;
    params.is_demo = is_demo;
    params.custom_video = true;
    params.r_host_speaker = true;
    params.video_record = true;
	params.log_path = sdk_log_file_path.toStdString();
    nim::NimService::GetInstance()->Initialize(Config::GetInstance(), params);

    login l;
    nim::NimService::GetInstance()->AddClientEventHandler(&l);

    if (l.exec() == QDialog::Accepted) {

        oss::OssService::GetInstance()->Initialize(
          core::CoreService::GetInstance()->GetCurrentOssKey(),
          core::CoreService::GetInstance()->GetCurrentOssSecret());

        core::CoreService::GetInstance()->GetCurrentUserInfo();

        ultrasound u;
        u.Initialize();

        nim::NimService::GetInstance()->AddClientEventHandler(&u);
        nim::NimService::GetInstance()->AddVChatEventHandler(&u);
		nim::NimService::GetInstance()->AddSysMsgEventHandler(&u);

        core::CoreService::GetInstance()->AddUserEventHandler(&u);
        core::CoreService::GetInstance()->AddReportEventHandler(&u);

        u.showMaximized();

        int ret = a.exec();

        // Save config
        DShow::DShowInput* dshow = u.GetDShowInput();
        if (dshow) {
            Config::GetInstance()->SetVideoConfig(dshow->GetCurrentVideoConfig());
        }

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
