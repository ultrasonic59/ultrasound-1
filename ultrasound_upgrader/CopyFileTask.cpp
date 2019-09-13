#include <QFileInfoList>
#include <QDebug>
#include "CopyFileTask.h"

CopyFileTask::CopyFileTask(QString& name, QMap<QString, QString>& context)
	:UpgradeTask(name,context)
{
	setTotal(1);
}


CopyFileTask::~CopyFileTask()
{
	qDebug() << "Call CopyFileTask::~CopyFileTask";
}

void CopyFileTask::run()
{
	emit taskExecuteProgress(this);

	QMap<QString, QString> &ctx = getContext();
	QString toDir =ctx["app/copyfile/to"];
	QString fromDir = ctx["app/copyfile/from"];
	if (copyDir(fromDir, toDir))
	{
		emit taskExecuteSuccessed(this);
	}
	else {
		emit taskExecuteFailed(this);
	}
}

bool CopyFileTask::copyDir(const QDir from, QDir to, bool cover) {
	if (!to.exists())
	{
		if (!to.mkdir(to.absolutePath())) {
			qDebug() << "´´½¨Ä¿Â¼Ê§°Ü," << to.absolutePath();
			return false;
		}
	}
	QFileInfoList fileInfoList = from.entryInfoList();
	foreach(QFileInfo fileInfo, fileInfoList)
	{
		if (fileInfo.fileName() == "." || fileInfo.fileName() == "..")
			continue;

		if (fileInfo.isDir()) {
			if (!copyDir(fileInfo.filePath(), to.filePath(fileInfo.fileName())))
				return false;
		}
		else {
			if (cover && to.exists(fileInfo.fileName())) {
				to.remove(fileInfo.fileName());
			}
			if (!QFile::copy(fileInfo.filePath(), to.filePath(fileInfo.fileName()))) {
				return false;
			}
		}
	}
	return true;
}
