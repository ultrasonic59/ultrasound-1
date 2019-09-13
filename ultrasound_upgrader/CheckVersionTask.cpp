#include <QDebug>
#include <QThread>
#include "CheckVersionTask.h"
#include "core_service/core_service.h"


CheckVersionTask::CheckVersionTask(QString& name, QMap<QString, QString>& context)
	:UpgradeTask(name,context)
{
	core::CoreService::GetInstance()->Initialize();
	setTotal(1);
}


CheckVersionTask::~CheckVersionTask()
{
	core::CoreService::GetInstance()->Shutdown();
	qDebug() << "Call CheckVersionTask::~CheckVersionTask";
}

void CheckVersionTask::run()
{
	qDebug() << "Call CheckVersionTask::run";
	emit taskExecuteProgress(this);

	QMap<QString, QString>& ctx = getContext();
	if (ctx.contains("app/name")){
		QString app = ctx["app/name"];
		core::CoreService::GetInstance()->AddUpgradeEventHandler(this);
		core::CoreService::GetInstance()->CheckUpgrade(app.toStdString());
		QMutexLocker locker(&m_complete_mutex);
		m_waitForComplete.wait(&m_complete_mutex);
	}
	else {
		qDebug() << "app parameter is missing";
		setMessage("app/name is missing at app.in");
		emit taskExecuteFailed(this);
	}
	qDebug() << "End call CheckVersionTask::run";
}

void CheckVersionTask::OnCheckUpgradeSuccessed(core::AppUpgradeInfo upgradeInfo)
{
	QMap<QString, QString>& ctx = getContext();
	if (!ctx.contains("app/version")) {
		setMessage("app/version is missing at app.in");
		emit taskExecuteFailed(this);
	}
	else {
		QString currentVersion = ctx["app/version"];
		QString newVersion = QString::fromStdString(upgradeInfo.GetVersion());
		if (currentVersion.compare(newVersion, Qt::CaseInsensitive) < 0) {
			ctx["app/upgrade/url"] = QString::fromStdString(upgradeInfo.GetUrl());
			ctx["app/upgrade/version"] = QString::fromStdString(upgradeInfo.GetVersion());
			
			emit taskExecuteSuccessed(this);
		}
		else {
			setMessage(QString("cannot upgrade,old version:%1,new version:%2").arg(currentVersion).arg(newVersion));
			setCode(-1);
			emit taskExecuteSuccessed(this);
		}
	}
	
	QMutexLocker locker(&m_complete_mutex);
	m_waitForComplete.wakeAll();
}

void CheckVersionTask::OnCheckUpgradeFailed(int error_code, const std::string& message)
{
	setMessage(QString::fromStdString(message));
	emit taskExecuteFailed(this);

	QMutexLocker locker(&m_complete_mutex);
	m_waitForComplete.wakeAll();
}

