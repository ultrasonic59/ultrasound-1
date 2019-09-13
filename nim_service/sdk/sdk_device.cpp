#include "nim_service/sdk/sdk_device.h"

#include "nim_service/sdk/sdk_instance.h"

namespace nim {

typedef void(*nim_vchat_start_device)(enum NIMDeviceType type, 
  const char *device_path, unsigned fps, const char *json_extension, 
  nim_vchat_start_device_cb_func cb, const void *user_data);
typedef void(*nim_vchat_end_device)(enum NIMDeviceType type, 
  const char *json_extension);
typedef void(*nim_vchat_set_audio_data_cb)(bool capture, 
  const char *json_extension, nim_vchat_audio_data_cb_func cb, 
  const void *user_data);
typedef bool(*nim_vchat_custom_audio_data)(uint64_t time, 
  const char *data, uint32_t size, const char *json_extension);
typedef bool(*nim_vchat_custom_video_data)(uint64_t time, const char *data, 
  uint32_t size, uint32_t width, uint32_t height, const char *json_extension);
typedef void(*nim_vchat_set_video_data_cb)(bool capture, 
  const char *json_extension, nim_vchat_video_data_cb_func cb, 
  const void *user_data);
typedef void(*nim_vchat_enum_device_devpath)(enum NIMDeviceType type,
  const char *json_extension, nim_vchat_enum_device_devpath_sync_cb_func cb,
  const void *user_data);
typedef bool(*nim_vchat_accompanying_sound)(unsigned char id, 
  unsigned __int64 time, const char *data, unsigned int size, unsigned int rate, 
  unsigned int channels, const char *json_extension);
typedef void(*nim_vchat_set_audio_data_cb_ex)(int type, 
  const char *json_extension, nim_vchat_audio_data_cb_func_ex cb,
  const void *user_data);
typedef void(*nim_vchat_add_device_status_cb)(enum NIMDeviceType type, 
  nim_vchat_device_status_cb_func cb, const void *user_data);
typedef void(*nim_vchat_remove_device_status_cb)(enum NIMDeviceType type);
typedef void(*nim_vchat_start_extend_camera)(const char *id,
  const char *device_path, uint32_t fps, const char *json_extension, 
  nim_vchat_start_device_cb_func cb, const void *user_data);
typedef void(*nim_vchat_stop_extend_camera)(const char *id, 
  const char *json_extension);
typedef void(*nim_vchat_set_audio_volumn)(unsigned char volumn, bool capture);
typedef unsigned char(*nim_vchat_get_audio_volumn)(bool capture);
typedef void(*nim_vchat_set_audio_input_auto_volumn)(bool auto_volumn);
typedef bool(*nim_vchat_get_audio_input_auto_volumn)();
typedef void(*nim_vchat_set_audio_process_info)(bool aec, bool ns, bool vad);
typedef void(*nim_vchat_set_audio_howling_suppression)(bool work);


void SdkDevice::StartDevice(enum NIMDeviceType type, const char *device_path,
  unsigned fps, const char *json_extension,
  nim_vchat_start_device_cb_func cb, const void *user_data) {
    NIM_SDK_GET_FUNC(nim_vchat_start_device)(type, device_path, fps, 
      json_extension, cb, user_data);
}

void SdkDevice::EndDevice(enum NIMDeviceType type, const char *json_extension) {
    NIM_SDK_GET_FUNC(nim_vchat_end_device)(type, json_extension);
}

void SdkDevice::SetAudioDataCb(bool capture, const char *json_extension,
  nim_vchat_audio_data_cb_func cb, const void *user_data) {
    NIM_SDK_GET_FUNC(nim_vchat_set_audio_data_cb)(capture, json_extension, cb, 
      user_data);
}

bool SdkDevice::CustomAudioData(uint64_t time, const char *data, uint32_t size,
  const char *json_extension) {
    return NIM_SDK_GET_FUNC(nim_vchat_custom_audio_data)(time, data, size,
      json_extension);
}

bool SdkDevice::CustomVideoData(uint64_t time, const char *data, uint32_t size,
  uint32_t width, uint32_t height, const char *json_extension) {
    return NIM_SDK_GET_FUNC(nim_vchat_custom_video_data)(time, data, size,
      width, height, json_extension);
}

void SdkDevice::SetVideoDataCb(bool capture, const char *json_extension,
  nim_vchat_video_data_cb_func cb, const void *user_data) {
    NIM_SDK_GET_FUNC(nim_vchat_set_video_data_cb)(capture, json_extension, cb, 
      user_data);
}

void SdkDevice::EnumDeviceDevpath(enum NIMDeviceType type,
  const char *json_extension, nim_vchat_enum_device_devpath_sync_cb_func cb,
  const void *user_data) {
    NIM_SDK_GET_FUNC(nim_vchat_enum_device_devpath)(type, json_extension, cb,
      user_data);
}

bool SdkDevice::AccompanyingSound(unsigned char id, unsigned __int64 time,
  const char *data, unsigned int size, unsigned int rate,
  unsigned int channels, const char *json_extension) {
    return NIM_SDK_GET_FUNC(nim_vchat_accompanying_sound)( id, time, data, 
      size, rate, channels, json_extension);
}

void SdkDevice::SetAudioDataCbEx(int type, const char *json_extension,
  nim_vchat_audio_data_cb_func_ex cb, const void *user_data) {
    NIM_SDK_GET_FUNC(nim_vchat_set_audio_data_cb_ex)(type, json_extension, cb, 
      user_data);
}

void SdkDevice::AddDeviceStatusCb(enum NIMDeviceType type,
  nim_vchat_device_status_cb_func cb, const void *user_data) {
    NIM_SDK_GET_FUNC(nim_vchat_add_device_status_cb)(type, cb, user_data);
}

void SdkDevice::RemoveDeviceStatusCb(enum NIMDeviceType type) {
    NIM_SDK_GET_FUNC(nim_vchat_remove_device_status_cb)(type);
}

void SdkDevice::StartExtendCamera(const char *id, const char *device_path,
  uint32_t fps, const char *json_extension,
  nim_vchat_start_device_cb_func cb, const void *user_data) {
    NIM_SDK_GET_FUNC(nim_vchat_start_extend_camera)(id, device_path, fps, 
      json_extension, cb, user_data);
}

void SdkDevice::StopExtendCamera(const char *id, const char *json_extension) {
    NIM_SDK_GET_FUNC(nim_vchat_stop_extend_camera)(id, json_extension);
}

void SdkDevice::SetAudioVolumn(unsigned char volumn, bool capture) {
    NIM_SDK_GET_FUNC(nim_vchat_set_audio_volumn)(volumn, capture);
}

unsigned char SdkDevice::GetAudioVolumn(bool capture) {
    return NIM_SDK_GET_FUNC(nim_vchat_get_audio_volumn)(capture);
}

void SdkDevice::SetAudioInputAutoVolumn(bool auto_volumn) {
    NIM_SDK_GET_FUNC(nim_vchat_set_audio_input_auto_volumn)(auto_volumn);
}

bool SdkDevice::GetAudioInputAutoVolumn() {
    return NIM_SDK_GET_FUNC(nim_vchat_get_audio_input_auto_volumn)();
}

void SdkDevice::SetAudioProcessInfo(bool aec, bool ns, bool vad) {
    NIM_SDK_GET_FUNC(nim_vchat_set_audio_process_info)(aec, ns, vad);
}

void SdkDevice::SetAudioHowlingSuppression(bool work) {
    NIM_SDK_GET_FUNC(nim_vchat_set_audio_howling_suppression)(work);
}

} // namespace nim