#ifndef ULTRALSOUND_UPGRADE_WORKTHREAD_H
#define ULTRALSOUND_UPGRADE_WORKTHREAD_H

#pragma once
#include <QRunnable>
#include <QObject>
#include <QQueue>
#include <QMutex>

#include "UpgradeTask.h"

class WorkThreadTask :
	public UpgradeTask
{
	Q_OBJECT
public:
	WorkThreadTask();
	virtual ~WorkThreadTask();
	
	void enUpgradeTask(UpgradeTask* task);
	
	virtual void run() override;
private:
	QMutex m_tasks_mutex;
	QQueue<UpgradeTask*> m_taskQ;

	UpgradeTask* popUpgradeTask();
};

#endif//ULTRALSOUND_UPGRADE_WORKTHREAD_H
