#include "nim_service/sdk/sdk_client.h"

#include "nim_service/sdk/sdk_instance.h"

namespace nim {

typedef bool(*nim_client_init)(const char *app_data_dir, 
  const char *app_install_dir, const char *json_extension);
typedef void(*nim_client_cleanup)(const char *json_extension);
typedef void(*nim_client_login)(const char *app_key, const char *account, 
  const char *token, const char *json_extension, nim_json_transport_cb_func cb, 
  const void *user_data);
typedef int(*nim_client_get_login_state)(const char *json_extension);
typedef void(*nim_client_relogin)(const char *json_extension);
typedef void(*nim_client_logout)(enum NIMLogoutType logout_type, 
  const char *json_extension, nim_json_transport_cb_func cb, 
  const void *user_data);
typedef void(*nim_client_kick_other_client)(const char *json_extension);
typedef void(*nim_client_reg_auto_relogin_cb)(const char *json_extension, 
  nim_json_transport_cb_func cb, const void *user_data);
typedef void(*nim_client_reg_kickout_cb)(const char *json_extension, 
  nim_json_transport_cb_func cb, const void *user_data);
typedef void(*nim_client_reg_disconnect_cb)(const char *json_extension, 
  nim_json_transport_cb_func cb, const void *user_data);
typedef void(*nim_client_reg_multispot_login_notify_cb)(
  const char *json_extension, nim_json_transport_cb_func cb, 
  const void *user_data);
typedef void(*nim_client_reg_kickout_other_client_cb)(
  const char *json_extension, nim_json_transport_cb_func cb, 
  const void *user_data);
typedef void(*nim_client_reg_sync_multiport_push_config_cb)(
  const char *json_extension, nim_client_multiport_push_config_cb_func cb, 
  const void *user_data);
typedef void(*nim_client_set_multiport_push_config)(const char *switch_content, 
  const char *json_extension, nim_client_multiport_push_config_cb_func cb, 
  const void *user_data);
typedef void(*nim_client_get_multiport_push_config)(const char *json_extension,
  nim_client_multiport_push_config_cb_func cb, const void *user_data);

bool SdkClient::Init(const char *app_data_dir, const char *app_install_dir,
  const char *json_extension) {
    return NIM_SDK_GET_FUNC(nim_client_init)(app_data_dir, app_install_dir, 
      json_extension);
}

void SdkClient::Cleanup(const char *json_extension) {
    NIM_SDK_GET_FUNC(nim_client_cleanup)(json_extension);
}

void SdkClient::Login(const char *app_key, const char *account,
  const char *token, const char *json_extension,
  nim_json_transport_cb_func cb, const void *user_data) {
    NIM_SDK_GET_FUNC(nim_client_login)(app_key, account, token, json_extension, 
        cb, user_data);
}

int SdkClient::GetLoginState(const char *json_extension) {
    return NIM_SDK_GET_FUNC(nim_client_get_login_state)(json_extension);
}

void SdkClient::Relogin(const char *json_extension) {
    NIM_SDK_GET_FUNC(nim_client_relogin)(json_extension);
}

void SdkClient::Logout(enum NIMLogoutType logout_type,
  const char *json_extension, nim_json_transport_cb_func cb,
  const void *user_data) {
    NIM_SDK_GET_FUNC(nim_client_logout)(logout_type, json_extension, cb,
      user_data);
}

void SdkClient::KickOtherClient(const char *json_extension) {
    NIM_SDK_GET_FUNC(nim_client_kick_other_client)(json_extension);
}

void SdkClient::RegAutoReloginCb(const char *json_extension,
  nim_json_transport_cb_func cb, const void *user_data) {
    NIM_SDK_GET_FUNC(nim_client_reg_auto_relogin_cb)(json_extension, cb,
      user_data);
}

void SdkClient::RegKickoutCb(const char *json_extension,
  nim_json_transport_cb_func cb, const void *user_data) {
    NIM_SDK_GET_FUNC(nim_client_reg_kickout_cb)(json_extension, cb,
      user_data);
}

void SdkClient::RegDisconnectCb(const char *json_extension,
  nim_json_transport_cb_func cb, const void *user_data) {
    NIM_SDK_GET_FUNC(nim_client_reg_disconnect_cb)(json_extension, cb,
      user_data);
}

void SdkClient::RegMultispotLoginNotifyCb(const char *json_extension,
  nim_json_transport_cb_func cb, const void *user_data) {
    NIM_SDK_GET_FUNC(nim_client_reg_multispot_login_notify_cb)(json_extension, 
      cb, user_data);
}

void SdkClient::RegKickoutOtherClientCb(const char *json_extension,
  nim_json_transport_cb_func cb, const void *user_data) {
    NIM_SDK_GET_FUNC(nim_client_reg_kickout_other_client_cb)(json_extension,
      cb, user_data);
}

void SdkClient::RegSyncMultiportPushConfigCb(const char *json_extension,
  nim_client_multiport_push_config_cb_func cb, const void *user_data) {
    NIM_SDK_GET_FUNC(nim_client_reg_sync_multiport_push_config_cb)(json_extension,
      cb, user_data);
}

void SdkClient::SetMultiportPushConfig(const char *switch_content,
  const char *json_extension, nim_client_multiport_push_config_cb_func cb,
  const void *user_data) {
    NIM_SDK_GET_FUNC(nim_client_set_multiport_push_config)(switch_content,
        json_extension, cb, user_data);
}

void SdkClient::GetMultiportPushConfig(const char *json_extension,
  nim_client_multiport_push_config_cb_func cb, const void *user_data) {
    NIM_SDK_GET_FUNC(nim_client_get_multiport_push_config)(json_extension,
      cb, user_data);
}

} // namespace nim