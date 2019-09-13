#include "nim_service/nim_service.h"

#include "json.h"

#include "base/json/json_util.h"
#include "nim_service/callback/client_callback.h"
#include "nim_service/callback/device_callback.h"
#include "nim_service/callback/vchat_callback.h"
#include "nim_service/callback/sysmsg_callback.h"
#include "nim_service/sdk/sdk_client.h"
#include "nim_service/sdk/sdk_global.h"
#include "nim_service/sdk/sdk_device.h"
#include "nim_service/sdk/sdk_instance.h"
#include "nim_service/sdk/sdk_vchat.h"
#include "nim_service/sdk/sdk_sysmsg.h"
#include "nim_sysmsg_def.h"
#include "nim_sysmsg.h"

namespace nim {

static const char kSdkNimDll[] = "nim.dll";
static const char kRealAppKey[] = "3a28de2f550386be38ae1acd1254293d";
static const char kDemoAppKey[] = "45c6af3c98409b18a84451215d0bdd6e";
static const char kDatabaseEncryptKey[] = "ultrasound";

SdkInstance *g_nim_sdk_instance = NULL;

bool NimService::Initialize(NimSettings* settings, const InitParams& params) {
    if (m_initialized)
        return m_initialized;

    m_settings = settings;
    assert(m_settings);

    m_init_params = params;

    bool res = false;
    if(m_init_params.is_demo)
        res = ClientInit(kDemoAppKey, m_init_params.log_path, "");
    else
        res = ClientInit(kRealAppKey, m_init_params.log_path, "");

    if (!res)
        return res;


	if (m_settings->GetNetProxyInfo().nim.enabled) {
		SdkGlobal::setProxy(NIMProxyType::kNIMProxySocks5,
			m_settings->GetNetProxyInfo().nim.host.c_str(),
			m_settings->GetNetProxyInfo().nim.port, 
			m_settings->GetNetProxyInfo().nim.username.c_str(), 
			m_settings->GetNetProxyInfo().nim.password.c_str());
	}

    SdkClient::RegKickoutCb(nullptr, ClientCallback::KickoutCb, this);

    res = VChatInit();
    if (!res)
        return res;

    // If TRUE, the initial state is signaled; otherwise, it is nonsignaled.
    m_client_logout_event = CreateEvent(NULL, TRUE, TRUE, NULL);
    m_vchat_logout_event = CreateEvent(NULL, TRUE, TRUE, NULL);

    SetDefaultDeviceInfo(DeviceEventHandler::DeviceTypeAudioIn, 
      m_settings->GetAudioInDeviceInfo());
    SetMicAutoVolumn(m_settings->GetAudioInputAutoVolume());
    SetMicAudioVolumn(m_settings->GetAudioInputVolume());
    SetDefaultDeviceInfo(DeviceEventHandler::DeviceTypeAudioOut, 
      m_settings->GetAudioOutDeviceInfo());
    SetSpeakerAudioVolumn(m_settings->GetAudioOutputVolume());
    
    m_initialized = true;

    return res;
}

void NimService::Shutdown() {
    RemoveAllEventHandlers();

    if (!m_initialized)
        return;

    m_settings->SetAudioInDeviceInfo(GetDefaultDeviceInfo(DeviceEventHandler::DeviceTypeAudioIn));
    m_settings->SetAudioInputAutoVolume(GetMicAutoVolumn());
    m_settings->SetAudioInputVolume(GetMicAudioVolumn());
    m_settings->SetAudioOutDeviceInfo(GetDefaultDeviceInfo(DeviceEventHandler::DeviceTypeAudioOut));
    m_settings->SetAudioOutputVolume(GetSpeakerAudioVolumn());
    
    VChatEnd("");

    WaitForSingleObject(m_vchat_logout_event, INFINITE);
    
    VChatCleanup();

    ClientLogout();

    WaitForSingleObject(m_client_logout_event, INFINITE);

    ClientCleanup();

    CloseHandle(m_client_logout_event);
    CloseHandle(m_vchat_logout_event);

    m_initialized = false;
}

void NimService::ClientLogin(const std::string &account,  
  const std::string &password) {
    
    m_account = account;
    m_password = password;

    if(m_init_params.is_demo)
        SdkClient::Login(kDemoAppKey, account.c_str(), password.c_str(), 
          nullptr, ClientCallback::LoginCb, this);
    else
        SdkClient::Login(kRealAppKey, account.c_str(), password.c_str(),
          nullptr, ClientCallback::LoginCb, this);
}

void NimService::ClientLogout() {
    SdkClient::Logout(kNIMLogoutAppExit, nullptr, ClientCallback::LogoutCb, 
      this);
}

bool NimService::VChatStart(const std::string& apns_text, 
  const std::string& custom_info,
  const std::string& uid,
  const std::string& session_id) {
    m_session_id = session_id;
    m_uid = uid;

    Json::FastWriter fs;
    Json::Value value;
    value[kNIMVChatUids].append(uid);
    value[kNIMVChatSessionId] = session_id;
    value[kNIMVChatCustomVideo] = m_init_params.custom_video ? 1 : 0;
    value[kNIMVChatRecord] = 1;
    value[kNIMVChatVideoRecord] = m_init_params.video_record ? 1 : 0;;
    value[kNIMVChatRecordType] = 0;
    value[kNIMVChatRHostSpeaker] = m_init_params.r_host_speaker ? 1 : 0;
    value[kNIMVChatVideoQuality] = kNIMVChatVideoQuality720p;
    value[kNIMVChatVideoFrameRate] = kNIMVChatVideoFrameRate25;
    //value[kNIMVChatAudioHighRate] = 1;
    //value[kNIMVChatAudioMode] = 1;
    value[kNIMVChatNeedBadge] = 0;
    //value[kNIMVChatApnsPayload] = "";
    //value[kNIMVChatSound] = "video_chat_tip_receiver.aac";
    value[kNIMVChatKeepCalling] = 0;
    value[kNIMVChatVEncodeMode] = kNIMVChatVEModeScreen;

    std::string json_extension = fs.write(value);

    return SdkVChat::Start(kNIMVideoChatModeVideo, apns_text.c_str(),
      custom_info.c_str(), json_extension.c_str(), this);
}

void NimService::VChatEnd(const std::string& session_id) {
    StopAudioDevice();

    Json::FastWriter fs;
    Json::Value value;
    value[kNIMVChatSessionId] = session_id;
    std::string json_extension = fs.write(value);
    SdkVChat::End(json_extension.c_str());

    m_channel_id = 0;
    m_session_id.clear();
    m_uid.clear();
    m_is_vchat_connected = false;
}

bool NimService::VChatCalleeAck(bool accept, const std::string& session_id) {
    std::string json_extension;
    if (accept)
    {
        Json::FastWriter fs;
        Json::Value value;
        value[kNIMVChatSessionId] = session_id;
        value[kNIMVChatCustomVideo] = m_init_params.custom_video ? 1 : 0;
        value[kNIMVChatRecord] = 1;
        value[kNIMVChatVideoRecord] = m_init_params.video_record ? 1 : 0;;
        value[kNIMVChatVideoQuality] = kNIMVChatVideoQuality720p;
        value[kNIMVChatVideoFrameRate] = kNIMVChatVideoFrameRate25;
		value[kNIMVChatRHostSpeaker] = m_init_params.r_host_speaker ? 1 : 0;
        //value[kNIMVChatAudioHighRate] = 1;
        //value[kNIMVChatAudioMode] = 1;
        value[kNIMVChatNeedBadge] = 0;
        //value[kNIMVChatApnsPayload] = "";
        //value[kNIMVChatSound] = "video_chat_tip_receiver.aac";
        value[kNIMVChatKeepCalling] = 1;
        value[kNIMVChatVEncodeMode] = kNIMVChatVEModeScreen;

        json_extension = fs.write(value);
    }
    return SdkVChat::CalleeAck(m_channel_id, accept,
      json_extension.c_str(), this);
}

bool NimService::VChatControl(ControlType type) {
    return SdkVChat::Control(m_channel_id, (NIMVChatControlType)type,
      nullptr, this);
}

void NimService::VChatSetVideoBitrate(int bitrate) {
    SdkVChat::SetVideoBitrate(bitrate, nullptr,
        VChatCallback::SetVideoBitrateCb, this);
}

unsigned char NimService::GetMicAudioVolumn() {
    return SdkDevice::GetAudioVolumn(true);
}

void NimService::SetMicAudioVolumn(unsigned char volumn) {
    SdkDevice::SetAudioVolumn(volumn, true);
}

unsigned char NimService::GetSpeakerAudioVolumn() {
    return SdkDevice::GetAudioVolumn(false);
}

void NimService::SetSpeakerAudioVolumn(unsigned char volumn) {
    SdkDevice::SetAudioVolumn(volumn, false);
}

bool NimService::GetMicAutoVolumn() {
    return SdkDevice::GetAudioInputAutoVolumn();
}

void NimService::SetMicAutoVolumn(bool auto_volumn) {
    SdkDevice::SetAudioInputAutoVolumn(auto_volumn);
}

bool NimService::CustomVideoDataI420(uint8_t* data, int size, int width, 
  int height) {
    return SdkDevice::CustomVideoData(0, (const char*)data,
        size, width, height, nullptr);
}

std::vector<DeviceEventHandler::DeviceInfo> NimService::EnumDevice(
  DeviceEventHandler::DeviceType type) {
    std::list<DeviceEventHandler::DeviceInfo> devices = GetDeviceInfo(type);
    return std::vector<DeviceEventHandler::DeviceInfo>(
      devices.begin(), devices.end());
}

void NimService::StartDevice(const DeviceEventHandler::DeviceInfo& device) {
    std::string device_path = device.device_path;
    if (device_path.empty())
    {
        int num_no = 0;
        GetDefaultDevicePath(num_no, device_path, device.type);
    }
    SetDefaultDevicePath(device_path, device.type);
    
    SdkDevice::StartDevice((NIMDeviceType)device.type, device_path.c_str(), 50, 
      nullptr, DeviceCallback::StartDeviceCb, this);

    SdkDevice::AddDeviceStatusCb((NIMDeviceType)device.type, 
      DeviceCallback::DeviceStatusCb, this);
}

void NimService::EndDevice(const DeviceEventHandler::DeviceInfo& device) {

    SdkDevice::RemoveDeviceStatusCb((NIMDeviceType)device.type);

    SdkDevice::EndDevice((NIMDeviceType)device.type, nullptr);
}

const DeviceEventHandler::DeviceInfo NimService::GetDefaultDeviceInfo(
  DeviceEventHandler::DeviceType type) {
    return m_default_device_info[type];
}

void NimService::SetDefaultDeviceInfo(DeviceType type, const DeviceInfo& info) {
    m_default_device_info[type] = info;
}

void NimService::StartAudioDevice() {
    std::string def_audio;
    int no = 0;

    GetDefaultDevicePath(no, def_audio, DeviceTypeAudioOutChat);
    DeviceInfo out_device_info;
    out_device_info.device_path = def_audio;
    out_device_info.type = DeviceTypeAudioOutChat;
    StartDevice(out_device_info);

    GetDefaultDevicePath(no, def_audio, DeviceTypeAudioIn);
    DeviceInfo in_device_info;
    in_device_info.device_path = def_audio;
    in_device_info.type = DeviceTypeAudioIn;
    StartDevice(in_device_info);
}

void NimService::StopAudioDevice() {
    DeviceInfo in_device_info;
    in_device_info.type = DeviceTypeAudioIn;
    EndDevice(in_device_info);

    DeviceInfo out_device_info;
    out_device_info.type = DeviceTypeAudioOutChat;
    EndDevice(out_device_info);
}

void NimService::SendSystemMessage(const SystemMessage& sys_msg) {
	Json::Value json_msg;
	json_msg[kNIMSysMsgKeyToAccount] = sys_msg.receive_uid; //接收者id
	json_msg[kNIMSysMsgKeyFromAccount] = sys_msg.send_uid; //自己id
	json_msg[kNIMSysMsgKeyAttach] = sys_msg.message; //通知附件内容
	json_msg[kNIMSysMsgKeyType] = kNIMSysMsgTypeCustomP2PMsg; //通知类型

	SdkSysMsg::SendCustomNotification(json_msg.toStyledString().c_str(), nullptr);
}

void NimService::AddClientEventHandler(ClientEventHandler* handler) {
    std::lock_guard<std::mutex> guard(m_client_handlers_mutex);
    m_client_handlers.push_back(handler);
}

void NimService::RemoveClientEventHandler(ClientEventHandler* handler) {
    std::lock_guard<std::mutex> guard(m_client_handlers_mutex);
    m_client_handlers.remove(handler);
}

void NimService::AddVChatEventHandler(VChatEventHandler* handler) {
    std::lock_guard<std::mutex> guard(m_vchat_handlers_mutex);
    m_vchat_handlers.push_back(handler);
}

void NimService::RemoveVChatEventHandler(VChatEventHandler* handler) {
    std::lock_guard<std::mutex> guard(m_vchat_handlers_mutex);
    m_vchat_handlers.remove(handler);
}

void NimService::AddDeviceEventHandler(DeviceEventHandler* handler) {
    std::lock_guard<std::mutex> guard(m_device_handlers_mutex);
    m_device_handlers.push_back(handler);
}

void NimService::RemoveDeviceEventHandler(DeviceEventHandler* handler) {
    std::lock_guard<std::mutex> guard(m_device_handlers_mutex);
    m_device_handlers.remove(handler);
}

void NimService::AddSysMsgEventHandler(SysMsgEventHandler * handler)
{
	std::lock_guard<std::mutex> guard(m_sysmsg_handlers_mutex);
	m_sysmsg_handlers.push_back(handler);
}

void NimService::RemoveSysMsgEventHandler(SysMsgEventHandler * handler)
{
	std::lock_guard<std::mutex> guard(m_sysmsg_handlers_mutex);
	m_sysmsg_handlers.remove(handler);
}

void NimService::RemoveAllEventHandlers() {
    {
        std::lock_guard<std::mutex> guard(m_client_handlers_mutex);
        m_client_handlers.clear();
    }
    {
        std::lock_guard<std::mutex> guard(m_vchat_handlers_mutex);
        m_vchat_handlers.clear();
    }
    {
        std::lock_guard<std::mutex> guard(m_device_handlers_mutex);
        m_device_handlers.clear();
    }

	{
		std::lock_guard<std::mutex> guard(m_sysmsg_handlers_mutex);
		m_sysmsg_handlers.clear();
	}
}

void NimService::OnLogin(const std::list<OtherClientInfo>& clients) {
    std::lock_guard<std::mutex> guard(m_client_handlers_mutex);
    for (ClientEventHandler* handler : m_client_handlers) {
        handler->OnLogin(clients);
    }

    ResetEvent(m_client_logout_event);
}

void NimService::OnLogout() {
    std::lock_guard<std::mutex> guard(m_client_handlers_mutex);
    for (ClientEventHandler* handler : m_client_handlers) {
        handler->OnLogout();
    }

    SetEvent(m_client_logout_event);
}

void NimService::OnLoginInfo(Info info) {
    std::lock_guard<std::mutex> guard(m_client_handlers_mutex);
    for (ClientEventHandler* handler : m_client_handlers) {
        handler->OnLoginInfo(info);
    }
}

void NimService::OnLoginError(Error error, int code, const std::string& message) {
    std::lock_guard<std::mutex> guard(m_client_handlers_mutex);
    for (ClientEventHandler* handler : m_client_handlers) {
        handler->OnLoginError(error, code, message);
    }
}

void NimService::OnKickOut(ClientType client_type, KickReason kick_reason) {
    std::lock_guard<std::mutex> guard(m_client_handlers_mutex);
    for (ClientEventHandler* handler : m_client_handlers) {
        handler->OnKickOut(client_type, kick_reason);
    }
}

void NimService::OnVChatChannelCreated(const int64_t channel_id) {
    m_channel_id = channel_id;

    std::lock_guard<std::mutex> guard(m_vchat_handlers_mutex);
    for (VChatEventHandler* handler : m_vchat_handlers) {
        handler->OnVChatChannelCreated(channel_id);
    }
}

void NimService::OnVChatStartError(int code) {
    m_is_vchat_connected = false;

    std::lock_guard<std::mutex> guard(m_vchat_handlers_mutex);
    for (VChatEventHandler* handler : m_vchat_handlers) {
        handler->OnVChatStartError(code);
    }
}

void NimService::OnVChatConnected(const int64_t channel_id) {
    SdkVChat::SetCustomData(false, m_init_params.custom_video, nullptr,
      VChatCallback::SetCustomDataCb, this);
    SdkVChat::SetVideoQuality(kNIMVChatVideoQuality720p, nullptr, 
      VChatCallback::SetVideoQualityCb, this);
    SdkVChat::SelectVideoAdaptiveStrategy(kNIMVChatVEModeScreen, nullptr, 
      VChatCallback::SelectVideoAdaptiveStrategyCb, this);
    SdkVChat::SetFrameRate(kNIMVChatVideoFrameRate25, nullptr, 
      VChatCallback::SetFrameRateCb, this);
    SdkVChat::SetVideoBitrate(2000*1000, nullptr, 
      VChatCallback::SetVideoBitrateCb, this);
    
    m_is_vchat_connected = true;

    std::lock_guard<std::mutex> guard(m_vchat_handlers_mutex);
    for (VChatEventHandler* handler : m_vchat_handlers) {
        handler->OnVChatConnected(m_channel_id);
    }

    ResetEvent(m_vchat_logout_event);
}

void NimService::OnVChatConnectLogout() {
    std::lock_guard<std::mutex> guard(m_vchat_handlers_mutex);
    for (VChatEventHandler* handler : m_vchat_handlers) {
        handler->OnVChatConnectLogout();
    }

    SetEvent(m_vchat_logout_event);
}

void NimService::OnVChatConnectError(const int code) {
    VChatEnd("");

    std::lock_guard<std::mutex> guard(m_vchat_handlers_mutex);
    for (VChatEventHandler* handler : m_vchat_handlers) {
        handler->OnVChatConnectError(code);
    }
}

void NimService::OnCalleeRejected() {
    m_is_vchat_connected = false;

    std::lock_guard<std::mutex> guard(m_vchat_handlers_mutex);
    for (VChatEventHandler* handler : m_vchat_handlers) {
        handler->OnCalleeRejected();
    }
}

void NimService::OnControlNotify(const std::string& uid, ControlType type) {
    std::lock_guard<std::mutex> guard(m_vchat_handlers_mutex);
    for (VChatEventHandler* handler : m_vchat_handlers) {
        handler->OnControlNotify(uid, type);
    }
}

void NimService::OnReceivedVChatInvite(const int64_t channel_id, 
  const std::string& uid, const std::string& custom_info) {
    m_channel_id = channel_id;
    m_uid = uid;

    std::lock_guard<std::mutex> guard(m_vchat_handlers_mutex);
    for (VChatEventHandler* handler : m_vchat_handlers) {
        handler->OnReceivedVChatInvite(channel_id, uid, custom_info);
    }
}

void NimService::OnHangup() {
    VChatEnd("");

    std::lock_guard<std::mutex> guard(m_vchat_handlers_mutex);
    for (VChatEventHandler* handler : m_vchat_handlers) {
        handler->OnHangup();
    }
}

void NimService::OnCapturedVideoData(unsigned __int64 time, const char* data,
    unsigned int size, unsigned int width, unsigned int height) {
    std::lock_guard<std::mutex> guard(m_vchat_handlers_mutex);
    for (VChatEventHandler* handler : m_vchat_handlers) {
        handler->OnCapturedVideoData(time, data, size, width, height);
    }
}

void NimService::OnReceivedVideoData(unsigned __int64 time, const char* data,
  unsigned int size, unsigned int width, unsigned int height) {
    std::lock_guard<std::mutex> guard(m_vchat_handlers_mutex);
    for (VChatEventHandler* handler : m_vchat_handlers) {
        handler->OnReceivedVideoData(time, data, size, width, height);
    }
}

void NimService::OnRealtimeStatsInfo(const StatsInfo& info) {
    std::lock_guard<std::mutex> guard(m_vchat_handlers_mutex);
    for (VChatEventHandler* handler : m_vchat_handlers) {
        handler->OnRealtimeStatsInfo(info);
    }
}

void NimService::OnSessionNetStatus(const std::string& uid, 
  const SessionNetStatus status) {
    std::lock_guard<std::mutex> guard(m_vchat_handlers_mutex);
    for (VChatEventHandler* handler : m_vchat_handlers) {
        handler->OnSessionNetStatus(uid, status);
    }
}

void NimService::OnVChatOptError(const OptType type, const int code) {
    std::lock_guard<std::mutex> guard(m_vchat_handlers_mutex);
    for (VChatEventHandler* handler : m_vchat_handlers) {
        handler->OnVChatOptError(type, code);
    }
}

void NimService::OnAudioVolumeNotify(const int32_t self_volume,
  const int32_t other_volume) {
    std::lock_guard<std::mutex> guard(m_vchat_handlers_mutex);
    for (VChatEventHandler* handler : m_vchat_handlers) {
        handler->OnAudioVolumeNotify(self_volume, other_volume);
    }
}

void NimService::OnVchatSyncNotify(bool accept) {
    std::lock_guard<std::mutex> guard(m_vchat_handlers_mutex);
    for (VChatEventHandler* handler : m_vchat_handlers) {
        handler->OnVchatSyncNotify(accept);
    }
}

void NimService::OnDeviceStarted(const DeviceInfo& device) {
    std::lock_guard<std::mutex> guard(m_device_handlers_mutex);
    for (DeviceEventHandler* handler : m_device_handlers) {
        handler->OnDeviceStarted(device);
    }
}

void NimService::OnDeviceStartError(const DeviceInfo& device) {
    std::lock_guard<std::mutex> guard(m_device_handlers_mutex);
    for (DeviceEventHandler* handler : m_device_handlers) {
        handler->OnDeviceStartError(device);
    }
}

void NimService::OnDeviceStatus(const DeviceInfo& device) {
    std::lock_guard<std::mutex> guard(m_device_handlers_mutex);
    for (DeviceEventHandler* handler : m_device_handlers) {
        handler->OnDeviceStatus(device);
    }
}

void NimService::OnReceiveMessage(const SystemMessage& sys_msg)
{
	std::lock_guard<std::mutex> guard(m_sysmsg_handlers_mutex);
	for (SysMsgEventHandler* handler : m_sysmsg_handlers) {
		handler->OnReceiveMessage(sys_msg);
	}
}

bool NimService::ClientInit(const std::string& app_key, 
  const std::string& app_data_dir, 
  const std::string& app_install_dir) {

    if (NULL == g_nim_sdk_instance)
        g_nim_sdk_instance = new SdkInstance();

    if (!g_nim_sdk_instance->LoadSdkDll(app_install_dir.c_str(), kSdkNimDll))
        return false;

    Json::Value json_extension;
    json_extension[kNIMAppKey] = app_key;

    Json::Value global_config;
    global_config[kNIMDataBaseEncryptKey] = kDatabaseEncryptKey;

#if defined _DEBUG
    global_config[kNIMSDKLogLevel] = kNIMSDKLogLevelPro;
#else
    global_config[kNIMSDKLogLevel] = kNIMSDKLogLevelApp;
#endif
    
    json_extension[kNIMGlobalConfig] = global_config;

    return SdkClient::Init(app_data_dir.c_str(), app_install_dir.c_str(), 
      base::GetJsonStringWithNoStyled(json_extension).c_str());
}


void NimService::ClientCleanup() {
    SdkClient::Cleanup(nullptr);

    g_nim_sdk_instance->UnLoadSdkDll();
    delete g_nim_sdk_instance;
    g_nim_sdk_instance = NULL;
}

bool NimService::VChatInit() {
    bool res = SdkVChat::Init("");
    if (!res)
        return res;

    Json::FastWriter fs;
    Json::Value value;
    value[kNIMVideoSubType] = kNIMVideoSubTypeI420;
    std::string json_extension = fs.write(value);

    SdkVChat::SetCbFunc(VChatCallback::VChatCb, this);

    SdkDevice::SetVideoDataCb(true, json_extension.c_str(),
        DeviceCallback::CapturedVideoDataCb, this);

    SdkDevice::SetVideoDataCb(false, json_extension.c_str(),
        DeviceCallback::ReceivedVideoDataCb, this);

	SdkSysMsg::RegSysmsgCb(NULL,SysMsgCallback::ReceiveMessageCb, this);
    return res;
}

void NimService::VChatCleanup() {
    SdkVChat::Cleanup(nullptr);
}

std::list<DeviceEventHandler::DeviceInfo> NimService::GetDeviceInfo(
  DeviceType type)
{
    SdkDevice::EnumDeviceDevpath((NIMDeviceType)type, nullptr,
        DeviceCallback::EnumDeviceDevpathCb, this);
    return m_device_info_list[type];
}

bool NimService::GetDefaultDevicePath(int &no, std::string &device_path, 
    DeviceType type)
{
    if (type == DeviceTypeAudioOutChat)
    {
        type = DeviceTypeAudioOut;
    }
    no = 0;
    device_path.clear();
    GetDeviceInfo(type);
    if (m_device_info_list[type].size() != 0)
    {
        if (!m_default_device_info[type].device_path.empty())
        {
            int k = 0;
            for (auto i = m_device_info_list[type].begin(); 
                 i != m_device_info_list[type].end(); i++, k++)
            {
                if (i->device_path == m_default_device_info[type].device_path)
                {
                    no = k;
                    device_path = m_default_device_info[type].device_path;
                    break;
                }
            }
        }
        if (device_path.empty())
        {
            no = 0;
            device_path = m_device_info_list[type].begin()->device_path;
            m_default_device_info[type].device_path = device_path;
        }
    }

    return !device_path.empty();
}

void NimService::SetDefaultDevicePath(std::string device_path, DeviceType type)
{
    if (type == DeviceTypeAudioOutChat)
    {
        type = DeviceTypeAudioOut;
    }
    m_default_device_info[type].device_path = device_path;
}


} // namespace nim