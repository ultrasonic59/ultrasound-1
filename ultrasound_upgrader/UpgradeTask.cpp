#include <QDebug>
#include "UpgradeTask.h"

UpgradeTask::UpgradeTask(QString &name, QMap<QString, QString>& context /*= QMap<QString, QString>()*/)
	:m_context(context)
	,m_name(name)
	, m_bStop(false)
{
	setAutoDelete(false);
	setSent(0);
	setTotal(0);
	setCode(0);
}

UpgradeTask::~UpgradeTask()
{
	qDebug() << "Call UpgradeTask::~UpgradeTask";
}

void UpgradeTask::stop()
{
	m_bStop = true;
}

bool UpgradeTask::isStop()
{
	return m_bStop;
}

void UpgradeTask::onTaskExecuteProgress(UpgradeTask* task)
{
	emit taskExecuteProgress(task);
}
