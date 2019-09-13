#ifndef NIM_SERVICE_DEVICE_EVENT_HANDLER_H_
#define NIM_SERVICE_DEVICE_EVENT_HANDLER_H_

#include <list>
#include <string>

namespace nim {

class DeviceEventHandler {
public:
    enum DeviceType {
        DeviceTypeAudioIn = 0,
        DeviceTypeAudioOut = 1,
        DeviceTypeAudioOutChat = 2,
        DeviceTypeVideo = 3,
        DeviceTypeMax = 4,
    };

    enum DeviceStatus {
        DeviceStatusNoChange = 0x0,
        DeviceStatusChange = 0x1,
        DeviceStatusWorkRemove = 0x2,
        DeviceStatusReset = 0x4,
        DeviceStatusStart = 0x8,
        DeviceStatusEnd = 0x10,
    };

    struct DeviceInfo {
        DeviceType type = DeviceTypeAudioIn;
        DeviceStatus status = DeviceStatusNoChange;
        std::string device_path;
        std::string friendly_name;
    };


    virtual ~DeviceEventHandler() {}

    virtual void OnDeviceStarted(const DeviceInfo& device) = 0;

    virtual void OnDeviceStartError(const DeviceInfo& device) = 0;

    virtual void OnDeviceStatus(const DeviceInfo& device) = 0;
};

} // namespace nim

#endif // NIM_SERVICE_DEVICE_EVENT_HANDLER_H_
