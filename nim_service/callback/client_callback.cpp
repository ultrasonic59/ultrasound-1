#include "nim_service/callback/client_callback.h"

#include "json.h"
#include "nim_res_code_def.h"
#include "nim_service/nim_service.h"

namespace nim {

void ClientCallback::LoginCb(const char *json_params, const void *user_data) {
    Json::Value value;
    Json::Reader reader;
    if (reader.parse(json_params, value) && value.isObject())
    {
        NIMResCode res_code = (NIMResCode)value[kNIMResCode].asInt();
        NIMLoginStep login_step = (NIMLoginStep)value[kNIMLoginStep].asUInt();
        bool relogin = value[kNIMRelogin].asBool();
        bool retrying = value[kNIMRetrying].asBool();
        Json::Value other_clients = value[kNIMOtherClientsPres];

        if (res_code == kNIMResSuccess) {
            switch (login_step)
            {
            case kNIMLoginStepLinking: {
                NimService::GetInstance()->OnLoginInfo(
                    ClientEventHandler::InfoLinking);
                break;
            }
            case kNIMLoginStepLink: {
                NimService::GetInstance()->OnLoginInfo(
                    ClientEventHandler::InfoLink);
                break;
            }
            case kNIMLoginStepLogining: {
                NimService::GetInstance()->OnLoginInfo(
                    ClientEventHandler::InfoLogining);
                break;
            }
            case kNIMLoginStepLogin: {
                std::list<ClientEventHandler::OtherClientInfo> clients;
                if (other_clients.isArray())
                {
                    int size = other_clients.size();
                    for (int index = 0; index < size; index++)
                    {
                        ClientEventHandler::OtherClientInfo info;
                        info.app_account = other_clients[index][kNIMPresAppAccount].asString();
                        info.client_type = (ClientEventHandler::ClientType)other_clients[index][kNIMPresClientType].asUInt();
                        info.client_os = other_clients[index][kNIMPresOS].asString();
                        info.mac_address = other_clients[index][kNIMPresMac].asString();
                        info.device_id = other_clients[index][kNIMPresDeviceID].asString();
                        info.login_time = other_clients[index][kNIMPresLoginTime].asInt64();
                        clients.push_back(info);
                    }
                }

                NimService::GetInstance()->OnLogin(clients);
                break;
            }
            default:
                break;
            }
        }
        else {
            switch (res_code)
            {
            case kNIMResUidPassError:
                NimService::GetInstance()->OnLoginError(
                    ClientEventHandler::ERROR_USER_PASS, res_code, "");
                break;
            default:
                NimService::GetInstance()->OnLoginError(
                    ClientEventHandler::ERROR_UNKONWN, res_code, "");
                break;
            }
        }
    }
}

void ClientCallback::LogoutCb(const char *json_params, const void *user_data) {
    NIMResCode res_code = kNIMResSuccess;
    Json::Reader reader;
    Json::Value value;
    if (reader.parse(json_params, value) && value.isObject())
    {
        res_code = (NIMResCode)value[kNIMLogoutErrorCode].asInt();
    }

    NimService::GetInstance()->OnLogout();
}

void ClientCallback::AutoReloginCb(const char *json_params, 
  const void *user_data) {

}

void ClientCallback::KickoutCb(const char *json_params, const void *user_data) {
    Json::Reader reader;
    Json::Value values;
    if (reader.parse(json_params, values) && values.isObject())
    {
        ClientEventHandler::ClientType client_type = 
          (ClientEventHandler::ClientType)values[kNIMKickoutClientType].asUInt();
        ClientEventHandler::KickReason kick_reason = 
          (ClientEventHandler::KickReason)values[kNIMKickoutReasonCode].asUInt();

        nim::NimService::GetInstance()->OnKickOut(client_type, kick_reason);
    }
}

void ClientCallback::DisconnectCb(const char *json_params, 
  const void *user_data) {

}

void ClientCallback::MultispotLoginNotifyCb(const char *json_params,
  const void *user_data) {

}

void ClientCallback::KickoutOtherClientCb(const char *json_params,
  const void *user_data) {

}

void ClientCallback::SyncMultiportPushConfigCb(int rescode, const char *content,
  const char *json_params, const void *user_data) {

}

void ClientCallback::SetMultiportPushConfigCb(int rescode, const char *content,
  const char *json_params, const void *user_data) {

}

void ClientCallback::GetMultiportPushConfigCb(int rescode, const char *content,
  const char *json_params, const void *user_data) {

}

} // namespace nim