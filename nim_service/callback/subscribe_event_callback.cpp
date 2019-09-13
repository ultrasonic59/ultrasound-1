#include "nim_service/callback/subscribe_event_callback.h"

namespace nim {

void SubscribeEventCallback::PushEventCb(int res_code,
  const char *event_info_json, const char *json_extension,
  const void *user_data) {

}

void SubscribeEventCallback::BatchPushEventCb(int res_code, 
  const char *event_list_json, const char *json_extension, 
  const void *user_data) {

}

void SubscribeEventCallback::PublishEventCb(int res_code, int event_type,
  const char *event_info_json, const char *json_extension,
  const void *user_data) {

}

void SubscribeEventCallback::SubscribeEventCb(int res_code, int event_type,
  const char *faild_list_json, const char *json_extension, 
  const void *user_data) {

}

void SubscribeEventCallback::UnsubscribeEventCb(int res_code, int event_type,
  const char *faild_list_json, const char *json_extension,
  const void *user_data) {

}

void SubscribeEventCallback::BatchUnsubscribeEventCb(int res_code, int event_type,
  const char *json_extension, const void *user_data) {

}

void SubscribeEventCallback::QuerySubscribeEventCb(int res_code, int event_type,
  const char *subscribe_list_json, const char *json_extension,
  const void *user_data) {

}

void SubscribeEventCallback::BatchQuerySubscribeEventCb(int res_code, int event_type,
  const char *subscribe_list_json, const char *json_extension,
  const void *user_data) {

}

} // namespace nim