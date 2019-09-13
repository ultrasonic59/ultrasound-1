#ifndef NIM_SERVICE_SDK_SDK_GLOBAL_H_
#define NIM_SERVICE_SDK_SDK_GLOBAL_H_

#pragma once
#include <stdint.h>
#include "nim_global.h"

namespace nim {
	class SdkGlobal
	{
	public:
		static void setProxy(enum NIMProxyType type, const char *host, int port, const char *user, const char *password);
	};
}

#endif // !NIM_SERVICE_SDK_SDK_GLOBAL_H_
