#include "nim_service/sdk/sdk_vchat.h"

#include "nim_service/sdk/sdk_instance.h"

namespace nim {

typedef bool(*nim_vchat_init)(const char *json_extension);
typedef void(*nim_vchat_cleanup)(const char *json_extension);
typedef void(*nim_vchat_set_cb_func)(nim_vchat_cb_func cb, 
  const void *user_data);
typedef bool(*nim_vchat_start)(enum NIMVideoChatMode mode, 
  const char *apns_text, const char *custom_info, const char *json_extension,
  const void *user_data);
typedef bool(*nim_vchat_callee_ack)(int64_t channel_id, bool accept,
  const char *json_extension, const void *user_data);
typedef bool(*nim_vchat_control)(int64_t channel_id, 
  enum NIMVChatControlType type, const char *json_extension, 
  const void *user_data);
typedef void(*nim_vchat_set_viewer_mode)(bool viewer);
typedef bool(*nim_vchat_get_viewer_mode)();
typedef void(*nim_vchat_set_audio_mute)(bool muted);
typedef bool(*nim_vchat_audio_mute_enabled)();
typedef void(*nim_vchat_set_member_in_blacklist)(const char *uid, bool add, 
  bool audio, const char *json_extension, nim_vchat_opt_cb_func cb, 
  const void *user_data);
typedef void(*nim_vchat_end)(const char *json_extension);
typedef void(*nim_vchat_create_room)(const char *room_name, 
  const char *custom_info, const char *json_extension, 
  nim_vchat_opt2_cb_func cb, const void *user_data);
typedef bool(*nim_vchat_join_room)(enum NIMVideoChatMode mode, 
  const char *room_name, const char *json_extension, nim_vchat_opt2_cb_func cb, 
  const void *user_data);
typedef void(*nim_vchat_set_custom_data)(bool custom_audio, bool custom_video, 
  const char *json_extension, nim_vchat_opt_cb_func cb, const void *user_data);
typedef bool(*nim_vchat_set_talking_mode)(enum NIMVideoChatMode mode, 
  const char *json_extension);
typedef void(*nim_vchat_set_rotate_remote_video)(bool rotate);
typedef bool(*nim_vchat_rotate_remote_video_enabled)();
typedef void(*nim_vchat_set_video_quality)(int video_quality,
  const char *json_extension, nim_vchat_opt_cb_func cb, const void *user_data);
typedef void(*nim_vchat_set_video_bitrate)(int video_bitrate, 
  const char *json_extension, nim_vchat_opt_cb_func cb, const void *user_data);
typedef uint64_t(*nim_vchat_net_detect)(const char *json_extension, 
  nim_vchat_opt_cb_func cb, const void *user_data);
typedef void(*nim_vchat_set_frame_rate)(enum NIMVChatVideoFrameRate frame_rate, 
  const char* json_extension, nim_vchat_opt_cb_func cb, const void *user_data);
typedef void(*nim_vchat_set_video_frame_scale)(
  enum NIMVChatVideoFrameScaleType type);
typedef int(*nim_vchat_get_video_frame_scale_type)();
typedef void(*nim_vchat_start_record)(const char *path, 
  const char *json_extension, nim_vchat_mp4_record_opt_cb_func cb, 
  const void *user_data);
typedef void(*nim_vchat_stop_record)(const char *json_extension, 
  nim_vchat_mp4_record_opt_cb_func cb, const void *user_data);
typedef void(*nim_vchat_start_audio_record)(const char *path, 
  const char *json_extension, nim_vchat_audio_record_opt_cb_func cb,
  const void *user_data);
typedef void(*nim_vchat_stop_audio_record)(const char *json_extension, 
  nim_vchat_audio_record_opt_cb_func cb, const void *user_data);
typedef void(*nim_vchat_update_rtmp_url)(const char *rtmp_url, 
  const char *json_extension, nim_vchat_opt_cb_func cb, const void *user_data);
typedef void(*nim_vchat_select_video_adaptive_strategy)(
  NIMVChatVideoEncodeMode mode, const char *json_extension, 
  nim_vchat_opt_cb_func cb, const void *user_data);
typedef void(*nim_vchat_set_uid_picture_as_main)(const char *uid, 
  const char* json_extension, nim_vchat_opt_cb_func cb, const void* user_data);
typedef void(*nim_vchat_set_proxy)(enum NIMProxyType type, const char *host, int port, const char *user, const char *password);


bool SdkVChat::Init(const char *json_extension) {
    return NIM_SDK_GET_FUNC(nim_vchat_init)(json_extension);
}

void SdkVChat::Cleanup(const char *json_extension) {
    NIM_SDK_GET_FUNC(nim_vchat_cleanup)(json_extension);
}

void SdkVChat::SetCbFunc(nim_vchat_cb_func cb, const void *user_data) {
    NIM_SDK_GET_FUNC(nim_vchat_set_cb_func)(cb, user_data);
}

bool SdkVChat::Start(enum NIMVideoChatMode mode, const char *apns_text,
  const char *custom_info, const char *json_extension,
  const void *user_data) {
    return NIM_SDK_GET_FUNC(nim_vchat_start)(mode, apns_text, custom_info, 
      json_extension, user_data);
}

bool SdkVChat::CalleeAck(int64_t channel_id, bool accept,
  const char *json_extension, const void *user_data) {
    return NIM_SDK_GET_FUNC(nim_vchat_callee_ack)(channel_id, accept,
      json_extension, user_data);
}

bool SdkVChat::Control(int64_t channel_id,
  enum NIMVChatControlType type, const char *json_extension,
  const void *user_data) {
    return NIM_SDK_GET_FUNC(nim_vchat_control)(channel_id, type,
      json_extension, user_data);
}

void SdkVChat::SetViewerMode(bool viewer) {
    NIM_SDK_GET_FUNC(nim_vchat_set_viewer_mode)(viewer);
}

bool SdkVChat::GetViewerMode() {
    return NIM_SDK_GET_FUNC(nim_vchat_get_viewer_mode)();
}

void SdkVChat::SetAudioMute(bool muted) {
    NIM_SDK_GET_FUNC(nim_vchat_set_audio_mute)(muted);
}

bool SdkVChat::AudioMuteEnabled() {
    return NIM_SDK_GET_FUNC(nim_vchat_audio_mute_enabled)();
}

void SdkVChat::SetMemberInBlacklist(const char *uid, bool add, bool audio,
  const char *json_extension, nim_vchat_opt_cb_func cb,
  const void *user_data) {
    NIM_SDK_GET_FUNC(nim_vchat_set_member_in_blacklist)(uid, add, audio, 
      json_extension, cb, user_data);
}

void SdkVChat::End(const char *json_extension) {
    NIM_SDK_GET_FUNC(nim_vchat_end)(json_extension);
}

void SdkVChat::CreateRoom(const char *room_name, const char *custom_info,
  const char *json_extension, nim_vchat_opt2_cb_func cb,
  const void *user_data) {
    NIM_SDK_GET_FUNC(nim_vchat_create_room)(room_name, custom_info, 
      json_extension, cb, user_data);
}

bool SdkVChat::JoinRoom(enum NIMVideoChatMode mode, const char *room_name,
  const char *json_extension, nim_vchat_opt2_cb_func cb,
  const void *user_data) {
    return  NIM_SDK_GET_FUNC(nim_vchat_join_room)(mode, room_name,
      json_extension, cb, user_data);
}

void SdkVChat::SetCustomData(bool custom_audio, bool custom_video,
  const char *json_extension, nim_vchat_opt_cb_func cb,
  const void *user_data) {
    NIM_SDK_GET_FUNC(nim_vchat_set_custom_data)(custom_audio, custom_video,
      json_extension, cb, user_data);
}

bool SdkVChat::SetTalkingMode(enum NIMVideoChatMode mode,
  const char *json_extension) {
    return NIM_SDK_GET_FUNC(nim_vchat_set_talking_mode)(mode, json_extension);
}

void SdkVChat::SetRotateRemoteVideo(bool rotate) {
    NIM_SDK_GET_FUNC(nim_vchat_set_rotate_remote_video)(rotate);
}

bool SdkVChat::RotateRemoteVideoEnabled() {
    return NIM_SDK_GET_FUNC(nim_vchat_rotate_remote_video_enabled)();
}

void SdkVChat::SetVideoQuality(int video_quality, const char *json_extension,
  nim_vchat_opt_cb_func cb, const void *user_data) {
    NIM_SDK_GET_FUNC(nim_vchat_set_video_quality)(video_quality, json_extension,
      cb, user_data);
}

void SdkVChat::SetVideoBitrate(int video_bitrate, const char *json_extension,
  nim_vchat_opt_cb_func cb, const void *user_data) {
    NIM_SDK_GET_FUNC(nim_vchat_set_video_bitrate)(video_bitrate, json_extension,
      cb, user_data);
}

uint64_t SdkVChat::NetDetect(const char *json_extension,
  nim_vchat_opt_cb_func cb, const void *user_data) {
    return NIM_SDK_GET_FUNC(nim_vchat_net_detect)(json_extension, cb,
      user_data);
}

void SdkVChat::SetFrameRate(enum NIMVChatVideoFrameRate frame_rate,
  const char* json_extension, nim_vchat_opt_cb_func cb,
  const void *user_data) {
    NIM_SDK_GET_FUNC(nim_vchat_set_frame_rate)(frame_rate, json_extension, cb,
        user_data);
}

void SdkVChat::SetVideoFrameScale(enum NIMVChatVideoFrameScaleType type) {
    NIM_SDK_GET_FUNC(nim_vchat_set_video_frame_scale)(type);
}

int SdkVChat::GetVideoFrameScaleType() {
    return NIM_SDK_GET_FUNC(nim_vchat_get_video_frame_scale_type)();
}

void SdkVChat::StartRecord(const char *path, const char *json_extension,
  nim_vchat_mp4_record_opt_cb_func cb, const void *user_data) {
    NIM_SDK_GET_FUNC(nim_vchat_start_record)(path, json_extension, cb, 
      user_data);
}

void SdkVChat::StopRecord(const char *json_extension,
  nim_vchat_mp4_record_opt_cb_func cb, const void *user_data) {
    NIM_SDK_GET_FUNC(nim_vchat_stop_record)(json_extension, cb, user_data);
}

void SdkVChat::StartAudioRecord(const char *path, const char *json_extension,
  nim_vchat_audio_record_opt_cb_func cb, const void *user_data) {
    NIM_SDK_GET_FUNC(nim_vchat_start_audio_record)(path, json_extension, cb,
      user_data);
}

void SdkVChat::StopAudioRecord(const char *json_extension,
  nim_vchat_audio_record_opt_cb_func cb, const void *user_data) {
    NIM_SDK_GET_FUNC(nim_vchat_stop_audio_record)(json_extension, cb, 
      user_data);
}

void SdkVChat::UpdateRtmpUrl(const char *rtmp_url, const char *json_extension,
  nim_vchat_opt_cb_func cb, const void *user_data) {
    NIM_SDK_GET_FUNC(nim_vchat_update_rtmp_url)(rtmp_url, json_extension, cb,
        user_data);
}

void SdkVChat::SelectVideoAdaptiveStrategy(NIMVChatVideoEncodeMode mode,
  const char *json_extension, nim_vchat_opt_cb_func cb,
  const void *user_data) {
    NIM_SDK_GET_FUNC(nim_vchat_select_video_adaptive_strategy)(mode, 
      json_extension, cb, user_data);
}

void SdkVChat::SetUidPictureAsMain(const char *uid, const char* json_extension,
  nim_vchat_opt_cb_func cb, const void* user_data) {
    NIM_SDK_GET_FUNC(nim_vchat_set_uid_picture_as_main)(uid, json_extension, 
      cb, user_data);
}

void SdkVChat::SetProxy(enum NIMProxyType type, const char *host, int port, const char *user, const char *password) {
	NIM_SDK_GET_FUNC(nim_vchat_set_proxy)(type, host, port, user, password);
}



} // namespace nim