#include "core_service/report.h"

namespace core {

bool Report::LoadFromJson(const Json::Value& value) {

    m_uid = value["uid"].asString();
    m_name = value["name"].asString();
    m_sex = value["sex"].asString();
    m_age = value["age"].asInt();
    m_parts = value["parts"].asString();
    m_content = value["content"].asString();
    m_conclusion = value["conclusion"].asString();
    m_department = value["department"].asString();
    m_dialog_id = value["dialog_id"].asString();
    m_check_date = value["check_date"].asString();
    m_primary = value["primary"].asString();
    m_primary_id = value["primary_id"].asString();

    m_endpoint = value["endpoint"].asString();
    m_bucket = value["bucket"].asString();

    Json::Value images = value["images"];
    if (images.isArray()) {
        for (Json::Value& image : images) {
            m_images.push_back(image.asString());
        }
    }

    m_hospital = value["hospital"].asString();
    m_hospital_id = value["hospital_id"].asInt();
    m_channel_created = value["channel_created"].asBool();
    m_clouded = value["clouded"].asBool();

    m_channel = value["channel"].asString();
    m_channel_time = value["channel_time"].asInt();
    m_superior = value["superior"].asString();
    m_superior_id = value["superior_id"].asString();

    m_phone = value["phone"].asString();
    m_bed = value["bed"].asString();
    m_device = value["device"].asString();
    m_recorder = value["recorder"].asString();
    m_dia_doctor = value["dia_doctor"].asString();

    m_video = value["video"].asString();

	m_patient_id = value["patient_id"].asString();
	m_ruds_reason = value["ruds_reason"].asString();
	m_ruds_target = value["ruds_target"].asString();
	m_title = value["title"].asString();
	m_confirm_status = (CONFIRM_STATUS)value["confirm_status"].asInt();
	m_confirm_msg = value["confirm_msg"].asString();
	m_report_url = value["report_url"].asString();
	m_bedside_type = value["bedside_type"].asInt();

	m_hospital_s_id = value["hospital_s_id"].asInt();
	m_hospital_s_name = value["hospital_s_name"].asString();

	m_ruds_title_request = value["ruds_title_request"].asInt();
	m_ruds_report_title = value["ruds_report_title"].asString();
	m_ruds_report_bedside_title = value["ruds_report_bedside_title"].asString();

	return true;
}

void Report::SaveToJson(Json::Value& value, bool save_to_server) {
    // 必须输入值
    value["uid"] = m_uid;  // 超声编号，全局唯一
    value["name"] = m_name; // 患者姓名
    value["sex"] = m_sex;  // 患者性别
    value["age"] = m_age;  // 患者年龄
    value["parts"] = m_parts;      // 检查部位
    value["content"] = m_content;    // 检查描述
    value["conclusion"] = m_conclusion; // 检查结论
    value["department"] = m_department; // 送检科室
    value["dialog_id"] = m_dialog_id;  // 门诊号 / 住院号
    value["check_date"] = m_check_date; // 检查日期
    value["primary"] = m_primary;    // 基层医生
    value["primary_id"] = m_primary_id; // 基层医生ID

    value["endpoint"] = m_endpoint;
    value["bucket"] = m_bucket;

    // 采集图像列表
    Json::Value images(Json::arrayValue);
    for (const std::string& image : m_images) {
        images.append(image);
    }
    value["images"] = images;
    
    value["hospital"] = m_hospital; // 基层送检医院，这个不在报告里体现，主要是用于查询列表的的显示
    value["hospital_id"] = m_hospital_id;
    value["channel_created"] = m_channel_created;   // 通道是否创建，是否有过远程通话
    value["clouded"] =  m_clouded;           // 是否上云，是否开放给患者

    // 如果有通道，这些就是必填值
    value["channel"] = m_channel;      // 通话通道ID
    value["channel_time"] = m_channel_time; // 通话通道耗时，用秒计时
    value["superior"] = m_superior;     // 上级会诊医生
    value["superior_id"] = m_superior_id;  // 上级会诊医生ID

    // 选填输入值
    value["phone"] = m_phone;      // 患者电话，没有就不能向用户开放查询
    value["bed"] = m_bed;        // 床号
    value["device"] = m_device;     // 仪器ID
    value["recorder"] = m_recorder;   // 记录员名字
    value["dia_doctor"] = m_dia_doctor; // 送检医生

	value["patient_id"] = m_patient_id;
	value["ruds_reason"] = m_ruds_reason;
	value["ruds_target"] = m_ruds_target;
	value["title"] = m_title;
	value["confirm_status"] = m_confirm_status;
	value["confirm_msg"] = m_confirm_msg;
	value["report_url"] = m_report_url;
	value["bedside_type"] = m_bedside_type;

	if (!save_to_server) {
		value["hospital_s_id"] = m_hospital_s_id;
		value["hospital_s_name"] = m_hospital_s_name;
	}

	value["ruds_title_request"] = m_ruds_title_request;
	value["ruds_report_title"] = m_ruds_report_title;
	value["ruds_report_bedside_title"] = m_ruds_report_bedside_title;
}

void Report::New(const std::string& uid) {
    Clear();
    m_uid = uid;
}

void Report::Clear() {
    m_uid.clear();
    m_name.clear();
    m_sex.clear();
    m_age = 0;
    m_parts.clear();
    m_content.clear();
    m_conclusion.clear();
    m_department.clear();
    m_dialog_id.clear();
    m_check_date.clear();
    m_primary.clear();
    m_primary_id.clear();

    m_endpoint.clear();
    m_bucket.clear();
    m_images.clear();
    
    m_hospital.clear();
    m_channel_created = false;
    m_clouded = false;

    m_channel.clear();
    m_channel_time = 0;
    m_superior.clear();
    m_superior_id.clear();

    m_phone.clear();
    m_bed.clear();
    m_device.clear();
    m_recorder.clear();
    m_dia_doctor.clear();

    m_isfree = false;
    m_fee = 0;
    m_province_s_id = 0;
    m_city_s_id = 0;
    m_county_s_id = 0;
    m_hospital_s_id = 0;
	m_hospital_s_name.clear();

    m_updated = false;
    m_confirmed = false;
    m_video.clear();
    m_others.clear();

    m_province_id = 0;
    m_city_id = 0;
    m_county_id = 0;
    m_hospital_id = 0;

	m_patient_id.clear();
	m_ruds_reason.clear();
	m_ruds_target.clear();
	m_title.clear();
	m_confirm_status = CONFIRM_STATUS_NONE;
	m_confirm_msg.clear();
	m_report_url.clear();
	m_bedside_type = 0;

	m_ruds_title_request = 0;
	m_ruds_report_title.clear();
	m_ruds_report_bedside_title.clear();
}

} // namespace core