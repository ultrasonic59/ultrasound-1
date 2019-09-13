#pragma once
#include <list>
#include <string>

#include "json.h"

namespace core {

	class AppUpgradeInfo {
	public:
		bool LoadFromJson(const Json::Value& value);

		const std::string& GetVersion() const { return m_version; }
		const std::string& GetApp() const { return m_app; }
		const std::string& GetUrl() const { return m_url; }
		const std::string& GetMessage() const { return m_message; }
		const bool GetForce() const { return m_bforce; }
	private:
		std::string m_version;
		std::string m_app;
		std::string m_url;
		std::string m_message;
		bool m_bforce;
	};
}


