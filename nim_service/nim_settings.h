#ifndef NIM_SERVICE_NIM_SETTINGS_H_
#define NIM_SERVICE_NIM_SETTINGS_H_

#include "nim_service/device_event_handler.h"
#include "nim_service/client_event_handler.h"

namespace nim {

class NimSettings {
public:
    virtual ~NimSettings() {}

    virtual DeviceEventHandler::DeviceInfo GetAudioInDeviceInfo() = 0; 
    virtual void SetAudioInDeviceInfo(
      const DeviceEventHandler::DeviceInfo& device_info) = 0;

    virtual DeviceEventHandler::DeviceInfo GetAudioOutDeviceInfo() = 0;
    virtual void SetAudioOutDeviceInfo(
      const DeviceEventHandler::DeviceInfo& device_info) = 0;

    virtual int GetAudioOutputVolume() = 0;
    virtual void SetAudioOutputVolume(int volume) = 0;

    virtual int GetAudioInputVolume() = 0;
    virtual void SetAudioInputVolume(int volume) = 0;

    virtual bool GetAudioInputAutoVolume() = 0;
    virtual void SetAudioInputAutoVolume(bool auto_volume) = 0;

	virtual ClientEventHandler::NetProxyInfo GetNetProxyInfo() = 0;
};

} // namespace nim

#endif // NIM_SERVICE_NIM_SETTINGS_H_