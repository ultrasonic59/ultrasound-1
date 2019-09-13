#ifndef NIM_SERVICE_VCHAT_EVENT_HANDLER_H_
#define NIM_SERVICE_VCHAT_EVENT_HANDLER_H_

#include <stdint.h>
#include <string>

namespace nim {

class VChatEventHandler {
public:
    enum ControlType {
        ControlTypeOpenAudio = 1,
        ControlTypeCloseAudio = 2,
        ControlTypeOpenVideo = 3,
        ControlTypeCloseVideo = 4,
        ControlTypeAudioToVideo = 5,
        ControlTypeAgreeAudioToVideo = 6,
        ControlTypeRejectAudioToVideo = 7,
        ControlTypeVideoToAudio = 8,
        ControlTypeBusyLine = 9,
        ControlTypeCamaraNotAvailable = 10,
        ControlTypeEnterBackground = 11,
        ControlTypeReceiveStartNotifyFeedback = 12,
        ControlTypeMp4StartRecord = 13,
        ControlTypeMp4StopRecord = 14,
    };

    enum SessionNetStatus {
        SessionNetStatusVideoClose = -1,
        SessionNetStatusVeryGood = 0,
        SessionNetStatusGood = 1,
        SessionNetStatusPoor = 2,
        SessionNetStatusBad = 3,
        SessionNetStatusVeryBad = 4,
    };

    struct StatsInfo {
        int rtt = 0;

        int video_fps = 0;
        int video_KBps = 0;
        int video_lost_rate = 0;
        int video_width = 0;
        int video_height = 0;

        int audio_fps = 0;
        int audio_KBps = 0; 
        int audio_lost_rate = 0;
    };

    enum OptType {
        SetCustomData,
        SetVideoQuality,
        SetVideoBitrate,
        SetFrameRate,
        SelectVideoAdaptiveStrategy,
    };

    virtual ~VChatEventHandler() { }

    virtual void OnVChatChannelCreated(const int64_t channel_id) = 0;
    virtual void OnVChatStartError(int code) = 0;

    virtual void OnVChatConnected(const int64_t channel_id) = 0;
    virtual void OnVChatConnectLogout() = 0;
    virtual void OnVChatConnectError(const int code) = 0;

    virtual void OnCalleeRejected() = 0;

    virtual void OnControlNotify(const std::string& uid, ControlType type) = 0;

    virtual void OnReceivedVChatInvite(const int64_t channel_id, 
      const std::string& uid, const std::string& custom_info) = 0;

    virtual void OnHangup() = 0;

    virtual void OnCapturedVideoData(unsigned __int64 time, const char* data,
        unsigned int size, unsigned int width, unsigned int height) = 0;

    virtual void OnReceivedVideoData(unsigned __int64 time, const char* data,
        unsigned int size, unsigned int width, unsigned int height) = 0;

    virtual void OnRealtimeStatsInfo(const StatsInfo& info) = 0;

    virtual void OnSessionNetStatus(const std::string& uid, 
      const SessionNetStatus status) = 0;

    virtual void OnVChatOptError(const OptType type, const int code) = 0;

    virtual void OnAudioVolumeNotify(const int32_t self_volume, 
      const int32_t other_volume) = 0;

    virtual void OnVchatSyncNotify(bool accept) = 0;
};

} // namespace nim

#endif // NIM_SERVICE_VCHAT_EVENT_HANDLER_H_
