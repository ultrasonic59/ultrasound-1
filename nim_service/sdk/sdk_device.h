#ifndef NIM_SERVICE_SDK_SDK_DEVICE_H_
#define NIM_SERVICE_SDK_SDK_DEVICE_H_

#include <stdint.h>
#include "nim_sdk_api.h"

namespace nim {

class SdkDevice {
public:
    static void StartDevice(enum NIMDeviceType type, const char *device_path, 
      unsigned fps, const char *json_extension, 
      nim_vchat_start_device_cb_func cb, const void *user_data);

    static void EndDevice(enum NIMDeviceType type, const char *json_extension);

    static void SetAudioDataCb(bool capture, const char *json_extension, 
      nim_vchat_audio_data_cb_func cb, const void *user_data);

    static void SetVideoDataCb(bool capture, const char *json_extension,
        nim_vchat_video_data_cb_func cb, const void *user_data);

    static bool CustomAudioData(uint64_t time, const char *data, uint32_t size,
      const char *json_extension);

    static bool CustomVideoData(uint64_t time, const char *data, uint32_t size, 
      uint32_t width, uint32_t height, const char *json_extension);

    static void EnumDeviceDevpath(enum NIMDeviceType type, 
      const char *json_extension, nim_vchat_enum_device_devpath_sync_cb_func cb,
      const void *user_data);

    static bool AccompanyingSound(unsigned char id, unsigned __int64 time, 
      const char *data, unsigned int size, unsigned int rate,
      unsigned int channels, const char *json_extension);

    static void SetAudioDataCbEx(int type, const char *json_extension, 
      nim_vchat_audio_data_cb_func_ex cb, const void *user_data);

    static void AddDeviceStatusCb(enum NIMDeviceType type,
      nim_vchat_device_status_cb_func cb, const void *user_data);

    static void RemoveDeviceStatusCb(enum NIMDeviceType type);

    static void StartExtendCamera(const char *id, const char *device_path, 
      uint32_t fps, const char *json_extension, 
      nim_vchat_start_device_cb_func cb, const void *user_data);

    static void StopExtendCamera(const char *id, const char *json_extension);

    static void SetAudioVolumn(unsigned char volumn, bool capture);

    static unsigned char GetAudioVolumn(bool capture);

    static void SetAudioInputAutoVolumn(bool auto_volumn);

    static bool GetAudioInputAutoVolumn();

    static void SetAudioProcessInfo(bool aec, bool ns, bool vad);

    static void SetAudioHowlingSuppression(bool work);
};

} // namespace nim

#endif // NIM_SERVICE_SDK_SDK_DEVICE_H_


