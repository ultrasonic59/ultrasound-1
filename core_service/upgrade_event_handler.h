
#ifndef CORE_SERVICE_UPGRADE_EVENT_HANDLER_H_
#define CORE_SERVICE_UPGRADE_EVENT_HANDLER_H_
#pragma once
#include "core_service/app_upgrade_info.h"

namespace core {

	class UpgradeEventHandler {
	public:
		virtual ~UpgradeEventHandler() {}

		virtual void OnCheckUpgradeSuccessed(AppUpgradeInfo upgradeInfo) = 0;
		virtual void OnCheckUpgradeFailed(int error_code, const std::string& message) = 0;
	};

} // namespace core 

#endif // CORE_SERVICE_REPORT_EVENT_HANDLER_H_