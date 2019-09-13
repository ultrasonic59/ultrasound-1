#include "nim_service/sdk/sdk_subscribe_event.h"

#include "nim_service/sdk/sdk_instance.h"

namespace nim {

typedef void(*nim_subscribe_event_reg_push_event_cb)(const char *json_extension, 
  nim_push_event_cb_func cb, const void *user_data);
typedef void(*nim_subscribe_event_reg_batch_push_event_cb)(const char *json_extension, 
  nim_batch_push_event_cb_func cb, const void *user_data);
typedef void(*nim_publish_event)(const char *event_json, const char *json_extension, 
  nim_publish_event_cb_func cb, const void *user_data);
typedef void(*nim_subscribe_event)(int event_type, int64_t ttl, int sync_event, 
  const char *accid_list_json, const char *json_extension, 
  nim_subscribe_event_cb_func cb, const void *user_data);
typedef void(*nim_unsubscribe_event)(int event_type, const char *accid_list_json, 
  const char *json_extension, nim_unsubscribe_event_cb_func cb, const void *user_data);
typedef void(*nim_batch_unsubscribe_event)(int event_type, const char *json_extension, 
  nim_batch_unsubscribe_event_cb_func cb, const void *user_data);
typedef void(*nim_query_subscribe_event)(int event_type, const char *accid_list_json,
  const char *json_extension, nim_query_subscribe_event_cb_func cb, const void *user_data);
typedef void(*nim_batch_query_subscribe_event)(int event_type, const char *json_extension,
  nim_batch_query_subscribe_event_cb_func cb, const void *user_data);


void SdkSubscribeEvent::RegPushEventCb(const char *json_extension,
  nim_push_event_cb_func cb, const void *user_data) {
    NIM_SDK_GET_FUNC(nim_subscribe_event_reg_push_event_cb)(json_extension, cb, 
      user_data);
}

void SdkSubscribeEvent::RegBatchPushEventCb(const char *json_extension,
  nim_batch_push_event_cb_func cb, const void *user_data) {
    NIM_SDK_GET_FUNC(nim_subscribe_event_reg_batch_push_event_cb)(json_extension, cb, user_data);
}

void SdkSubscribeEvent::PublishEvent(const char *event_json, const char *json_extension,
  nim_publish_event_cb_func cb, const void *user_data) {
    NIM_SDK_GET_FUNC(nim_publish_event)(event_json, json_extension, cb, user_data);
}

void SdkSubscribeEvent::SubscribeEvent(int event_type, int64_t ttl, int sync_event,
  const char *accid_list_json, const char *json_extension,
  nim_subscribe_event_cb_func cb, const void *user_data) {
    NIM_SDK_GET_FUNC(nim_subscribe_event)(event_type, ttl, sync_event,
      accid_list_json, json_extension, cb, user_data);
}

void SdkSubscribeEvent::UnsubscribeEvent(int event_type,
  const char *accid_list_json, const char *json_extension,
  nim_unsubscribe_event_cb_func cb, const void *user_data) {
    NIM_SDK_GET_FUNC(nim_unsubscribe_event)(event_type, accid_list_json, 
      json_extension, cb, user_data);
}

void SdkSubscribeEvent::BatchUnsubscribeEvent(int event_type,
  const char *json_extension, nim_batch_unsubscribe_event_cb_func cb,
  const void *user_data) {
    NIM_SDK_GET_FUNC(nim_batch_unsubscribe_event)(event_type, json_extension, cb, user_data);
}

void SdkSubscribeEvent::QuerySubscribeEvent(int event_type, const char *accid_list_json,
  const char *json_extension, nim_query_subscribe_event_cb_func cb,
  const void *user_data) {
    NIM_SDK_GET_FUNC(nim_query_subscribe_event)(event_type, accid_list_json, 
      json_extension, cb, user_data);
}

void SdkSubscribeEvent::BatchQuerySubscribeEvent(int event_type,
  const char *json_extension, nim_batch_query_subscribe_event_cb_func cb,
  const void *user_data) {
    NIM_SDK_GET_FUNC(nim_batch_query_subscribe_event)(event_type, json_extension, 
      cb, user_data);
}

} // namespace nim
