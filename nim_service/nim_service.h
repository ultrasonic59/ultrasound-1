#ifndef NIM_SERVICE_NIM_SERVICE_H_
#define NIM_SERVICE_NIM_SERVICE_H_

#include <atomic>
#include <list>
#include <mutex>
#include <string>
#include <vector>
#include <windows.h>

#include "base/macros.h"
#include "nim_service/nim_settings.h"
#include "nim_service/client_event_handler.h"
#include "nim_service/vchat_event_handler.h"
#include "nim_service/device_event_handler.h"
#include "nim_service/sysmsg_event_handler.h"

namespace nim {

class NimService : public ClientEventHandler, 
                   public VChatEventHandler,
                   public DeviceEventHandler,
				   public SysMsgEventHandler {
public:
    SINGLETON_DEFINE(NimService);

    struct InitParams {
        bool is_demo = false;
        bool custom_video = true;
        bool r_host_speaker = true;
        bool video_record = true;
		std::string log_path;
    };

	bool Initialize(NimSettings* settings, const InitParams& params);
	void Shutdown();
    bool IsDemo() { return m_init_params.is_demo; }

	void ClientLogin(const std::string &account, const std::string &password);
	void ClientLogout();
    bool IsClientLogined() { return m_is_logined; }
    std::string GetCurrentAccount() { return m_account; }

    bool VChatStart(const std::string& apns_text,
      const std::string& custom_info, 
      const std::string& uid, 
      const std::string& session_id);
    void VChatEnd(const std::string& session_id);
    bool IsVChatConnected() { return m_is_vchat_connected; }
    bool VChatCalleeAck(bool accept, const std::string& session_id);
    bool VChatControl(ControlType type);
    void VChatSetVideoBitrate(int bitrate);
    int64_t VChatGetChannelId() { return m_channel_id; }
    std::string VChatGetUid() { return m_uid; }

    unsigned char GetMicAudioVolumn();
    void SetMicAudioVolumn(unsigned char volumn);
    unsigned char GetSpeakerAudioVolumn();
    void SetSpeakerAudioVolumn(unsigned char volumn);
    bool GetMicAutoVolumn();
    void SetMicAutoVolumn(bool auto_volumn);
    bool CustomVideoDataI420(uint8_t* data, int size, int width, int height);
    std::vector<DeviceInfo> EnumDevice(DeviceType type);
    void StartDevice(const DeviceInfo& device);
    void EndDevice(const DeviceInfo& device);
    const DeviceInfo GetDefaultDeviceInfo(DeviceType type);
    void SetDefaultDeviceInfo(DeviceType type, const DeviceInfo& info);
    void StartAudioDevice();
    void StopAudioDevice();
	void SendSystemMessage(const SystemMessage& sys_msg);
    
    void AddClientEventHandler(ClientEventHandler* handler);
    void RemoveClientEventHandler(ClientEventHandler* handler);
    void AddVChatEventHandler(VChatEventHandler* handler);
    void RemoveVChatEventHandler(VChatEventHandler* handler);
    void AddDeviceEventHandler(DeviceEventHandler* handler);
    void RemoveDeviceEventHandler(DeviceEventHandler* handler);
	void AddSysMsgEventHandler(SysMsgEventHandler* handler);
	void RemoveSysMsgEventHandler(SysMsgEventHandler* handler);

    // ClientEventHandler
    void OnLogin(const std::list<OtherClientInfo>& clients) override;
    void OnLogout() override;
    void OnLoginInfo(Info info) override;
    void OnLoginError(Error error, int code, const std::string& message) override;
    void OnKickOut(ClientType client_type, KickReason kick_reason) override;

    // VideoChatEventHander
    void OnVChatChannelCreated(const int64_t channel_id) override;
    void OnVChatStartError(int code) override;
    void OnVChatConnected(const int64_t channel_id) override;
    void OnVChatConnectLogout() override;
    void OnVChatConnectError(const int code) override;
    void OnCalleeRejected() override;
    void OnControlNotify(const std::string& uid, ControlType type) override;
    void OnReceivedVChatInvite(const int64_t channel_id, const std::string& uid,
      const std::string& custom_info) override;
    void OnHangup() override;
    void OnCapturedVideoData(unsigned __int64 time, const char* data,
        unsigned int size, unsigned int width, unsigned int height) override;
    void OnReceivedVideoData(unsigned __int64 time, const char* data,
      unsigned int size, unsigned int width, unsigned int height) override;
    void OnRealtimeStatsInfo(const StatsInfo& info) override;
    void OnSessionNetStatus(const std::string& uid, 
      const SessionNetStatus status) override;
    void OnVChatOptError(const OptType type, const int code) override;
    void OnAudioVolumeNotify(const int32_t self_volume,
      const int32_t other_volume) override;
    void OnVchatSyncNotify(bool accept) override;

    // DeviceEventHandler
    void OnDeviceStarted(const DeviceInfo& device) override;
    void OnDeviceStartError(const DeviceInfo& device) override;
    void OnDeviceStatus(const DeviceInfo& device) override;

	// Í¨¹ý SysMsgEventHandler ¼Ì³Ð
	virtual void OnReceiveMessage(const SystemMessage& sys_msg) override;

private:
    NimService() = default;
    ~NimService() = default;

    void RemoveAllEventHandlers();

    bool ClientInit(const std::string& app_key, const std::string& app_data_dir,
      const std::string& app_install_dir);

    void ClientCleanup();

    bool VChatInit();
    void VChatCleanup();

    std::list<DeviceInfo> NimService::GetDeviceInfo(DeviceType type);
    bool GetDefaultDevicePath(int &no, std::string &device_path,
        DeviceType type);
    void SetDefaultDevicePath(std::string device_path, DeviceType type);

    InitParams m_init_params;
    bool m_initialized = false;
    std::string m_account;
    std::string m_password;
    std::atomic_bool m_is_logined = false;
    
    int64_t m_channel_id = 0;
    std::string m_session_id;
    std::string m_uid;
    std::atomic_bool m_is_vchat_connected = false;

    friend class DeviceCallback;
    std::list<DeviceInfo> m_device_info_list[DeviceTypeMax];
    DeviceInfo m_default_device_info[DeviceTypeMax];

    std::mutex m_client_handlers_mutex;
    std::mutex m_vchat_handlers_mutex;
    std::mutex m_device_handlers_mutex;
	std::mutex m_sysmsg_handlers_mutex;
    std::list<ClientEventHandler*> m_client_handlers;
    std::list<VChatEventHandler*> m_vchat_handlers;
    std::list<DeviceEventHandler*> m_device_handlers;
	std::list<SysMsgEventHandler*> m_sysmsg_handlers;
	

    HANDLE m_client_logout_event;
    HANDLE m_vchat_logout_event;

    NimSettings* m_settings;
};

} // namespace nim

#endif // NIM_SERVICE_NIM_SERVICE_H_
