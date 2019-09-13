#include <QDebug>
#include <QThread>
#include <QThreadPool>
#include <QNetworkAccessManager>
#include <QFileInfo>
#include <QFile>
#include <QNetworkReply>  
#include <QNetworkRequest>
#include <QDir>
#include <QEventLoop>
#include "PackageDownloadTask.h"


PackageDownloadTask::PackageDownloadTask(QString& name, QMap<QString, QString>& context)
	:UpgradeTask(name,context)
	, m_localFile(Q_NULLPTR)
	, m_accessManager(Q_NULLPTR)
	, m_reply(Q_NULLPTR)
{
}


PackageDownloadTask::~PackageDownloadTask()
{
	if (m_reply != Q_NULLPTR) {
		qDebug() << "abort download package ";
		m_reply->abort();
		m_reply = Q_NULLPTR;
	}

	if (m_localFile != Q_NULLPTR) {
		delete m_localFile;
		m_localFile = Q_NULLPTR;
	}
	if (m_accessManager != Q_NULLPTR) {
		delete m_accessManager;
		m_accessManager = Q_NULLPTR;
	}

	qDebug() << "PackageDownloadTask::~PackageDownloadTask";
}

void PackageDownloadTask::run()
{
	QMap<QString, QString> &ctx = getContext();
	QString downloadUrl = ctx["app/download/url"];
	QString localPathFile = ctx["app/download/file"];
	if (localPathFile.isEmpty()||
		downloadUrl.isEmpty() || checkDownloadURLIsValid(downloadUrl)){
		emit taskExecuteFailed(this);
		return;
	}
	//if (checkPackageFileValid(localPathFile)){
	//	emit taskExecuteSuccessed(this);
	//	return;
	//}
	QString upgradeHome = ctx["app/upgrade/home"];
	DelDir(upgradeHome,false);
	QDir upgradeHomeDir(upgradeHome);
	if (!upgradeHomeDir.exists()) {
		upgradeHomeDir.mkpath(upgradeHomeDir.absolutePath());
	}

	downloadHttpFile(downloadUrl, localPathFile);
}


bool PackageDownloadTask::DelDir(const QString &path,bool del)
{
	if (path.isEmpty()) {
		return false;
	}
	QDir dir(path);
	if (!dir.exists()) {
		return true;
	}
	dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot); //设置过滤
	QFileInfoList fileList = dir.entryInfoList(); // 获取所有的文件信息
	foreach(QFileInfo file, fileList) { //遍历文件信息
		if (file.isFile()) { // 是文件，删除
			file.dir().remove(file.fileName());
		}
		else { // 递归删除
			DelDir(file.absoluteFilePath(), true);
		}
	}
	if (del) {
		return dir.rmpath(dir.absolutePath()); // 删除文件夹
	}
	return true;	
}
void PackageDownloadTask::onTaskExecuteSuccessed(UpgradeTask* task)
{
	QMap<QString, QString> &ctx = getContext();
	emit taskExecuteSuccessed(this);

}

void PackageDownloadTask::onTaskExecuteFailed(UpgradeTask* task)
{
	emit taskExecuteFailed(this);
}

void PackageDownloadTask::onReplyFinished(QNetworkReply* reply)
{
	QNetworkReply::NetworkError nwError = reply->error();
	if (reply->error() == QNetworkReply::NoError) {
		m_localFile->flush();
		m_localFile->close();
		
		emit taskExecuteSuccessed(this);
	}
	else {
		emit taskExecuteFailed(this);
	}
	m_localFile->deleteLater();
	m_localFile = Q_NULLPTR;
	m_reply->deleteLater();
	m_reply = Q_NULLPTR;
	m_accessManager->deleteLater();
	m_accessManager = Q_NULLPTR;
	qDebug() << "Download package finished";
}

void PackageDownloadTask::onDownloadProgress(qint64 bytesSent, qint64 bytesTotal)
{
	setTotal(bytesTotal);
	setSent(bytesSent);
	emit taskExecuteProgress(this);
}

void PackageDownloadTask::onReadyRead()
{
	m_localFile->write(m_reply->readAll());
	m_localFile->flush();
}

bool PackageDownloadTask::checkPackageFileValid(QString localPathFile)
{
	QFileInfo fileInfo(localPathFile);
	if (fileInfo.exists()) {
		return true;
	}
	return false;
}

bool PackageDownloadTask::checkDownloadURLIsValid(QString downloadUrl)
{
	return false;
}

bool PackageDownloadTask::downloadHttpFile(QString downloadUrl, QString localPathFile)
{
	QUrl url(downloadUrl);
	QNetworkRequest request;
	m_localFile = new QFile(localPathFile);
	m_localFile->open(QIODevice::WriteOnly);
	m_accessManager = new QNetworkAccessManager();
	request.setUrl(url);
	m_reply = m_accessManager->get(request);
	connect(m_accessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onReplyFinished(QNetworkReply*)));
	connect(m_reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(onDownloadProgress(qint64, qint64)));
	connect(m_reply, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
	QEventLoop eventLoop;
	connect(m_reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
	eventLoop.exec(QEventLoop::ExcludeUserInputEvents);

	qDebug() << "downlaod task exit";
	return true;
}
