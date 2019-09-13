#include "nim_service/callback/vchat_callback.h"

#include <assert.h>

#include "json.h"

#include "nim_service/nim_service.h"
#include "nim_service/sdk/sdk_vchat.h"

namespace nim {

void VChatCallback::VChatCb(enum NIMVideoChatSessionType type, 
  int64_t channel_id, int code, const char *json_extension, 
  const void *user_data) {
    switch (type) {
    case kNIMVideoChatSessionTypeStartRes: {
        // 创建通话结果 code=200成功，json 返回kNIMVChatSessionId
        if (code == 200)
            NimService::GetInstance()->OnVChatChannelCreated(channel_id);
        else
            NimService::GetInstance()->OnVChatStartError(code);
        break;
    }
    case kNIMVideoChatSessionTypeInviteNotify: {
        // 通话邀请 code无效,json 返回kNIMVChatUid发起者，
        // kNIMVChatType对应NIMVideoChatMode, kNIMVChatTime, kNIMVChatCustomInfo \n
        Json::Value value;
        Json::Reader reader;
        if (reader.parse(json_extension, value)) {
            std::string uid = value[kNIMVChatUid].asString();
            int mode = value[kNIMVChatType].asInt();
            int64_t chat_time = value[kNIMVChatTime].asInt64();
            std::string custom_info = value[kNIMVChatCustomInfo].asString();

            if (NimService::GetInstance()->IsVChatConnected()) {
                SdkVChat::Control(channel_id, kNIMTagControlBusyLine, nullptr, 
                  nullptr);
                SdkVChat::CalleeAck(channel_id, false, nullptr, nullptr);
            }
            else if (mode == kNIMRtsVideoChatModeVideo) {
                SdkVChat::Control(channel_id, 
                  kNIMTagControlReceiveStartNotifyFeedback, nullptr, nullptr);

                NimService::GetInstance()->OnReceivedVChatInvite(channel_id, 
                  uid, custom_info);
            }
        }
        break;
    }
    case kNIMVideoChatSessionTypeCalleeAckRes: {
        // 确认通话，接受拒绝结果 json 无效 code: 200:成功 9103 : 已经在其他端接听 / 
        // 拒绝过这通电话 \n

        // TODO(victor) : What should I do?
        break;
    }
    case kNIMVideoChatSessionTypeCalleeAckNotify: {
        // 确认通话，接受拒绝通知 code=无效，json 返回kNIMVChatUid对方，
        // kNIMVChatType对应NIMVideoChatMode, kNIMVChatAccept \n
        Json::Value value;
        Json::Reader reader;
        if (reader.parse(json_extension, value)) {
            bool accept = value[kNIMVChatAccept].asInt() > 0;
            if (!accept) {
                NimService::GetInstance()->OnCalleeRejected();
            }
        }
        break;
    }
    case kNIMVideoChatSessionTypeControlRes: {
        // NIMVChatControlType 结果  code=200成功，
        // json 返回kNIMVChatType对应NIMVChatControlType \n
        Json::Value value;
        Json::Reader reader;
        if (reader.parse(json_extension, value)) {
            int type = value[kNIMVChatType].asInt();
        }
        break;
    }
    case kNIMVideoChatSessionTypeControlNotify: {
        // NIMVChatControlType 通知	code=无效，json 返回kNIMVChatUid对方，
        // kNIMVChatType对应NIMVChatControlType \n
        Json::Value value;
        Json::Reader reader;
        if (reader.parse(json_extension, value)) {
            std::string uid = value[kNIMVChatUid].asString();
            int type = value[kNIMVChatType].asInt();
            NimService::GetInstance()->OnControlNotify(uid, 
              (VChatEventHandler::ControlType)type);
        }
        break;
    }
    case kNIMVideoChatSessionTypeConnect: {
        // 通话中链接状态通知 code对应NIMVChatConnectErrorCode，非200均失败并底层结束 \n
        // code=1001,代表通话注销结果，带通话本地时长（kNIMVChatTime）及数据量
        //（kNIMVChatTrafficStatRX kNIMVChatTrafficStatTX）\n
        if (code == 200)
            NimService::GetInstance()->OnVChatConnected(0);
        else  if (code == kNIMVChatConnectLogout)
            NimService::GetInstance()->OnVChatConnectLogout();
        else
            NimService::GetInstance()->OnVChatConnectError(code);
        break;
    }
    case kNIMVideoChatSessionTypePeopleStatus: {
        // 通话中成员状态 code对应NIMVideoChatSessionStatus, json返回kNIMVChatUid，
        // 如果是离开带kNIMVChatStatus对应NIMVideoChatUserLeftType \n
        
        // TODO(victor) : multiple vchat
        break;
    }
    case kNIMVideoChatSessionTypeNetStatus: {
        // 通话中网络状态 code对应NIMVideoChatSessionNetStat, json返回kNIMVChatUid \n
        Json::Value value;
        Json::Reader reader;
        if (reader.parse(json_extension, value)) {
            std::string uid = value[kNIMVChatUid].asString();
            NimService::GetInstance()->OnSessionNetStatus(uid,
              (VChatEventHandler::SessionNetStatus)code);
        }

        break;
    }
    case kNIMVideoChatSessionTypeHangupRes: {
        //通话主动结果 code=200成功，json无效 \n
        break;
    }
    case kNIMVideoChatSessionTypeHangupNotify: {
        //通话被挂断通知 code无效，json无效 \n
        NimService::GetInstance()->OnHangup();
        break;
    }
    case kNIMVideoChatSessionTypeSyncAckNotify: {
        // 其他端接听挂断后的同步通知 json 返回 kNIMVChatTime，
        // kNIMVChatType对应NIMVideoChatMode，kNIMVChatAccept，kNIMVChatClient  \n

        Json::Value value;
        Json::Reader reader;
        if (reader.parse(json_extension, value)) {
            bool accept = value[kNIMVChatAccept].asInt() > 0;
            NimService::GetInstance()->OnVchatSyncNotify(accept);
        }
        break;
    }
    case kNIMVideoChatSessionTypeMp4Notify: {
        // windows(pc)有效.通知MP4录制状态，包括开始录制和结束录制， code无效，json(其他成员的录制通知带uid，自己的不带) 返回如下 \n
        // MP4开始 	{"mp4_start":{ "mp4_file": "d:\\test.mp4", "time": 14496477000000, "uid":"id123" }} \n
        // MP4结束 	{"mp4_close":{ "mp4_file": "d:\\test.mp4", "time": 120000, "status": 0, "uid":"id123" }} \n
        break;
    }
    case kNIMVideoChatSessionTypeAuRecordNotify: {
        // windows(pc)有效.通知音频录制状态，包括开始录制和结束录制 code无效，json 返回如下 \n
        // 录制开始 	{"audio_record_start":{ "file": "d:\\test.aac", "time": 14496477000000 }} \n
        // 录制结束 	{"audio_record_close":{ "file": "d:\\test.aac", "time": 120000, "status": 0 }} \n
        break;
    }
    case kNIMVideoChatSessionTypeInfoNotify: {
        // windows(pc)有效.实时状态 
        /*
        {
            "static_info": {
                "audio": { "KBps": 7, "fps": 16, "lost_rate": 58 },
                "cpu": { "self_percent": 11, "total_percent": 26 },
                "meminfo": { "memory_size": 46508 },
                "rtt": 21,
                "video": { "KBps": 139, "fps": 6, "height": 480,
                "lost_rate": 58, "width": 640 }
            }
        }
        */
        Json::Value value;
        Json::Reader reader;
        if (reader.parse(json_extension, value)) {
            VChatEventHandler::StatsInfo info;

            Json::Value video = value[kNIMVChatStaticInfo][kNIMVChatVideo];
            Json::Value audio = value[kNIMVChatStaticInfo][kNIMVChatAudio];

            info.rtt = value[kNIMVChatStaticInfo][kNIMVChatRtt].asInt();

            info.audio_fps = audio[kNIMVChatFPS].asInt();
            info.audio_KBps = audio[kNIMVChatKBPS].asInt();;
            info.audio_lost_rate = audio[kNIMVChatLostRate].asInt();;

            info.video_fps = video[kNIMVChatFPS].asInt();;
            info.video_height = video["height"].asInt();
            info.video_KBps = video[kNIMVChatKBPS].asInt();;
            info.video_lost_rate = video[kNIMVChatLostRate].asInt();;
            info.video_width = video["width"].asInt();

            NimService::GetInstance()->OnRealtimeStatsInfo(info);
        }
        break;
    }
    case kNIMVideoChatSessionTypeVolumeNotify: {
        // windows(pc)有效.音量状态 
        /*
        {
            "audio_volume":{ 
                 "self": {"status":600}, 
                 "receiver": [{"uid":"id123","status":1000},{"uid":"id456","status":222}] 
             }
         } 
         */

        Json::Value value;
        Json::Reader reader;
        if (reader.parse(json_extension, value))
        {
            if (value.isObject() && value[kNIMVChatAudioVolume].isObject())
            {
                std::string current_user_id = 
                NimService::GetInstance()->GetCurrentAccount();
                std::map<std::string, int32_t> volume_infos;
                int32_t self_volume = value[kNIMVChatAudioVolume]
                [kNIMVChatSelf][kNIMVChatStatus].asInt();
                volume_infos[current_user_id] = self_volume;
                  
                int32_t other_volume = 0;
                if (value[kNIMVChatAudioVolume][kNIMVChatReceiver].isArray())
                {
                    int count = 
                      value[kNIMVChatAudioVolume][kNIMVChatReceiver].size();
                    for (int i = 0; i < count; ++i)
                    {
                        Json::Value item;
                        item = value[kNIMVChatAudioVolume]
                          [kNIMVChatReceiver].get(i, item);
                        std::string uid = item[kNIMVChatUid].asString();
                        int32_t status = item[kNIMVChatStatus].asInt();
                        other_volume = volume_infos[uid] = status;
                    }
                }

                NimService::GetInstance()->OnAudioVolumeNotify(self_volume, 
                  other_volume);
            }
        }
        break;
    }
    default:
        break;
    }
}

void VChatCallback::SetMemberInBlacklistCb(bool ret, int code,
  const char *json_extension, const void *user_data) {

}

void VChatCallback::CreatRoomCb(int code, int64_t channel_id,
  const char *json_extension, const void *user_data) {

}

void VChatCallback::JoinRoomCb(int code, int64_t channel_id,
    const char *json_extension, const void *user_data) {

}

void VChatCallback::SetCustomDataCb(bool ret, int code, const char *json_extension,
  const void *user_data) {
    if (code != 200) {
        NimService::GetInstance()->OnVChatOptError(
          VChatEventHandler::SetCustomData, code);
    }
}

void VChatCallback::SetVideoQualityCb(bool ret, int code,
  const char *json_extension, const void *user_data) {
    if (code != 200) {
        NimService::GetInstance()->OnVChatOptError(
          VChatEventHandler::SetVideoQuality, code);
    }
}

void VChatCallback::SetVideoBitrateCb(bool ret, int code,
  const char *json_extension, const void *user_data) {
    if (code != 200) {
        NimService::GetInstance()->OnVChatOptError(
            VChatEventHandler::SetVideoBitrate, code);
    }
}

void VChatCallback::NetDetectCb(bool ret, int code,
  const char *json_extension, const void *user_data) {

}

void VChatCallback::SetFrameRateCb(bool ret, int code,
  const char *json_extension, const void *user_data) {
    if (code != 200) {
        NimService::GetInstance()->OnVChatOptError(
            VChatEventHandler::SetFrameRate, code);
    }
}

void VChatCallback::StartRecordCb(bool ret, int code, const char *file,
  int64_t time, const char *json_extension, const void *user_data) {

}

void VChatCallback::StopRecordCb(bool ret, int code, const char *file,
  int64_t time, const char *json_extension, const void *user_data) {

}

void VChatCallback::StartAudioRecordCb(bool ret, int code, const char *file,
  int64_t time, const char *json_extension, const void *user_data) {

}

void VChatCallback::StopAudioRecordCb(bool ret, int code, const char *file,
  int64_t time, const char *json_extension, const void *user_data) {

}

void VChatCallback::UpdateRtmpUrlCb(bool ret, int code, const char *json_extension,
  const void *user_data) {

}

void VChatCallback::SelectVideoAdaptiveStrategyCb(bool ret, int code,
  const char *json_extension, const void *user_data) {
    if (code != 200) {
        NimService::GetInstance()->OnVChatOptError(
            VChatEventHandler::SelectVideoAdaptiveStrategy, code);
    }
}

void VChatCallback::SetUidPictureAsMainCb(bool ret, int code,
    const char *json_extension, const void *user_data) {

}

} // namespace nim