#include "nim_service/callback/sysmsg_callback.h"

#include "json.h"
#include "nim_service/nim_service.h"

namespace nim {

void SysMsgCallback::ReceiveCb(const char *content, const char *json_extension,
    const void *user_data) {

}

void SysMsgCallback::CustomSysmsgAckCb(const char *result, const void *user_data) {

}

void SysMsgCallback::QueryCb(int count, const char *result, const char *json_extension,
    const void *user_data) {

}

void SysMsgCallback::ResCb(int res_code, int unread_count, const char *json_extension,
    const void *user_data) {

}

void SysMsgCallback::ResExCb(int res_code, int64_t msg_id, int unread_count,
    const char *json_extension, const void *user_data) {

}

void SysMsgCallback::ReceiveMessageCb(const char * content, const char * json_extension, const void * user_data) {
	Json::Reader reader;
	Json::Value value;
	if (reader.parse(content, value) && value.isObject()) {
		Json::Value content_object = value[kNIMSysMsgKeyLocalContent];
		if (content_object.isObject()) {
			SystemMessage sys_msg;
			sys_msg.receive_uid = content_object[kNIMSysMsgKeyToAccount].asString();
			sys_msg.send_uid = content_object[kNIMSysMsgKeyFromAccount].asString();
			sys_msg.message = content_object[kNIMSysMsgKeyAttach].asString();

			NimService::GetInstance()->OnReceiveMessage(sys_msg);
		}
	}
}

} // namespace nim