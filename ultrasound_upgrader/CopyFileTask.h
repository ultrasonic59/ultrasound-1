#pragma once
#include <QRunnable>
#include <QDir>

#include "UpgradeTask.h"
class CopyFileTask :
	public UpgradeTask
{

	Q_OBJECT
public:
	CopyFileTask(QString& name, QMap<QString, QString>& context);
	virtual ~CopyFileTask();

	virtual void run() override;

	bool copyDir(const QDir from, QDir to, bool cover = true);
};

