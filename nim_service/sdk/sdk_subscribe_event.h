#ifndef NIM_SERVICE_SDK_SDK_SUBSCRIBE_EVENT_H_
#define NIM_SERVICE_SDK_SDK_SUBSCRIBE_EVENT_H_

#include <stdint.h>
#include "nim_sdk_api.h"
#include "nim_subscribe_event.h"

namespace nim {

class SdkSubscribeEvent {
public:
    static void RegPushEventCb(const char *json_extension, 
      nim_push_event_cb_func cb, const void *user_data);

    static void RegBatchPushEventCb(const char *json_extension, 
      nim_batch_push_event_cb_func cb, const void *user_data);

    static void PublishEvent(const char *event_json, const char *json_extension, 
      nim_publish_event_cb_func cb, const void *user_data);

    static void SubscribeEvent(int event_type, int64_t ttl, int sync_event, 
      const char *accid_list_json, const char *json_extension, 
      nim_subscribe_event_cb_func cb, const void *user_data);

    static void UnsubscribeEvent(int event_type, 
      const char *accid_list_json, const char *json_extension, 
      nim_unsubscribe_event_cb_func cb, const void *user_data);

    static void BatchUnsubscribeEvent(int event_type, 
      const char *json_extension, nim_batch_unsubscribe_event_cb_func cb, 
      const void *user_data);

    static void QuerySubscribeEvent(int event_type, const char *accid_list_json, 
      const char *json_extension, nim_query_subscribe_event_cb_func cb, 
      const void *user_data);

    static void BatchQuerySubscribeEvent(int event_type, 
      const char *json_extension, nim_batch_query_subscribe_event_cb_func cb, 
      const void *user_data);
};

} // namespace nim

#endif // NIM_SERVICE_SDK_SDK_SUBSCRIBE_EVENT_H_