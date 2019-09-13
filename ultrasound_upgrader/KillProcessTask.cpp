#include <QProcess>
#include <QDebug>
#include "KillProcessTask.h"



KillProcessTask::KillProcessTask(QString& name, QMap<QString, QString>& context)
	:UpgradeTask(name,context)
{
	setTotal(1);
}


KillProcessTask::~KillProcessTask()
{
	qDebug() << "Call KillProcessTask::~KillProcessTask";
}

void KillProcessTask::run()
{
	emit taskExecuteProgress(this);

	QMap<QString, QString> &ctx = getContext();
	QStringList processNameList = ctx["app/process/kill"].split(";");
	for (int i = 0; i < processNameList.size(); i++)
	{
		QString processName = processNameList[i] + ".exe";
		QProcess p;
		QString cmdline = QString("taskkill /im %1 /f").arg(processName);
		p.execute(cmdline);
		p.close();
	}
	emit taskExecuteSuccessed(this);
}
