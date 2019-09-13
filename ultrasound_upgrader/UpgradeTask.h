#pragma once

#include <QMap>
#include <QObject>
#include <QRunnable>

class UpgradeTask :
	public QObject, public QRunnable
{
	Q_OBJECT
public:
	UpgradeTask(QString &name = QString(), QMap<QString, QString>& context = QMap<QString, QString>());
	virtual ~UpgradeTask();

	int getCode() const { return m_code; }
	UpgradeTask& setCode(int val) { m_code = val; return *this; }

	QString getMessage() const { return m_message; }
	UpgradeTask& setMessage(QString val) { m_message = val; return *this; }

	void setResult(int code, QString message) { m_code = code; m_message = message; }

	virtual QMap<QString, QString>& getContext() { return m_context; }
	
	virtual qint64 getSent() const { return m_sent; }
	virtual void setSent(qint64 val) { m_sent = val; }

	virtual qint64 getTotal() const { return m_total; }
	virtual void setTotal(qint64 val) { m_total = val; }

	virtual QString getName() const { return m_name; }
	virtual void setName(QString val) { m_name = val; }

	virtual void stop();
	virtual bool isStop();
Q_SIGNALS:
	void taskExecuteStarting(UpgradeTask* task);
	void taskExecuteProgress(UpgradeTask* task);
	void taskExecuteSuccessed(UpgradeTask* task);
	void taskExecuteFailed(UpgradeTask* task);
public slots:
	virtual void onTaskExecuteProgress(UpgradeTask* task);

private:
	QString m_name;
	int m_code;
	QString m_message;
	QMap<QString, QString>& m_context;
	qint64 m_sent;
	qint64 m_total;
	bool m_bStop;
};

