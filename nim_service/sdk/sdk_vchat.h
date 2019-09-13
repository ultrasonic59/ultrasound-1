#ifndef NIM_SERVICE_SDK_SDK_VCHAT_H_
#define NIM_SERVICE_SDK_SDK_VCHAT_H_

#include <stdint.h>
#include "nim_sdk_api.h"

namespace nim {

class SdkVChat {
public:
    static bool Init(const char *json_extension);

    static void Cleanup(const char *json_extension);

    static void SetCbFunc(nim_vchat_cb_func cb, const void *user_data);

    static bool Start(enum NIMVideoChatMode mode, const char *apns_text, 
      const char *custom_info, const char *json_extension, 
      const void *user_data);

    static bool CalleeAck(int64_t channel_id, bool accept, 
      const char *json_extension, const void *user_data);

    static bool Control(int64_t channel_id, 
      enum NIMVChatControlType type, const char *json_extension, 
      const void *user_data);

    static void SetViewerMode(bool viewer);

    static bool GetViewerMode();

    static void SetAudioMute(bool muted);

    static bool AudioMuteEnabled();

    static void SetMemberInBlacklist(const char *uid, bool add, bool audio, 
      const char *json_extension, nim_vchat_opt_cb_func cb, 
      const void *user_data);

    static void End(const char *json_extension);

    static void CreateRoom(const char *room_name, const char *custom_info, 
      const char *json_extension, nim_vchat_opt2_cb_func cb, 
      const void *user_data);

    static bool JoinRoom(enum NIMVideoChatMode mode, const char *room_name, 
      const char *json_extension, nim_vchat_opt2_cb_func cb, 
      const void *user_data);

    static void SetCustomData(bool custom_audio, bool custom_video, 
      const char *json_extension, nim_vchat_opt_cb_func cb, 
      const void *user_data);

    static bool SetTalkingMode(enum NIMVideoChatMode mode, 
      const char *json_extension);

    static void SetRotateRemoteVideo(bool rotate);

    static bool RotateRemoteVideoEnabled();

    static void SetVideoQuality(int video_quality, const char *json_extension, 
      nim_vchat_opt_cb_func cb, const void *user_data);

    static void SetVideoBitrate(int video_bitrate, const char *json_extension, 
      nim_vchat_opt_cb_func cb, const void *user_data);

    static uint64_t NetDetect(const char *json_extension, 
      nim_vchat_opt_cb_func cb, const void *user_data);

    static void SetFrameRate(enum NIMVChatVideoFrameRate frame_rate, 
      const char* json_extension, nim_vchat_opt_cb_func cb, 
      const void *user_data);

    static void SetVideoFrameScale(enum NIMVChatVideoFrameScaleType type);

    static int GetVideoFrameScaleType();

    static void StartRecord(const char *path, const char *json_extension, 
      nim_vchat_mp4_record_opt_cb_func cb, const void *user_data);

    static void StopRecord(const char *json_extension,
      nim_vchat_mp4_record_opt_cb_func cb, const void *user_data);

    static void StartAudioRecord(const char *path, const char *json_extension, 
      nim_vchat_audio_record_opt_cb_func cb, const void *user_data);

    static void StopAudioRecord(const char *json_extension, 
      nim_vchat_audio_record_opt_cb_func cb, const void *user_data);

    static void UpdateRtmpUrl(const char *rtmp_url, const char *json_extension, 
      nim_vchat_opt_cb_func cb, const void *user_data);

    static void SelectVideoAdaptiveStrategy(NIMVChatVideoEncodeMode mode, 
      const char *json_extension, nim_vchat_opt_cb_func cb, 
      const void *user_data);

    static void SetUidPictureAsMain(const char *uid, const char* json_extension, 
      nim_vchat_opt_cb_func cb, const void* user_data);

	static void SetProxy(enum NIMProxyType type, const char *host, int port, const char *user, const char *password);
};

} // namespace nim

#endif // NIM_SERVICE_SDK_SDK_VCHAT_H_


