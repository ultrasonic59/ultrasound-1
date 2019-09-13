#include <QDebug>
#include <QThread>

#include "ProcessStartTask.h"



ProcessStartTask::ProcessStartTask(QString& name, QMap<QString, QString> &context)
	:UpgradeTask(name,context)
{
	connect(&m_processStarter, SIGNAL(stateChanged(QProcess::ProcessState)),
		this, SLOT(onShowState(QProcess::ProcessState)));
	connect(&m_processStarter, SIGNAL(error(QProcess::ProcessError)),
		this, SLOT(onShowError()));
	connect(&m_processStarter, SIGNAL(finished(int, QProcess::ExitStatus)),
		this, SLOT(onShowFinished(int, QProcess::ExitStatus)));

	setTotal(1);
}


ProcessStartTask::~ProcessStartTask()
{
	qDebug() << "Call ProcessStartTask::~ProcessStartTask(" << getName()<<")";
}

void ProcessStartTask::run()
{
	qDebug() << "Call ProcessStartTask::run";
	emit taskExecuteProgress(this);

	QMap<QString, QString> &ctx = getContext();
	QString program = ctx["app/process/fullname"];
	QStringList arguments = ctx["app/process/arguments"].split(";");
	QString programDetach = ctx["app/program/detach"];
	bool bProgramDetach = QVariant(programDetach).toBool();
	
	if (bProgramDetach){
		m_processStarter.startDetached(program, arguments);
		emit taskExecuteSuccessed(this);
	}
	else {
		m_processStarter.start(program, arguments);
		if (!m_processStarter.waitForStarted()) {
			emit taskExecuteFailed(this);
		}
		else {
			emit taskExecuteSuccessed(this);
		}
	}
	qDebug() << "end call ProcessStartTask::run";
}

void ProcessStartTask::onShowState(QProcess::ProcessState state)
{
}

void ProcessStartTask::onShowError()
{
	//emit taskExecuteFailed(this);
}

void ProcessStartTask::onShowFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
}
