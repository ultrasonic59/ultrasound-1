#include <QThread>
#include <QDebug>

#include "WorkThreadTask.h"

WorkThreadTask::WorkThreadTask()
	:m_tasks_mutex(QMutex::Recursive)
{
}


WorkThreadTask::~WorkThreadTask()
{
	
}


void WorkThreadTask::enUpgradeTask(UpgradeTask* task)
{
	QMutexLocker locker(&m_tasks_mutex);
	m_taskQ.enqueue(task);
}

void WorkThreadTask::run()
{
	qDebug() << "execute WorkThreadTask ,ThreadId:" << QThread::currentThreadId();
	while (!isStop())
	{
		UpgradeTask* task = popUpgradeTask();
		if (task == Q_NULLPTR) {
			QThread::msleep(200);
			continue;
		}
		task->run();
	}

	// execute completed.
	emit taskExecuteSuccessed(this);
}

UpgradeTask* WorkThreadTask::popUpgradeTask()
{
	QMutexLocker locker(&m_tasks_mutex);
	if (!m_taskQ.isEmpty())
	{
		return m_taskQ.dequeue();
	}
	return Q_NULLPTR;
}
