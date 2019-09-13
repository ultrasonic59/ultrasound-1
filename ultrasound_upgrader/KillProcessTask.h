#pragma once
#include <QRunnable>
#include "UpgradeTask.h"
class KillProcessTask :
	public UpgradeTask
{
	Q_OBJECT
public:
	KillProcessTask(QString& name, QMap<QString, QString>& context);
	virtual ~KillProcessTask();

	virtual void run() override;

};

