#pragma once

#include <list>

#include <QJsonObject>
#include <QString>

class ReportHtml {
public:
    static QString BuildHtml(const QJsonObject& report, QString& report_path, 
		int primary_hospital_id, int superior_hospital_id);

private:

	static QString BuildHtmlNormal(const QJsonObject& report, QString& report_path);

	static QString BuildHtml3rdHospital(const QJsonObject& report, QString& report_path);

	// �����е�������ҽԺID
	static const int MIAN_YANG_3RD_HOSPITAL_ID = 280202001;
};