#ifndef NIM_SERVICE_SDK_SDK_SYSMSG_H_
#define NIM_SERVICE_SDK_SDK_SYSMSG_H_

#include <stdint.h>
#include "nim_sdk_api.h"

namespace nim {

class SdkSysMsg{
public:
    static void RegSysmsgCb(const char *json_extension, 
      nim_sysmsg_receive_cb_func cb, const void *user_data);

    static void SendCustomNotification(const char *json_msg, 
      const char *json_extension);

    static void RegCustomNotificationAckCb(const char *json_extension, 
      nim_custom_sysmsg_ack_cb_func cb, const void *user_data);

    static void QueryMsgAsync(int limit_count, int64_t last_time, 
      const char *json_extension, nim_sysmsg_query_cb_func cb, const void *user_data);

    static void QueryUnreadCount(const char *json_extension, nim_sysmsg_res_cb_func cb,
      const void *user_data);

    static void SetStatusAsync(int64_t msg_id, enum NIMSysMsgStatus status, 
      const char *json_extension, nim_sysmsg_res_ex_cb_func cb, const void *user_data);

    static void ReadAllAsync(const char *json_extension, nim_sysmsg_res_cb_func cb,
      const void *user_data);

    static void DeleteAsync(int64_t msg_id, const char *json_extension, 
      nim_sysmsg_res_ex_cb_func cb, const void *user_data);

    static void DeleteAllAsync(const char *json_extension, nim_sysmsg_res_cb_func cb, 
      const void *user_data);

    static void SetLogsStatusByTypeAsync(enum NIMSysMsgType type, enum NIMSysMsgStatus status, 
      const char *json_extension, nim_sysmsg_res_cb_func cb, const void *user_data);

    static void DeleteLogsByTypeAsync(enum NIMSysMsgType type, const char *json_extension, 
      nim_sysmsg_res_cb_func cb, const void *user_data);
};

} // namespace nim

#endif // NIM_SERVICE_SDK_SDK_SYSMSG_H_