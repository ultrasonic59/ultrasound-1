#ifndef NIM_SERVICE_CALLBACK_VCHAT_CALLBACK_H_
#define NIM_SERVICE_CALLBACK_VCHAT_CALLBACK_H_

#include <stdint.h>
#include "nim_sdk_api.h"

namespace nim {

class VChatCallback {
public:
    static void VChatCb(enum NIMVideoChatSessionType type, int64_t channel_id, 
      int code, const char *json_extension, const void *user_data);

    static void SetMemberInBlacklistCb(bool ret, int code, 
      const char *json_extension, const void *user_data);

    static void CreatRoomCb(int code, int64_t channel_id, 
      const char *json_extension, const void *user_data);

    static void JoinRoomCb(int code, int64_t channel_id,
      const char *json_extension, const void *user_data);

    static void SetCustomDataCb(bool ret, int code, const char *json_extension, 
      const void *user_data);
    
    static void SetVideoQualityCb(bool ret, int code, 
      const char *json_extension, const void *user_data);

    static void SetVideoBitrateCb(bool ret, int code,
      const char *json_extension, const void *user_data);

    static void NetDetectCb(bool ret, int code,
      const char *json_extension, const void *user_data);

    static void SetFrameRateCb(bool ret, int code,
      const char *json_extension, const void *user_data);

    static void StartRecordCb(bool ret, int code, const char *file, 
      int64_t time, const char *json_extension, const void *user_data);

    static void StopRecordCb(bool ret, int code, const char *file,
      int64_t time, const char *json_extension, const void *user_data);

    static void StartAudioRecordCb(bool ret, int code, const char *file, 
      int64_t time, const char *json_extension, const void *user_data);

    static void StopAudioRecordCb(bool ret, int code, const char *file,
      int64_t time, const char *json_extension, const void *user_data);

    static void UpdateRtmpUrlCb(bool ret, int code, const char *json_extension, 
      const void *user_data);

    static void SelectVideoAdaptiveStrategyCb(bool ret, int code, 
      const char *json_extension, const void *user_data);

    static void SetUidPictureAsMainCb(bool ret, int code,
        const char *json_extension, const void *user_data);
};

} // namespace nim

#endif // NIM_SERVICE_CALLBACK_VCHAT_CALLBACK_H_
