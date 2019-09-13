#pragma once
#include <QProcess>
#include "UpgradeTask.h"
class ProcessStartTask :
	public UpgradeTask
{
	Q_OBJECT
public:
	ProcessStartTask(QString& name, QMap<QString,QString> &context);
	virtual ~ProcessStartTask();

	virtual void run() override;
private:
	QProcess m_processStarter;
public slots:
	void onShowState(QProcess::ProcessState state);
	void onShowError();
	void onShowFinished(int exitCode, QProcess::ExitStatus exitStatus);
};

