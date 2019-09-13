#ifndef ULTRASOUND_UPGRADE_STATUS_H
#define ULTRASOUND_UPGRADE_STATUS_H

#pragma once
#include <QRunnable>
#include <QWaitCondition>
#include <QMutex>

#include "UpgradeTask.h"
#include "core_service/upgrade_event_handler.h"

class CheckVersionTask :
	public UpgradeTask, public core::UpgradeEventHandler
{
	Q_OBJECT
public:
	CheckVersionTask(QString& name,QMap<QString, QString>& context);
	virtual ~CheckVersionTask();

	virtual void run() override;

	virtual void OnCheckUpgradeSuccessed(core::AppUpgradeInfo upgradeInfo) override;
	virtual void OnCheckUpgradeFailed(int error_code, const std::string& message) override;
private:
	QWaitCondition m_waitForComplete;
	QMutex m_complete_mutex;

};

#endif//ULTRASOUND_UPGRADE_STATUS_H
