#include <QDebug>
#include <QThread>
#include "WorkflowTask.h"



WorkflowTask::WorkflowTask(UpgradeTask* task)
	:m_currentTask(task)
	, m_nextTask(Q_NULLPTR)
{
	this->setAutoDelete(false);
	task->setAutoDelete(false);
	connect(m_currentTask, SIGNAL(taskExecuteSuccessed(UpgradeTask*)), this, SLOT(onTaskExecuteSuccessed(UpgradeTask*)));
	connect(m_currentTask, SIGNAL(taskExecuteFailed(UpgradeTask*)), this, SLOT(onTaskExecuteFailed(UpgradeTask*)));
	connect(m_currentTask, SIGNAL(taskExecuteProgress(UpgradeTask*, qint64, qint64)), this, SLOT(onTaskExecuteProgress(UpgradeTask*, qint64, qint64)));
}

WorkflowTask::~WorkflowTask()
{
	if (m_currentTask != Q_NULLPTR) {
		delete m_currentTask;
		m_currentTask = Q_NULLPTR;
	}
	if (m_nextTask != Q_NULLPTR){
		delete  m_nextTask;
		m_nextTask = Q_NULLPTR;
	}
}

WorkflowTask* WorkflowTask::setNextTask(WorkflowTask* nextTask)
{
	//qDebug() << "Call setNextTask in WorkflowTask" << ",thread:" << QThread::currentThreadId();
	m_nextTask = nextTask;
	nextTask->setAutoDelete(false);
	connect(nextTask, SIGNAL(taskExecuteSuccessed(UpgradeTask*)), this, SLOT(onNextTaskExecuteSuccessed(UpgradeTask*)));
	connect(nextTask, SIGNAL(taskExecuteFailed(UpgradeTask*)), this, SLOT(onNextTaskExecuteFailed(UpgradeTask*)));
	return nextTask;
}

WorkflowTask* WorkflowTask::getNextTask() const
{
	return m_nextTask;
}

bool WorkflowTask::hasNextTask()
{
	if (m_nextTask != Q_NULLPTR)
		return true;
	return false;
}

void WorkflowTask::run()
{
	//qDebug() << "Call run in WorkflowTask" << ",thread:" << QThread::currentThreadId();
	m_currentTask->run();
}

QMap<QString, QString>& WorkflowTask::getContext()
{
	return m_currentTask ->getContext();
}

QString WorkflowTask::getName() const
{
	return m_currentTask->getName();
}

void WorkflowTask::onTaskExecuteSuccessed(UpgradeTask* task)
{
	//qDebug() << "Call onTaskExecuteSuccessed in WorkflowTask" << ",thread:" << QThread::currentThreadId();
	if (hasNextTask())
	{
		emit nextTaskExecute(getNextTask());
		getNextTask()->run();
	}
	else {
		setMessage(task->getMessage());
		emit taskExecuteSuccessed(this);
	}
}

void WorkflowTask::onTaskExecuteFailed(UpgradeTask* task)
{
	//qDebug() << "Call onTaskExecuteSuccessed in WorkflowTask";
	setMessage(task->getMessage());
	emit taskExecuteFailed(this);
}

void WorkflowTask::onNextTaskExecuteSuccessed(UpgradeTask* task)
{
	setMessage(task->getMessage());
	emit taskExecuteSuccessed(this);
}

void WorkflowTask::onNextTaskExecuteFailed(UpgradeTask* task)
{
	setMessage(task->getMessage());
	emit taskExecuteFailed(this);
}
