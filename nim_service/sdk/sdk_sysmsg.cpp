#include "nim_service/sdk/sdk_sysmsg.h"

#include "nim_service/sdk/sdk_instance.h"

namespace nim {

typedef void(*nim_sysmsg_reg_sysmsg_cb)(const char *json_extension,
  nim_sysmsg_receive_cb_func cb, const void* user_data);
typedef void(*nim_sysmsg_query_msg_async)(int limit_count, int64_t last_time,
  const char *json_extension, nim_sysmsg_query_cb_func cb, const void* user_data);
typedef void(*nim_sysmsg_query_unread_count)(const char *json_extension,
  nim_sysmsg_res_cb_func cb, const void *user_data);
typedef void(*nim_sysmsg_set_status_async)(int64_t msg_id, NIMSysMsgStatus status,
  const char *json_extension, nim_sysmsg_res_ex_cb_func cb, const void* user_data);
typedef void(*nim_sysmsg_read_all_async)(const char *json_extension,
  nim_sysmsg_res_cb_func cb, const void* user_data);
typedef void(*nim_sysmsg_delete_async)(int64_t msg_id, const char *json_extension,
  nim_sysmsg_res_ex_cb_func cb, const void *user_data);
typedef void(*nim_sysmsg_delete_all_async)(const char *json_extension,
  nim_sysmsg_res_cb_func cb, const void *user_data);
typedef void(*nim_sysmsg_reg_custom_notification_ack_cb)(const char *json_extension,
  nim_custom_sysmsg_ack_cb_func cb, const void *user_data);
typedef void(*nim_sysmsg_send_custom_notification)(const char *json_msg,
  const char *json_extension);
typedef void(*nim_sysmsg_set_logs_status_by_type_async)(NIMSysMsgType type,
  NIMSysMsgStatus status, const char *json_extension, nim_sysmsg_res_cb_func cb, const void *user_data);
typedef void(*nim_sysmsg_delete_logs_by_type_async)(NIMSysMsgType type,
  const char *json_extension, nim_sysmsg_res_cb_func cb, const void *user_data);


void SdkSysMsg::RegSysmsgCb(const char *json_extension,
  nim_sysmsg_receive_cb_func cb, const void *user_data) {
    NIM_SDK_GET_FUNC(nim_sysmsg_reg_sysmsg_cb)(json_extension, cb, user_data);
}

void SdkSysMsg::SendCustomNotification(const char *json_msg, 
  const char *json_extension) {
    NIM_SDK_GET_FUNC(nim_sysmsg_send_custom_notification)(json_msg, json_extension);
}

void SdkSysMsg::RegCustomNotificationAckCb(const char *json_extension,
  nim_custom_sysmsg_ack_cb_func cb, const void *user_data) {
    NIM_SDK_GET_FUNC(nim_sysmsg_reg_custom_notification_ack_cb)(json_extension, cb, user_data);
}

void SdkSysMsg::QueryMsgAsync(int limit_count, int64_t last_time,
  const char *json_extension, nim_sysmsg_query_cb_func cb, const void *user_data) {
    NIM_SDK_GET_FUNC(nim_sysmsg_query_msg_async)(limit_count, last_time, json_extension, cb, user_data);
}

void SdkSysMsg::QueryUnreadCount(const char *json_extension, nim_sysmsg_res_cb_func cb,
  const void *user_data) {
    NIM_SDK_GET_FUNC(nim_sysmsg_query_unread_count)(json_extension, cb, user_data);
}

void SdkSysMsg::SetStatusAsync(int64_t msg_id, enum NIMSysMsgStatus status,
  const char *json_extension, nim_sysmsg_res_ex_cb_func cb, const void *user_data) {
    NIM_SDK_GET_FUNC(nim_sysmsg_set_status_async)(msg_id, status, json_extension, cb, user_data);
}

void SdkSysMsg::ReadAllAsync(const char *json_extension, nim_sysmsg_res_cb_func cb,
  const void *user_data) {
    NIM_SDK_GET_FUNC(nim_sysmsg_read_all_async)(json_extension, cb, user_data);
}

void SdkSysMsg::DeleteAsync(int64_t msg_id, const char *json_extension,
  nim_sysmsg_res_ex_cb_func cb, const void *user_data) {
    NIM_SDK_GET_FUNC(nim_sysmsg_delete_async)(msg_id, json_extension, cb, user_data);
}

void SdkSysMsg::DeleteAllAsync(const char *json_extension, nim_sysmsg_res_cb_func cb,
  const void *user_data) {
    NIM_SDK_GET_FUNC(nim_sysmsg_delete_all_async)(json_extension, cb, user_data);
}

void SdkSysMsg::SetLogsStatusByTypeAsync(enum NIMSysMsgType type, enum NIMSysMsgStatus status,
  const char *json_extension, nim_sysmsg_res_cb_func cb, const void *user_data) {
    NIM_SDK_GET_FUNC(nim_sysmsg_set_logs_status_by_type_async)(type, status, json_extension, cb, user_data);
}

void SdkSysMsg::DeleteLogsByTypeAsync(enum NIMSysMsgType type, const char *json_extension,
  nim_sysmsg_res_cb_func cb, const void *user_data) {
    NIM_SDK_GET_FUNC(nim_sysmsg_delete_logs_by_type_async)(type, json_extension, cb, user_data);
}

} // namespace nim