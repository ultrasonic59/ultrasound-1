#pragma once
#include "qrunnable.h"
#include "upgradetask.h"
class WorkflowTask :
	public UpgradeTask
{
	Q_OBJECT
public:
	WorkflowTask(UpgradeTask* task);
	virtual ~WorkflowTask();

	WorkflowTask* setNextTask(WorkflowTask* nextTask);
	WorkflowTask* getNextTask()const;
	bool hasNextTask();

	virtual void run() override;

	virtual QMap<QString, QString>& getContext() override;


	virtual QString getName() const override;

	public Q_SLOTS:
	void onTaskExecuteSuccessed(UpgradeTask* task);
	void onTaskExecuteFailed(UpgradeTask* task);
	void onNextTaskExecuteSuccessed(UpgradeTask* task);
	void onNextTaskExecuteFailed(UpgradeTask* task);
Q_SIGNALS:
	void nextTaskExecute(UpgradeTask* task);
private:
	UpgradeTask * m_currentTask;
	WorkflowTask* m_nextTask;
};

