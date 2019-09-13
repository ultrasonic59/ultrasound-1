#include <QDebug>
#include <QThreadPool>
#include <QDebug>
#include <QSettings>
#include <QDir>
#include <iostream>
#include <QMessageBox>
#include <QtCore/QCoreApplication>
#include "UpgradeMainWindow.h"



UpgradeMainWindow::UpgradeMainWindow(bool bUpgrade, QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	setWindowFlags(Qt::WindowStaysOnTopHint);
	setWindowFlags(windowFlags() &~Qt::WindowCloseButtonHint);
	checkVersionTask = Q_NULLPTR;
	packageDownloadTask = Q_NULLPTR;
	unzipTask = Q_NULLPTR;
	startUpgradeTask = Q_NULLPTR;
	startClientTask = Q_NULLPTR;

	killProcessTask = Q_NULLPTR;
	copyFileTask = Q_NULLPTR;

	qDebug() << "bUpgrade:" << bUpgrade;

	QString settingFileStr = QCoreApplication::applicationDirPath() + QDir::separator() + "app.ini";
	QSettings  settingFile(settingFileStr, QSettings::IniFormat);

	m_context["app/home"] = QCoreApplication::applicationDirPath();
	m_context["app/name"] = settingFile.value("app/name").toString();//"pri_pc";
	m_context["app/binary"] = settingFile.value("app/binary").toString();
	m_context["app/version"] = settingFile.value("app/version").toString(); //"0.0.1";
	QString upgradeHome = QCoreApplication::applicationDirPath() + QDir::separator() + "temp";
	QDir upgradeHomeDir(upgradeHome);
	if (!upgradeHomeDir.exists()) {
		upgradeHomeDir.mkpath(upgradeHome);
	}
	m_context["app/upgrade/home"] = upgradeHome;
	m_context["app/process/kill"] = settingFile.value("app/binary").toString();
	m_context["app/copyfile/from"] = QCoreApplication::applicationDirPath();

	ui.progressBar->setRange(0, 0);
	if (bUpgrade){
		startUpgradeTasks();
	}
	else {
		startCheckUpgradeTasks();
	}
}

UpgradeMainWindow::~UpgradeMainWindow()
{
	qDebug() << "begin  call UpgradeMainWindow::~UpgradeMainWindow";

	if (Q_NULLPTR != checkVersionTask) {
		delete checkVersionTask;
		checkVersionTask = Q_NULLPTR;
	}

	if (Q_NULLPTR != packageDownloadTask) {
		delete packageDownloadTask;
		packageDownloadTask = Q_NULLPTR;
	}

	if (Q_NULLPTR != unzipTask) {
		delete unzipTask;
		unzipTask = Q_NULLPTR;
	}

	if (Q_NULLPTR != startUpgradeTask) {
		delete startUpgradeTask;
		startUpgradeTask = Q_NULLPTR;
	}
	if (Q_NULLPTR != startClientTask) {
		delete startClientTask;
		startClientTask = Q_NULLPTR;
	}

	if (Q_NULLPTR != killProcessTask) {
	   delete killProcessTask;
		killProcessTask = Q_NULLPTR;
	}
	if (Q_NULLPTR != copyFileTask) {
		delete copyFileTask;
		copyFileTask = Q_NULLPTR;
	}
	qDebug() << " wait for thread pool exit";
	QThreadPool::globalInstance()->waitForDone();
	qDebug() << "end call UpgradeMainWindow::~UpgradeMainWindow";
}

void UpgradeMainWindow::setUpgradeDir(QString upgradeDir)
{
	m_context["app/copyfile/to"] = upgradeDir;
}

void UpgradeMainWindow::closeEvent(QCloseEvent * event)
{
	event->ignore();
}

void UpgradeMainWindow::onTaskExecuteProgress(UpgradeTask* task)
{
	if (m_currentTask == Q_NULLPTR || m_currentTask != task) {
		m_currentTask = task;
		QString message = QString::fromLocal8Bit("%1-->正在执行").arg(task->getName());
		ui.label->setText(message);
		ui.progressBar->setRange(0, task->getTotal());
	}

	ui.progressBar->setValue(task->getSent());
}

void UpgradeMainWindow::onTaskExecuteSuccessed(UpgradeTask* task)
{
	if (task == checkVersionTask) {
		QMap<QString, QString> &currentContext = task->getContext();
		QMap<QString, QString> &nextContext = packageDownloadTask->getContext();

		if (task->getCode() != 0) {
			nextContext["app/process/fullname"] = m_context["app/home"] + QDir::separator() + currentContext["app/binary"] + ".exe";
			nextContext["app/program/detach"] = "true";
			QThreadPool::globalInstance()->start(startClientTask);
		}
		else {
			QString upgradeUrl = currentContext["app/upgrade/url"];
			QString upgradeHome = currentContext["app/upgrade/home"];

			QFileInfo fileinfo(upgradeUrl);
			nextContext["app/download/url"] = upgradeUrl;
			nextContext["app/download/file"] = upgradeHome + QDir::separator() + fileinfo.fileName();
			QThreadPool::globalInstance()->start(packageDownloadTask);
		}
		
		//mainWorkThreadTask->enUpgradeTask(packageDownloadTask);
	}
	else if (task == packageDownloadTask) {
		QMap<QString, QString> &currentContext = task->getContext();
		QMap<QString, QString> &nextContext = unzipTask->getContext();

		nextContext["app/unziped/file"] = currentContext["app/download/file"];
		nextContext["app/unziped/dir"] = currentContext["app/upgrade/home"];

		//mainWorkThreadTask->enUpgradeTask(unzipTask);
		QThreadPool::globalInstance()->start(unzipTask);
	}
	else if (task == unzipTask) {
		QMap<QString, QString> &currentContext = task->getContext();
		QMap<QString, QString> &nextContext = startUpgradeTask->getContext();

		QString binaryDir = currentContext["app/unziped/dir"];
		nextContext["app/process/fullname"] = binaryDir + QDir::separator()+ QCoreApplication::applicationName() + ".exe";
		nextContext["app/process/arguments"] = QString("--upgrade;%1").arg(currentContext["app/home"]);
		nextContext["app/program/detach"] = "true";
		//mainWorkThreadTask->enUpgradeTask(startUpgradeTask);
		QThreadPool::globalInstance()->start(startUpgradeTask);
	}
	if (killProcessTask == task) {
		QThreadPool::globalInstance()->start(copyFileTask);
	}
	else if (copyFileTask == task) {
		QMap<QString, QString> &currentContext = task->getContext();
		QMap<QString, QString> &nextContext = copyFileTask->getContext();

		QString copyto = currentContext["app/copyfile/to"];

		nextContext["app/process/fullname"] = copyto + QDir::separator() + currentContext["app/binary"] + ".exe";
		nextContext["app/program/detach"] = "true";

		QThreadPool::globalInstance()->start(startClientTask);
	}
	else if (task == startUpgradeTask) {
		// exit application
		QThread::msleep(500);
		QCoreApplication::exit(0);
	}
	else if (task == startClientTask) {
		// exit application
		QThread::msleep(500);
		QCoreApplication::exit(0);
	}

	QString message = QString::fromLocal8Bit("%1-->执行完成").arg(task->getName());
	ui.label->setText(message);
	ui.progressBar->setValue(task->getTotal());
}

void UpgradeMainWindow::onTaskExecuteFailed(UpgradeTask* task)
{
	QMap<QString, QString> &context = startClientTask->getContext();

	QString appHome = context["app/home"];
	context["app/process/fullname"] = appHome + QDir::separator() + context["app/binary"] + ".exe";
	context["app/program/detach"] = "true";

	QString message = QString::fromLocal8Bit("%1-->执行失败").arg(task->getName());
	ui.label->setText(message);

	QMessageBox messagebox(QMessageBox::NoIcon, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("升级失败"));
	messagebox.setIconPixmap(QPixmap("messagebox.png"));
	messagebox.exec();

	//mainWorkThreadTask->enUpgradeTask(startClientTask);
	QThreadPool::globalInstance()->start(startClientTask);
}

void UpgradeMainWindow::startUpgradeTasks()
{
	killProcessTask = new KillProcessTask(QString::fromLocal8Bit("终止运行终端"), m_context);
	copyFileTask = new CopyFileTask(QString::fromLocal8Bit("升级程序文件"), m_context);
	startClientTask = new ProcessStartTask(QString::fromLocal8Bit("启动终端程序"), m_context);

	connect(killProcessTask, &UpgradeTask::taskExecuteSuccessed, this, &UpgradeMainWindow::onTaskExecuteSuccessed);
	connect(killProcessTask, &UpgradeTask::taskExecuteFailed, this, &UpgradeMainWindow::onTaskExecuteFailed);
	connect(killProcessTask, &UpgradeTask::taskExecuteProgress, this, &UpgradeMainWindow::onTaskExecuteProgress);

	connect(copyFileTask, &UpgradeTask::taskExecuteSuccessed, this, &UpgradeMainWindow::onTaskExecuteSuccessed);
	connect(copyFileTask, &UpgradeTask::taskExecuteFailed, this, &UpgradeMainWindow::onTaskExecuteFailed);
	connect(copyFileTask, &UpgradeTask::taskExecuteProgress, this, &UpgradeMainWindow::onTaskExecuteProgress);

	connect(startClientTask, &UpgradeTask::taskExecuteSuccessed, this, &UpgradeMainWindow::onTaskExecuteSuccessed);
	connect(startClientTask, &UpgradeTask::taskExecuteFailed, this, &UpgradeMainWindow::onTaskExecuteFailed);
	connect(startClientTask, &UpgradeTask::taskExecuteProgress, this, &UpgradeMainWindow::onTaskExecuteProgress);

	QThread::msleep(1000);
	QThreadPool::globalInstance()->start(killProcessTask);
}

void UpgradeMainWindow::startCheckUpgradeTasks()
{
	qDebug() << "UpgradeMainWindow::startCheckUpgradeTasks ,ThreadId:" << QThread::currentThreadId();

	checkVersionTask = new CheckVersionTask(QString::fromLocal8Bit("检查可升级版本"), m_context);
	packageDownloadTask = new PackageDownloadTask(QString::fromLocal8Bit("开始下载升级包"), m_context);
	unzipTask = new UnzipTask(QString::fromLocal8Bit("开始解压升级包"), m_context);
	startUpgradeTask = new ProcessStartTask(QString::fromLocal8Bit("开始启动升级程序"), m_context);
	startClientTask = new ProcessStartTask(QString::fromLocal8Bit("启动客户端程序"), m_context);


	connect(checkVersionTask, &UpgradeTask::taskExecuteSuccessed, this, &UpgradeMainWindow::onTaskExecuteSuccessed);
	connect(checkVersionTask, &UpgradeTask::taskExecuteFailed, this, &UpgradeMainWindow::onTaskExecuteFailed);
	connect(checkVersionTask, &UpgradeTask::taskExecuteProgress, this, &UpgradeMainWindow::onTaskExecuteProgress);

	connect(packageDownloadTask, &UpgradeTask::taskExecuteSuccessed, this, &UpgradeMainWindow::onTaskExecuteSuccessed);
	connect(packageDownloadTask, &UpgradeTask::taskExecuteFailed, this, &UpgradeMainWindow::onTaskExecuteFailed);
	connect(packageDownloadTask, &UpgradeTask::taskExecuteProgress, this, &UpgradeMainWindow::onTaskExecuteProgress);

	connect(unzipTask, &UpgradeTask::taskExecuteSuccessed, this, &UpgradeMainWindow::onTaskExecuteSuccessed);
	connect(unzipTask, &UpgradeTask::taskExecuteFailed, this, &UpgradeMainWindow::onTaskExecuteFailed);
	connect(unzipTask, &UpgradeTask::taskExecuteProgress, this, &UpgradeMainWindow::onTaskExecuteProgress);

	connect(startUpgradeTask, &UpgradeTask::taskExecuteSuccessed, this, &UpgradeMainWindow::onTaskExecuteSuccessed);
	connect(startUpgradeTask, &UpgradeTask::taskExecuteFailed, this, &UpgradeMainWindow::onTaskExecuteFailed);
	connect(startUpgradeTask, &UpgradeTask::taskExecuteProgress, this, &UpgradeMainWindow::onTaskExecuteProgress);

	connect(startClientTask, &UpgradeTask::taskExecuteSuccessed, this, &UpgradeMainWindow::onTaskExecuteSuccessed);
	connect(startClientTask, &UpgradeTask::taskExecuteFailed, this, &UpgradeMainWindow::onTaskExecuteFailed);
	connect(startClientTask, &UpgradeTask::taskExecuteProgress, this, &UpgradeMainWindow::onTaskExecuteProgress);


	//mainWorkThreadTask->enUpgradeTask(checkVersionTask);
	QThreadPool::globalInstance()->start(checkVersionTask);
}
