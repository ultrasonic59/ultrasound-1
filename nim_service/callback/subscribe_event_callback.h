#ifndef NIM_SERVICE_CALLBACK_SUBSCRIBE_EVENT_CALLBACK_H_
#define NIM_SERVICE_CALLBACK_SUBSCRIBE_EVENT_CALLBACK_H_

#include <stdint.h>
#include "nim_sdk_api.h"

namespace nim {

class SubscribeEventCallback {
public:
    static void PushEventCb(int res_code, const char *event_info_json,
      const char *json_extension, const void *user_data);

    static void BatchPushEventCb(int res_code, const char *event_list_json,
      const char *json_extension, const void *user_data);

    static void PublishEventCb(int res_code, int event_type,
      const char *event_info_json, const char *json_extension,
      const void *user_data);

    static void SubscribeEventCb(int res_code, int event_type,
      const char *faild_list_json, const char *json_extension,
      const void *user_data);

    static void UnsubscribeEventCb(int res_code, int event_type,
      const char *faild_list_json, const char *json_extension, 
      const void *user_data);

    static void BatchUnsubscribeEventCb(int res_code, int event_type,
      const char *json_extension, const void *user_data);

    static void QuerySubscribeEventCb(int res_code, int event_type,
      const char *subscribe_list_json, const char *json_extension, 
      const void *user_data);

    static void BatchQuerySubscribeEventCb(int res_code, int event_type,
      const char *subscribe_list_json, const char *json_extension, 
      const void *user_data);
};

} // namespace nim

#endif // NIM_SERVICE_CALLBACK_SUBSCRIBE_EVENT_CALLBACK_H_
