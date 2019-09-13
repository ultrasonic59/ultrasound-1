#include <QDateTime>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QCoreApplication>
#include <QStandardPaths>
#include "RecentCall.h"

bool compareCallItem(const RecentCallItem& item1, const RecentCallItem &item2)
{
	QDateTime dt1 = QDateTime::fromString(item1.getReceiveTime(), "yyyy-MM-dd hh:mm:ss");
	QDateTime dt2 = QDateTime::fromString(item2.getReceiveTime(), "yyyy-MM-dd hh:mm:ss");
	if (dt1 > dt2)
	{
		return true;
	}
	return false;
}

RecentCall::RecentCall()
{
	
}


RecentCall::~RecentCall()
{
	
}

RecentCall & RecentCall::getInstance()
{
	// TODO: 在此处插入 return 语句
	static RecentCall recentCall;
	return recentCall;
}

void RecentCall::load(QString dir)
{
	recent_call_json_file = dir + QDir::separator() + "recentcall.json";
	QFile json_file(recent_call_json_file);

	if (!json_file.open(QIODevice::ReadWrite))
	{
		qDebug() << "could't open projects json";
		return;
	}

	QString allData = json_file.readAll();
	json_file.close();

	QJsonParseError json_error;
	QJsonDocument jsonDoc(QJsonDocument::fromJson(allData.toUtf8(), &json_error));

	if (json_error.error != QJsonParseError::NoError)
	{
		qDebug() << "json error!";
		return;
	}

	QJsonObject rootObj = jsonDoc.object();

	if (!rootObj.contains("recentcall")) {
		qDebug() << "invalid recent call json error!";
		return;
	}

	QJsonArray recent_calls = rootObj.value("recentcall").toArray();
	for (int i = 0; i< recent_calls.size(); i++)
	{
		QJsonObject recent_call_object = recent_calls.at(i).toObject();
		RecentCallItem call_item(recent_call_object);
		addCallItem(call_item);
	}

	qSort(callItems.begin(), callItems.end(), compareCallItem);
}

void RecentCall::addCallItem(RecentCallItem callItem)
{
	QMutexLocker locker(&mutex);

	if (callItems.size() > 100) {
		callItems.pop_back();
	}
	callItems.push_front(callItem);
}

QList<RecentCallItem> RecentCall::getCallItems()  const
{
	return callItems;
}

void RecentCall::flush()
{
	QMutexLocker locker(&mutex);

	QFile json_file(recent_call_json_file);

	if (!json_file.open(QIODevice::ReadWrite))
	{
		qDebug() << "could't open projects json";
		return;
	}

	QJsonArray recent_call_json;
	for (RecentCallItem& item : callItems) {
		recent_call_json.append(item.getJsonObject());
	}
	QJsonObject root_object;
	root_object.insert("recentcall", recent_call_json);
	QJsonDocument document;
	document.setObject(root_object);
	QByteArray byteArray = document.toJson(QJsonDocument::Compact);
	QString strJson(byteArray);

	QTextStream in(&json_file);
	in.setCodec("UTF-8");
	in << strJson.toUtf8();
	json_file.close();
}

RecentCallItem::RecentCallItem()
{
}

RecentCallItem::RecentCallItem(QJsonObject jsonObject)
	:_jsonObject(jsonObject)
{
}

RecentCallItem::~RecentCallItem()
{

}

QString RecentCallItem::getUid() const
{
	return _jsonObject.value("uid").toString();
}

void RecentCallItem::setUid(QString uid)
{
	_jsonObject.remove("uid");
	_jsonObject.insert("uid", QJsonValue(uid));
}

QString RecentCallItem::getName()  const
{
	return _jsonObject.value("name").toString();
}

void RecentCallItem::setName(QString name)
{
	_jsonObject.remove("name");
	_jsonObject.insert("name", QJsonValue(name));
}

QString RecentCallItem::getSex()  const
{
	return _jsonObject.value("sex").toString();
}

void RecentCallItem::setSex(QString sex)
{
	_jsonObject.remove("sex");
	_jsonObject.insert("sex", QJsonValue(sex));
}

QString RecentCallItem::getReceiveTime() const
{
	return _jsonObject.value("receiveTime").toString();
}

void RecentCallItem::setReceiveTime(QString receiveTime)
{
	_jsonObject.remove("receiveTime");
	_jsonObject.insert("receiveTime", QJsonValue(receiveTime));
}

QString RecentCallItem::getReceiveState() const
{
	return _jsonObject.value("receiveState").toString();
}

void RecentCallItem::setReceiveState(QString receiveState)
{
	_jsonObject.remove("receiveState");
	_jsonObject.insert("receiveState", QJsonValue(receiveState));
}

QString RecentCallItem::getTalkTime() const
{
	return _jsonObject.value("talkTime").toString();
}

void RecentCallItem::setTalkTime(QString talkTime)
{
	_jsonObject.remove("talkTime");
	_jsonObject.insert("talkTime", QJsonValue(talkTime));
}

QString RecentCallItem::getTelephone() const
{
	return _jsonObject.value("telephone").toString();
}

void RecentCallItem::setTelephone(QString telephone)
{
	_jsonObject.remove("telephone");
	_jsonObject.insert("telephone", QJsonValue(telephone));
}

QString RecentCallItem::getHospital() const
{
	return _jsonObject.value("hospital").toString();
}

void RecentCallItem::setHospital(QString hospital)
{
	_jsonObject.remove("hospital");
	_jsonObject.insert("hospital", QJsonValue(hospital));
}

QJsonObject RecentCallItem::getJsonObject()
{
	return _jsonObject;
}
