#ifndef NIM_SERVICE_SYSMSG_EVENT_HANDLER_H_
#define NIM_SERVICE_SYSMSG_EVENT_HANDLER_H_
#pragma once

#include <stdint.h>
#include <string>

namespace nim {

struct SystemMessage {
	std::string message;
	std::string send_uid;
	std::string receive_uid;
};

class SysMsgEventHandler {
public:
	virtual void OnReceiveMessage(const SystemMessage& sys_msg) = 0;
};
}

#endif//NIM_SERVICE_SYSMSG_EVENT_HANDLER_H_