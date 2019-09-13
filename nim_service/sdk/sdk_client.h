#ifndef NIM_SERVICE_SDK_SDK_CLIENT_H_
#define NIM_SERVICE_SDK_SDK_CLIENT_H_

#include <stdint.h>
#include "nim_sdk_api.h"

namespace nim {

class SdkClient {
public:
    static bool Init(const char *app_data_dir, const char *app_install_dir, 
      const char *json_extension);

    static void Cleanup(const char *json_extension);

    static void Login(const char *app_key, const char *account, 
      const char *token, const char *json_extension, 
      nim_json_transport_cb_func cb, const void *user_data);

    static int GetLoginState(const char *json_extension);

    static void Relogin(const char *json_extension);

    static void Logout(enum NIMLogoutType logout_type, 
      const char *json_extension, nim_json_transport_cb_func cb, 
      const void *user_data);

    static void KickOtherClient(const char *json_extension);

    static void RegAutoReloginCb(const char *json_extension,
      nim_json_transport_cb_func cb, const void *user_data);

    static void RegKickoutCb(const char *json_extension, 
      nim_json_transport_cb_func cb, const void *user_data);

    static void RegDisconnectCb(const char *json_extension,
      nim_json_transport_cb_func cb, const void *user_data);

    static void RegMultispotLoginNotifyCb(const char *json_extension,
      nim_json_transport_cb_func cb, const void *user_data);

    static void RegKickoutOtherClientCb(const char *json_extension, 
      nim_json_transport_cb_func cb, const void *user_data);

    static void RegSyncMultiportPushConfigCb(const char *json_extension, 
      nim_client_multiport_push_config_cb_func cb, const void *user_data);

    static void SetMultiportPushConfig(const char *switch_content,
      const char *json_extension, nim_client_multiport_push_config_cb_func cb, 
      const void *user_data);

    static void GetMultiportPushConfig(const char *json_extension, 
      nim_client_multiport_push_config_cb_func cb, const void *user_data);
};

} // namespace nim

#endif // NIM_SERVICE_SDK_SDK_CLIENT_H_


