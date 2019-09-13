#ifndef ULTRASOUND_UPGRADE_EXECUTOR_H
#define ULTRASOUND_UPGRADE_EXECUTOR_H

#pragma once
#include <QRunnable>
#include "UpgradeTask.h"
class UnzipTask :
	public UpgradeTask
{
	Q_OBJECT
public:
	UnzipTask(QString& name, QMap<QString, QString>& context);
	virtual ~UnzipTask();

	virtual void run() override;

};

#endif//ULTRASOUND_UPGRADE_EXECUTOR_H
