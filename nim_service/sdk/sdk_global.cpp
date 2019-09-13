#include "nim_service/sdk/sdk_instance.h"
#include "sdk_global.h"

namespace nim {
	typedef void (*nim_global_set_proxy)(enum NIMProxyType type, const char *host, int port, const char *user, const char *password);
	
	void SdkGlobal::setProxy(NIMProxyType type, const char * host, int port, const char * user, const char * password)
	{
		NIM_SDK_GET_FUNC(nim_global_set_proxy)(type, host, port, user, password);
	}
}
