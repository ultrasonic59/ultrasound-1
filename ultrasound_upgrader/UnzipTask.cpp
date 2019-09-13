#include <QDebug>
#include "UnzipTask.h"
#include "JlCompress.h"


UnzipTask::UnzipTask(QString& name, QMap<QString, QString>& context)
	:UpgradeTask(name, context)
{
	setTotal(1);
}


UnzipTask::~UnzipTask()
{
	qDebug() << "UnzipTask::~UnzipTask";
}

void UnzipTask::run()
{
	emit taskExecuteProgress(this);

	try
	{
		QMap<QString, QString> &ctx = getContext();
		QString uzipedFile = ctx["app/unziped/file"];
		QString uzipedDir = ctx["app/unziped/dir"];
		QStringList retList = JlCompress::extractDir(uzipedFile, uzipedDir);
		if (retList.size() > 0) {
			QFileInfo fileInfo(uzipedFile);
			QString fileName = fileInfo.baseName();
			ctx["app/unziped/dir"] = uzipedDir + QDir::separator() + fileName;
			emit taskExecuteSuccessed(this);
		}
		else {
			setMessage(QString::fromLocal8Bit("½âÑ¹Éý¼¶°üÊ§°Ü"));
			emit taskExecuteFailed(this);
		}
	}
	catch (const std::exception& ex)
	{
		setMessage(QString::fromStdString(ex.what()));
		emit taskExecuteFailed(this);
	}
	
	
}
