#pragma once

#include <QtWidgets/QMainWindow>
#include <QCloseEvent>
#include "ui_UpgradeMainWindow.h"

#include "WorkThreadTask.h"
#include "CheckVersionTask.h"
#include "PackageDownloadTask.h"
#include "ProcessStartTask.h"
#include "UnzipTask.h"
#include "WorkflowTask.h"
#include "KillProcessTask.h"
#include "CopyFileTask.h"
#include "ProcessStartTask.h"


class UpgradeMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	UpgradeMainWindow(bool bUpgrade,QWidget *parent = Q_NULLPTR);
	~UpgradeMainWindow();

	void setUpgradeDir(QString upgradeDir);

	void closeEvent(QCloseEvent *event);
public slots:
	void onTaskExecuteProgress(UpgradeTask* task);
	void onTaskExecuteSuccessed(UpgradeTask* task);
	void onTaskExecuteFailed(UpgradeTask* task);
private:
	Ui::UpgradeMainWindow ui;

	UpgradeTask * m_currentTask;

	void startUpgradeTasks();
	void startCheckUpgradeTasks();

	QMap<QString, QString> m_context;
	UpgradeTask* checkVersionTask;
	UpgradeTask *packageDownloadTask;
	UpgradeTask *unzipTask;
	UpgradeTask *startUpgradeTask;
	UpgradeTask* startClientTask;

	UpgradeTask *killProcessTask;
	UpgradeTask *copyFileTask;
	UpgradeTask *processStartTask;

};

