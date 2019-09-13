#include "nim_service/callback/device_callback.h"

#include "json.h"

#include "nim_service/nim_service.h"

namespace nim {

void DeviceCallback::StartDeviceCb(enum NIMDeviceType type, bool ret,
  const char *json_extension, const void *user_data) {
    DeviceEventHandler::DeviceInfo device;
    device.type = (DeviceEventHandler::DeviceType)type;
    if (ret) {
        NimService::GetInstance()->OnDeviceStarted(device);
    }
    else {
        NimService::GetInstance()->OnDeviceStartError(device);
    }
}

void DeviceCallback::CaptureAudioDataCb(uint64_t time, const char *data,
    uint32_t size, const char *json_extension, const void *user_data) {

}

void DeviceCallback::ReceiveAudioDataCb(uint64_t time, const char *data,
    uint32_t size, const char *json_extension, const void *user_data) {

}

void DeviceCallback::CapturedVideoDataCb(uint64_t time, const char *data,
  uint32_t size, uint32_t width, uint32_t height,
  const char *json_extension, const void *user_data) {
    NimService::GetInstance()->OnCapturedVideoData(time, data, size, width,
        height);
}

void DeviceCallback::ReceivedVideoDataCb(uint64_t time, const char *data,
  uint32_t size, uint32_t width, uint32_t height,
  const char *json_extension, const void *user_data) {
    NimService::GetInstance()->OnReceivedVideoData(time, data, size, width, 
      height);
}

void DeviceCallback::EnumDeviceDevpathCb(bool ret, enum NIMDeviceType type,
  const char *json_extension, const void *user_data) {
    NimService::GetInstance()->m_device_info_list[type].clear();
    if (ret)
    {
        Json::Value value;
        Json::Reader reader;
        if (reader.parse(json_extension, value) && value.isArray())
        {
            int num = value.size();
            for (int i = 0; i < num; ++i)
            {
                Json::Value device;
                device = value.get(i, device);
                DeviceEventHandler::DeviceInfo info;
                info.type = (DeviceEventHandler::DeviceType)type;
                info.device_path = device[kNIMDevicePath].asString();
                info.friendly_name = device[kNIMDeviceName].asString();
                NimService::GetInstance()->m_device_info_list[type].push_back(
                  info);
            }
        }
    }
}

void DeviceCallback::HookAudioDataCb(uint64_t time, const char *data, 
  uint32_t size, int channels, int rate, int volume, const char *json_extension,
  const void *user_data) {

}

void DeviceCallback::HookAndMicAudioDataCb(uint64_t time, const char *data,
    uint32_t size, int channels, int rate, int volume,
    const char *json_extension, const void *user_data) {

}

void DeviceCallback::DeviceStatusCb(enum NIMDeviceType type, uint32_t status,
  const char *device_path, const char *json_extension, const void *user_data) {

    DeviceEventHandler::DeviceInfo device;
    device.type = (DeviceEventHandler::DeviceType)type;
    device.status = (DeviceEventHandler::DeviceStatus)status;
    device.device_path = device_path;

    NimService::GetInstance()->OnDeviceStatus(device);
}

void DeviceCallback::StartExtendCameraCb(enum NIMDeviceType type, bool ret,
    const char *json_extension, const void *user_data) {

}
    
} // namespace nim
