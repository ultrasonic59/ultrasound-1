#include "report_html.h"

#include <QDate>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonObject>
#include <QStringList>
#include <QTime>

#include "core_service/core_service.h"

QString ReportHtml::BuildHtml(const QJsonObject& report, QString& report_path,
	int primary_hospital_id, int superior_hospital_id) {
	if (primary_hospital_id == MIAN_YANG_3RD_HOSPITAL_ID || superior_hospital_id == MIAN_YANG_3RD_HOSPITAL_ID)
		return BuildHtml3rdHospital(report, report_path);

	return BuildHtmlNormal(report, report_path);
}

QString ReportHtml::BuildHtmlNormal(const QJsonObject& report, QString& report_path) {
    QString sample = QString::fromLocal8Bit("一二三四五六七八九十"
        "一二三四五六七八九十一二三四五六七八九十一二三四五六七八九十一二");
    int MAX_CHARS_PER_LINE = sample.size();

    int MAX_LINES_IN_DES = 13;
    int MAX_LINES_IN_CON = 5;
    
    QString name = report["name"].toString();
    QString sex = report["sex"].toString();
    QString age = QString::number(report["age"].toInt());
	QString patient_id = report["patient_id"].toString();
    QString check_point = report["parts"].toString();
    QString date = report["check_date"].toString();
    QString ultrasound_number = report["uid"].toString();
	QString description = report["content"].toString().replace("<", QString::fromLocal8Bit("&lt;"));
	QString conclusion = report["conclusion"].toString().replace("<", QString::fromLocal8Bit("&lt;"));
    QString department = report["department"].toString();
    core::CONFIRM_STATUS status = (core::CONFIRM_STATUS)report["confirm_status"].toInt();

    std::list<QString> checked_image_list;
    if (report.contains("images") && report["images"].isArray()) {
        QJsonArray capture_images = report["images"].toArray();

        for (int i = 0; i < capture_images.size(); i++) {
            QFileInfo fileInfo(capture_images[i].toString());
            QString filename(fileInfo.fileName());

            QString image = report_path + "/";
            image += filename;
            checked_image_list.push_back(image);
        }
    }

    QString country = QString::fromStdString(
        core::CoreService::GetInstance()->GetCurrentPrimaryUser().GetCountry());
    QString city = QString::fromStdString(
        core::CoreService::GetInstance()->GetCurrentPrimaryUser().GetCity());

    QString region = country.isEmpty() ? city : country;
    if (region.isEmpty())
        region = QString::fromLocal8Bit("多点");

    QString hospital = report["hospital"].toString();
    QString primary = report["primary"].toString();
    QString superior = report["superior"].toString();

	QString title = report["title"].toString();
	QStringList pieces = title.split(" ");
	QString header_hospital = pieces.size() > 1 ? pieces.at(0) : hospital;
	QString header_title = pieces.size() > 1 ? pieces.at(1) : QString::fromLocal8Bit("远程超声会诊报告单");

    // -------------------------------------------------------------------------
	QString qr_code_image = report_path + "/qr_code.png";
	if (QFileInfo(qr_code_image).exists() == false) {
		qr_code_image = ":/ultrasound/res/qr.bmp";
	}

    QString sign_image = report_path + "/sign.png";
    if (QFileInfo(sign_image).exists() == false) {
      sign_image.clear();
    }

    QString header = QString::fromLocal8Bit(
    "<html>"
    "<head>"
        "<style>"
        "p { font-family:宋体 }"
        "</style>"
    "</head>"
    "<body>"
      "<div>"
        "<img width=110 height=110 src=\"") + qr_code_image + QString::fromLocal8Bit("\" align=right>"
        "<div align=center>"
            "<p align=center><span style='font-size:18.0pt;'>") + header_hospital + QString::fromLocal8Bit("</span></p>"
            "<p align=center><span style='font-size:18.0pt;'>") + header_title + QString::fromLocal8Bit("</span></p>"
        "</div>"
      "</div>"
      "<p style='margin-top:35; margin-bottom:1'><img width=620 height=2 src=\":/ultrasound/res/hr.png\" align=center></p>");

    // -------------------------------------------------------------------------
	QString Qname = QString::fromLocal8Bit("<span>");
	int num = 6-name.count();
	for(int i=num;i>0;i--)
	{
		Qname = Qname + QString::fromLocal8Bit("&nbsp;");
	}
	Qname= Qname+ QString::fromLocal8Bit("</span>性别: ");

	QString patient = QString::fromLocal8Bit(
		"<p align=left style='font-size:13.0pt'>姓名: ") + name +
		Qname + sex +
		QString::fromLocal8Bit(
			"<span>&nbsp;&nbsp;</span>年龄: ") + age +
		QString::fromLocal8Bit("岁") +
		QString::fromLocal8Bit(
			"<span>&nbsp;&nbsp;</span>住院号: ") + patient_id +
		QString::fromLocal8Bit(" "

      "</p>"
      "<p align=left style='font-size:13.0pt'>申请单位: ") + department +
		QString::fromLocal8Bit(
			"<span>&nbsp;&nbsp;&nbsp;</span>检查部位: ") + check_point +
        QString::fromLocal8Bit(
      "</p>"
      "<p align=left style='font-size:13.0pt'>超声编号: ") + ultrasound_number +
        QString::fromLocal8Bit(
        "<span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</span>检查日期: ") + date +
        QString::fromLocal8Bit(
      "</p>"
      "<p style='margin-top:2; margin-bottom:2'><img width=620 height=2 src=\":/ultrasound/res/hr.png\" align=center></p>");

    // -------------------------------------------------------------------------
    QString images = QString::fromLocal8Bit(""
      "<p align=center style='margin-top:5; margin-bottom:5'>"
        "<span>");

    if (checked_image_list.size() > 0) {
        for (QString& image : checked_image_list) {
            QString img = QString::fromLocal8Bit("<img width=180 height=144 src=\"")  // 720:576 => 180:144
                + image + QString::fromLocal8Bit("\" align=center>&nbsp;");
            images += img;
        }
    }
    else {
        // Place holder image
        QString img = QString::fromLocal8Bit(""
            "<img width=180 height=144 src=\":/ultrasound/res/white.png\">&nbsp;");
        images += img;
    }
    images += QString::fromLocal8Bit(
        "</span>"
      "</p>"
      "<p style='margin-top:0; margin-bottom:5'><img width=620 height=2 src=\":/ultrasound/res/hr.png\" align=center></p>");

    // -------------------------------------------------------------------------
    QString line_start = QString::fromLocal8Bit("<p align=left style='margin-top:5;margin-bottom:5;font-size:11.0pt'>");
    QString line_end = QString::fromLocal8Bit("</p>");
    QString nbsp_line = line_start + "&nbsp;" + line_end;

    QStringList des_lines = description.split("\n");
    QString des = QString::fromLocal8Bit(""
      "<p align=left style='margin-top:5; margin-bottom:5'>"
        "<b><span style='font-size:12.0pt;'>描述:</span></b>"
      "</p>");

    int des_nbsp_lines = MAX_LINES_IN_DES;
    for (QString& line : des_lines) {
        QString line_text = line;
        if (line.isEmpty()) {
            line_text = "&nbsp;";
        }
        else {
            line_text.replace('\t', "&nbsp;&nbsp;");
        }
        
        des += (line_start + line_text + line_end);

        if (line.size() > MAX_CHARS_PER_LINE) {
            int span_lines = line.size() / MAX_CHARS_PER_LINE;
            int mod = line.size() % MAX_CHARS_PER_LINE;
            if (mod > 0)
                span_lines++;

            des_nbsp_lines -= span_lines;
        }
        else {
            des_nbsp_lines--;
        }
    }

    for (int i = 0; i < des_nbsp_lines; i++) {
        des += nbsp_line;
    }

    des += nbsp_line;

    // -------------------------------------------------------------------------
    QStringList con_lines = conclusion.split("\n");

    QString con = QString::fromLocal8Bit(
      "<p align=left style='margin-top:5; margin-bottom:5'>"
        "<b><span style='font-size:12.0pt;'>诊断提示:</span></b>"
      "</p>");

    int con_nbsp_lines = MAX_LINES_IN_CON;
    if (status == core::CONFIRM_STATUS_PASS && !sign_image.isEmpty()) {
      con_nbsp_lines = MAX_LINES_IN_CON - 1;
    }

    for (QString& line : con_lines) {

        con += (line_start + line + line_end);

        if (line.size() > MAX_CHARS_PER_LINE) {
            int span_lines = line.size() / MAX_CHARS_PER_LINE;
            int mod = line.size() % MAX_CHARS_PER_LINE;
            if (mod > 0)
                span_lines++;

            con_nbsp_lines -= span_lines;
        }
        else {
            con_nbsp_lines--;
        }
    }

    for (int i = 0; i < con_nbsp_lines; i++) {
        con += nbsp_line;
    }

    con += nbsp_line;

    // -------------------------------------------------------------------------
    QDate print_date = QDate::currentDate();
    QTime print_time = QTime::currentTime();
    QString timestamp = print_date.toString("yyyy-MM-dd") + print_time.toString("  hh:mm:ss");

    QString foot = QString::fromLocal8Bit(" "
      "<p align=left style='margin-top:1;margin-top:1; margin-bottom:5;font-size:11.0pt'>报告医生: ") +
        primary + QString::fromLocal8Bit(
          "<span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</span>"
          "会诊医生: ") + superior;
    
    if (status == core::CONFIRM_STATUS_PASS && !sign_image.isEmpty()) {
      foot += (QString::fromLocal8Bit(
        "<img width=100 height=50 src=\"") + sign_image + QString::fromLocal8Bit("\">"));
    }

      foot += QString::fromLocal8Bit(
      "</p>"
      "<p style='margin-top:1; margin-bottom:1'><img width=620 height=2 src=\":/ultrasound/res/hr.png\" align=center></p>"
      "<p style='margin-top:1; margin-bottom:1'>"
        "<b><span align=left style='font-size:11.0pt;'>注: </span></b>"
        "<span align=left style='font-size:9.0pt;'>此报告反映受查者当时情况，仅供临床医生参考。</span>"
        "<span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</span>"
        "<span align=right style='font-size:9.0pt;'>打印时间: ") + timestamp + QString::fromLocal8Bit(
        "</span>"
      "</p>"
    "</body>"
    "</html>");

    QString html = header + patient + images + des + con + foot;
    return html;
}

QString ReportHtml::BuildHtml3rdHospital(const QJsonObject& report, QString& report_path) {
	QString sample = QString::fromLocal8Bit("一二三四五六七八九十"
		"一二三四五六七八九十一二三四五六七八九十一二三四五六七八九十一二");
	int MAX_CHARS_PER_LINE = sample.size();

	int MAX_LINES_IN_DES = 11;
	int MAX_LINES_IN_CON = 5;

	QString name = report["name"].toString();
	QString sex = report["sex"].toString();
	QString age = QString::number(report["age"].toInt());
	QString patient_id = report["patient_id"].toString();
	QString date = report["check_date"].toString();
	QString ultrasound_number = report["uid"].toString();
	QString description = report["content"].toString().replace("<", QString::fromLocal8Bit("&lt;"));
	QString conclusion = report["conclusion"].toString().replace("<", QString::fromLocal8Bit("&lt;"));
	
	QString hospital = report["hospital"].toString();
	QString department = report["department"].toString();
    core::CONFIRM_STATUS status = (core::CONFIRM_STATUS)report["confirm_status"].toInt();

	// Add primary hospital to department if ruds_title_request is true.
	bool title_request = report["ruds_title_request"].toInt() == 1;
	if (title_request) {
		department = hospital + department;
	}

	QString ruds_reason = report["ruds_reason"].toString();
	QString ruds_target = report["ruds_target"].toString();

	std::list<QString> checked_image_list;
	if (report.contains("images") && report["images"].isArray()) {
		QJsonArray capture_images = report["images"].toArray();

		for (int i = 0; i < capture_images.size(); i++) {
			QFileInfo fileInfo(capture_images[i].toString());
			QString filename(fileInfo.fileName());

			QString image = report_path + "/";
			image += filename;
			checked_image_list.push_back(image);
		}
	}

	QString country = QString::fromStdString(
		core::CoreService::GetInstance()->GetCurrentPrimaryUser().GetCountry());
	QString city = QString::fromStdString(
		core::CoreService::GetInstance()->GetCurrentPrimaryUser().GetCity());

	QString region = country.isEmpty() ? city : country;
	if (region.isEmpty())
		region = QString::fromLocal8Bit("多点");

	QString primary = report["primary"].toString();
	QString superior = report["superior"].toString();

	QString title = report["title"].toString();
	QStringList pieces = title.split(" ");
	QString header_hospital = pieces.size() > 0 ? pieces.at(0) : hospital;
	QString header_title = pieces.size() > 1 ? pieces.at(1) : QString::fromLocal8Bit("远程超声会诊报告单");

	// -------------------------------------------------------------------------
	QString qr_code_image = report_path + "/qr_code.png";
	if (QFileInfo(qr_code_image).exists() == false) {
		qr_code_image = ":/ultrasound/res/qr.bmp";
	}

    QString sign_image = report_path + "/sign.png";
    if (QFileInfo(sign_image).exists() == false) {
      sign_image.clear();
    }

	QString header = QString::fromLocal8Bit(
		"<html>"
		"<head>"
		"<style>"
		"p { font-family:宋体 }"
		"</style>"
		"</head>"
		"<body>"
		"<div>"
		"<img width=110 height=110 src=\"") + qr_code_image + QString::fromLocal8Bit("\" align=right>"
			"<div align=center>"
			"<p align=center><span style='font-size:18.0pt;'>") + header_hospital + QString::fromLocal8Bit("</span></p>"
				"<p align=center><span style='font-size:18.0pt;'>") + header_title + QString::fromLocal8Bit("</span></p>"
					"</div>"
					"</div>"
					"<p style='margin-top:35; margin-bottom:1'><img width=620 height=2 src=\":/ultrasound/res/hr.png\" align=center></p>");

	// -------------------------------------------------------------------------
	QString Qname = QString::fromLocal8Bit("<span>");
	int num = 6 - name.count();
	for (int i = num; i>0; i--)
	{
		Qname = Qname + QString::fromLocal8Bit("&nbsp;");
	}
	Qname = Qname + QString::fromLocal8Bit("</span>性别: ");

	QString patient = QString::fromLocal8Bit(
		"<p align=left style='font-size:13.0pt'>姓名: ") + name +
		Qname + sex +
		QString::fromLocal8Bit(
			"<span>&nbsp;&nbsp;</span>年龄: ") + age +
		QString::fromLocal8Bit("岁") +
		QString::fromLocal8Bit(
			"<span>&nbsp;&nbsp;</span>住院号: ") + patient_id +
		QString::fromLocal8Bit(" "

			"</p>"
			"<p align=left style='font-size:13.0pt'>申请单位: ") + department +
		QString::fromLocal8Bit(
			"</p>"
			"<p align=left style='font-size:13.0pt'>会诊原因: ") + ruds_reason +
		QString::fromLocal8Bit(
			"<span>&nbsp;&nbsp;&nbsp;</span>会诊目的: ") + ruds_target +
		QString::fromLocal8Bit(
			"</p>"
			"<p align=left style='font-size:13.0pt'>超声编号: ") + ultrasound_number +
		QString::fromLocal8Bit(
			"<span>&nbsp;&nbsp;&nbsp;</span>检查日期: ") + date +
		QString::fromLocal8Bit(
			"</p>"
			"<p style='margin-top:2; margin-bottom:2'><img width=620 height=2 src=\":/ultrasound/res/hr.png\" align=center></p>");

	// -------------------------------------------------------------------------
	QString images = QString::fromLocal8Bit(""
		"<p align=center style='margin-top:5; margin-bottom:5'>"
		"<span>");

	if (checked_image_list.size() > 0) {
		for (QString& image : checked_image_list) {
			QString img = QString::fromLocal8Bit("<img width=180 height=144 src=\"")  // 720:576 => 180:144
				+ image + QString::fromLocal8Bit("\" align=center>&nbsp;");
			images += img;
		}
	}
	else {
		// Place holder image
		QString img = QString::fromLocal8Bit(""
			"<img width=180 height=144 src=\":/ultrasound/res/white.png\">&nbsp;");
		images += img;
	}
	images += QString::fromLocal8Bit(
		"</span>"
		"</p>"
		"<p style='margin-top:0; margin-bottom:5'><img width=620 height=2 src=\":/ultrasound/res/hr.png\" align=center></p>");

	// -------------------------------------------------------------------------
	QString line_start = QString::fromLocal8Bit("<p align=left style='margin-top:5;margin-bottom:5;font-size:11.0pt'>");
	QString line_end = QString::fromLocal8Bit("</p>");
	QString nbsp_line = line_start + "&nbsp;" + line_end;

	QStringList des_lines = description.split("\n");
	QString des = QString::fromLocal8Bit(""
		"<p align=left style='margin-top:5; margin-bottom:5'>"
		"<b><span style='font-size:12.0pt;'>描述:</span></b>"
		"</p>");

	int des_nbsp_lines = MAX_LINES_IN_DES;
	for (QString& line : des_lines) {
		QString line_text = line;
		if (line.isEmpty()) {
			line_text = "&nbsp;";
		}
		else {
			line_text.replace('\t', "&nbsp;&nbsp;");
		}

		des += (line_start + line_text + line_end);

		if (line.size() > MAX_CHARS_PER_LINE) {
			int span_lines = line.size() / MAX_CHARS_PER_LINE;
			int mod = line.size() % MAX_CHARS_PER_LINE;
			if (mod > 0)
				span_lines++;

			des_nbsp_lines -= span_lines;
		}
		else {
			des_nbsp_lines--;
		}
	}

	for (int i = 0; i < des_nbsp_lines; i++) {
		des += nbsp_line;
	}

	des += nbsp_line;

	// -------------------------------------------------------------------------
	QStringList con_lines = conclusion.split("\n");

	QString con = QString::fromLocal8Bit(
		"<p align=left style='margin-top:5; margin-bottom:5'>"
		"<b><span style='font-size:12.0pt;'>诊断提示:</span></b>"
		"</p>");

	int con_nbsp_lines = MAX_LINES_IN_CON;
    if (status == core::CONFIRM_STATUS_PASS && !sign_image.isEmpty()) {
      con_nbsp_lines = MAX_LINES_IN_CON - 1;
    }

	for (QString& line : con_lines) {

		con += (line_start + line + line_end);

		if (line.size() > MAX_CHARS_PER_LINE) {
			int span_lines = line.size() / MAX_CHARS_PER_LINE;
			int mod = line.size() % MAX_CHARS_PER_LINE;
			if (mod > 0)
				span_lines++;

			con_nbsp_lines -= span_lines;
		}
		else {
			con_nbsp_lines--;
		}
	}

	for (int i = 0; i < con_nbsp_lines; i++) {
		con += nbsp_line;
	}

	con += nbsp_line;

	// -------------------------------------------------------------------------
	QDate print_date = QDate::currentDate();
	QTime print_time = QTime::currentTime();
	QString timestamp = print_date.toString("yyyy-MM-dd") + print_time.toString("  hh:mm:ss");

	QString foot = QString::fromLocal8Bit(" "
		"<p align=left style='margin-top:1;margin-top:1; margin-bottom:5;font-size:11.0pt'>报告医生: ") +
		primary + QString::fromLocal8Bit(
			"<span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</span>"
          "会诊医生: ") + superior;

    if (status == core::CONFIRM_STATUS_PASS && !sign_image.isEmpty()) {
      foot += (QString::fromLocal8Bit(
        "<img width=100 height=50 src=\"") + sign_image + QString::fromLocal8Bit("\">"));
    }

    foot += QString::fromLocal8Bit(
      "</p>"
      "<p style='margin-top:1; margin-bottom:1'><img width=620 height=2 src=\":/ultrasound/res/hr.png\" align=center></p>"
      "<p style='margin-top:1; margin-bottom:1'>"
      "<b><span align=left style='font-size:11.0pt;'>注: </span></b>"
        "<span align=left style='font-size:9.0pt;'>此报告反映受查者当时情况，仅供临床医生参考。</span>"
        "<span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</span>"
        "<span align=right style='font-size:9.0pt;'>打印时间: ") + timestamp + QString::fromLocal8Bit(
        "</span>"
      "</p>"
      "</body>"
      "</html>");

	QString html = header + patient + images + des + con + foot;
	return html;
}