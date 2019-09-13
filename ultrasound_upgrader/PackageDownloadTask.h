#ifndef ULTRASOUND_UPGRADE_PACKAGE_DOWNLOADER_H
#define ULTRASOUND_UPGRADE_PACKAGE_DOWNLOADER_H

#pragma once
#include <QRunnable>
#include <QFile>
#include <QNetworkReply>  
#include <QNetworkRequest>
#include <QWaitCondition>
#include <QMutex>
#include "UpgradeTask.h"

class PackageDownloadTask :
	public UpgradeTask
{
	Q_OBJECT
public:
	PackageDownloadTask(QString& name, QMap<QString, QString>& context);
	virtual ~PackageDownloadTask();

	virtual void run() override;
public slots:
	void onTaskExecuteSuccessed(UpgradeTask* task);
	void onTaskExecuteFailed(UpgradeTask* task);

	void onReplyFinished(QNetworkReply* reply);
	void onDownloadProgress(qint64 bytesSent, qint64 bytesTotal);
	void onReadyRead();

private:
	bool checkPackageFileValid(QString localPathFile);
	bool checkDownloadURLIsValid(QString m_downloadUrl);
	bool downloadHttpFile(QString downloadUrl, QString localPathFile);
	bool DelDir(const QString &path, bool del);
	
	QFile * m_localFile;
	QNetworkAccessManager *m_accessManager;
	QNetworkReply *m_reply;
};
#endif//ULTRASOUND_UPGRADE_PACKAGE_DOWNLOADER_H
