#ifndef NIM_SERVICE_CALLBACK_DEVICE_CALLBACK_H_
#define NIM_SERVICE_CALLBACK_DEVICE_CALLBACK_H_

#include <stdint.h>
#include "nim_sdk_api.h"

namespace nim {

class DeviceCallback {
public:
    static void StartDeviceCb(enum NIMDeviceType type, bool ret, 
      const char *json_extension, const void *user_data);

    static void CaptureAudioDataCb(uint64_t time, const char *data, 
      uint32_t size, const char *json_extension, const void *user_data);

    static void ReceiveAudioDataCb(uint64_t time, const char *data, 
      uint32_t size, const char *json_extension, const void *user_data);

    static void CapturedVideoDataCb(uint64_t time, const char *data, 
      uint32_t size, uint32_t width, uint32_t height,
      const char *json_extension, const void *user_data);

    static void ReceivedVideoDataCb(uint64_t time, const char *data,
      uint32_t size, uint32_t width, uint32_t height,
      const char *json_extension, const void *user_data);

    static void EnumDeviceDevpathCb(bool ret, enum NIMDeviceType type, 
      const char *json_extension, const void *user_data);

    static void HookAudioDataCb(uint64_t time, const char *data, uint32_t size,
      int channels, int rate, int volume, const char *json_extension, 
      const void *user_data);

    static void HookAndMicAudioDataCb(uint64_t time, const char *data, 
      uint32_t size, int channels, int rate, int volume, 
      const char *json_extension, const void *user_data);

    static void DeviceStatusCb(enum NIMDeviceType type, uint32_t status, 
      const char *device_path, const char *json_extension, 
      const void *user_data);

    static void StartExtendCameraCb(enum NIMDeviceType type, bool ret, 
      const char *json_extension, const void *user_data);
};

} // namespace nim

#endif // NIM_SERVICE_CALLBACK_DEVICE_CALLBACK_H_