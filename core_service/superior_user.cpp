#include "core_service/superior_user.h"

namespace core {

bool SuperiorUser::LoadFromJson(const Json::Value& value) {
	Json::Value frees = value["frees"];
	if (frees.isArray()) {
		for (Json::Value& free : frees) {
			m_frees.push_back(free.asString());
		}
	}

	m_province = value["province"].asString();
	m_hospital = value["hospital"].asString();
	m_url_cert = value["url_cert"].asString();
	m_hospital_id = value["hospital_id"].asInt();
	m_uid = value["uid"].asString();
	m_description = value["description"].asString();
	
	Json::Value controlled = value["controlled"];
	if (controlled.isArray()) {
		for (Json::Value& ctrl : controlled) {
			m_controlled.push_back(ctrl.asString());
		}
	}
	
	Json::Value publish = value["publish"];
	if (publish.isArray()) {
		for (Json::Value& pub : publish) {
			m_publish.push_back(pub.asInt());
		}
	}

	m_title = value["title"].asString();
	m_second_hospital = value["second_hospital"].asString();
	m_visit_code = value["visit_code"].asString();
	m_city_id = value["city_id"].asInt();
	m_online = value["online"].asInt();
	m_county = value["county"].asString();
	m_sex = value["sex"].asString();
	m_name = value["name"].asString();
	m_id = value["id"].asString();
	m_price = value["price"].asInt();
	m_free = value["free"].asInt();
	m_certificate = value["certificate"].asString();
	m_province_id = value["province_id"].asInt();
	m_union_id = value["union_id"].asInt();
	m_url_thumb = value["url_thumb"].asString();
	m_county_id = value["county_id"].asInt();
	m_city = value["city"].asString();
	m_skill = value["skill"].asString();
	m_birth = value["birth"].asInt();
	m_phone = value["phone"].asString();

	Json::Value ruds_config = value["ruds_config"];
	if (ruds_config.isObject()) {
		m_ruds_report_title = ruds_config["ruds_report_title"].asString();
		m_ruds_report_bedside_title = ruds_config["ruds_report_bedside_title"].asString();
		m_ruds_title_request = ruds_config["ruds_title_request"].asInt();
		m_ruds_confirm_request = ruds_config["ruds_confirm_request"].asInt();
	}

	return true;
}

void SuperiorUser::SaveToJson(Json::Value& value) {

}

} // namespace core