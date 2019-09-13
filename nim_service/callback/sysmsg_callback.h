#ifndef NIM_SERVICE_CALLBACK_SYSMSG_CALLBACK_H_
#define NIM_SERVICE_CALLBACK_SYSMSG_CALLBACK_H_

#include <stdint.h>
#include "nim_sdk_api.h"

namespace nim {

class SysMsgCallback {
public:
    static void ReceiveCb(const char *content, const char *json_extension, 
      const void *user_data);

    static void CustomSysmsgAckCb(const char *result, const void *user_data);

    static void QueryCb(int count, const char *result, const char *json_extension, 
      const void *user_data);

    static void ResCb(int res_code, int unread_count, const char *json_extension, 
      const void *user_data);

    static void ResExCb(int res_code, int64_t msg_id, int unread_count, 
      const char *json_extension, const void *user_data);

	static void ReceiveMessageCb(const char *content, const char *json_extension, const void *user_data);
};

} // namespace nim

#endif // NIM_SERVICE_CALLBACK_SYSMSG_CALLBACK_H_