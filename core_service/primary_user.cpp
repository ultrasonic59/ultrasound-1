#include "core_service/primary_user.h"

namespace core {

bool PrimaryUser::LoadFromJson(const Json::Value& value) {
	m_name = value["name"].asString();
	m_id = value["id"].asString();
	m_free_code = value["free_code"].asString();
	m_province = value["province"].asString();
	m_hospital = value["hospital"].asString();
	m_url_cert = value["url_cert"].asString();
	m_hospital_id = value["hospital_id"].asInt();
	m_selfvalue = value["selfvalue"].asBool();
	m_certificate = value["certificate"].asString();
	m_province_id = value["province_id"].asInt();
	m_uid = value["uid"].asString();
	m_pay = value["pay"].asInt();
	m_union_id = value["union_id"].asInt();
	m_url_thumb = value["url_thumb"].asString();
	m_control_code = value["control_code"].asString();
	m_title = value["title"].asString();
	
	Json::Value subscriber = value["subscriber"];
	if (subscriber.isArray()) {
		for (Json::Value& sub : subscriber) {
			m_subscriber.push_back(sub.asInt());
		}
	}

	m_department = value["department"].asString();
	m_county_id = value["county_id"].asInt();
	m_city = value["city"].asString();
	m_city_id = value["city_id"].asInt();
	m_online = value["online"].asInt();
	m_county = value["county"].asString();
	m_phone = value["phone"].asString();
	m_sex = value["sex"].asString();

	Json::Value ruds_config = value["ruds_config"];
	if (ruds_config.isObject()) {
		m_ruds_report_title = ruds_config["ruds_report_title"].asString();
		m_ruds_report_bedside_title = ruds_config["ruds_report_bedside_title"].asString();
		m_ruds_title_request = ruds_config["ruds_title_request"].asInt();
		m_ruds_confirm_request = ruds_config["ruds_confirm_request"].asInt();
	}

	return true;
}

void PrimaryUser::SaveToJson(Json::Value& value) {

}

} // namespace core