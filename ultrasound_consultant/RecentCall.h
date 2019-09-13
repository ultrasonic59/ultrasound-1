#pragma once

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QList>
#include <QMutex>
#include <QFile>

class RecentCallItem {
public:
	RecentCallItem();
	RecentCallItem(QJsonObject jsonObject);
	virtual ~RecentCallItem();

	QString getUid() const;
	void setUid(QString uid);
	QString getName() const;
	void setName(QString name);
	QString getSex() const;
	void setSex(QString sex);
	QString getReceiveTime() const;
	void setReceiveTime(QString receiveTime);
	QString getReceiveState() const;
	void setReceiveState(QString receiveState);
	QString getTalkTime() const;
	void setTalkTime(QString talkTime);
	QString getTelephone() const;
	void setTelephone(QString telephone);
	QString getHospital() const;
	void setHospital(QString hospital);

	QJsonObject getJsonObject();
private:
	QJsonObject _jsonObject;
};

class RecentCall
{
public:
	RecentCall();
	virtual ~RecentCall();

	static RecentCall& getInstance();
	void load(QString dir);

	void addCallItem(RecentCallItem callItem);
	QList<RecentCallItem> getCallItems() const;

	void flush();
private:
	QMutex mutex;
	QList<RecentCallItem> callItems;
	QJsonDocument _document;
	QString recent_call_json_file;
};

