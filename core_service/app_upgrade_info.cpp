#include "app_upgrade_info.h"

bool core::AppUpgradeInfo::LoadFromJson(const Json::Value& value)
{
	m_version = value["version"].asString();
	m_app = value["app"].asString();
	m_url = value["url"].asString();
	m_message = value["message"].asString();
	m_bforce = value["force"].asBool();
	return true;
}
