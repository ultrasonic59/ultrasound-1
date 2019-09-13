#ifndef NIM_SERVICE_CALLBACK_CLIENT_CALLBACK_H_
#define NIM_SERVICE_CALLBACK_CLIENT_CALLBACK_H_

#include <stdint.h>
#include "nim_sdk_api.h"

namespace nim {

class ClientCallback {
public:

    static void LoginCb(const char *json_params, const void *user_data);

    static void LogoutCb(const char *json_params, const void *user_data);

    static void AutoReloginCb(const char *json_params, const void *user_data);

    static void KickoutCb(const char *json_params, const void *user_data);

    static void DisconnectCb(const char *json_params, const void *user_data);

    static void MultispotLoginNotifyCb(const char *json_params, 
      const void *user_data);

    static void KickoutOtherClientCb(const char *json_params,
        const void *user_data);

    static void SyncMultiportPushConfigCb(int rescode, const char *content, 
      const char *json_params, const void *user_data);

    static void SetMultiportPushConfigCb(int rescode, const char *content,
        const char *json_params, const void *user_data);

    static void GetMultiportPushConfigCb(int rescode, const char *content,
        const char *json_params, const void *user_data);
};

} // namespace nim

#endif // NIM_SERVICE_CALLBACK_CLIENT_CALLBACK_H_