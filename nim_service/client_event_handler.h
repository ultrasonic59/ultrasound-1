#ifndef NIM_SERVICE_CLIENT_EVENT_HANDLER_H_
#define NIM_SERVICE_CLIENT_EVENT_HANDLER_H_

#include <list>
#include <stdint.h>
#include <string>

namespace nim {

class ClientEventHandler {
public:

    enum ClientType {
        ClientTypeDefault = 0,  /**< Default, unset*/
        ClientTypeAndroid = 1,  /**< Android*/
        ClientTypeiOS = 2,  /**< iOS*/
        ClientTypePCWindows = 4,  /**< PC Windows*/
        ClientTypeWeb = 16, /**< Web*/
        ClientTypeRestAPI = 32, /**< RestAPI*/
        ClientTypeMacOS = 64, /**< Mac*/
    };

    struct OtherClientInfo {
        std::string   app_account;
        ClientType    client_type;
        std::string   client_os;
        std::string   mac_address;
        std::string   device_id;
        int64_t       login_time;
    };

    enum Info {
        InfoLinking = 0,
        InfoLink = 1,
        InfoLogining = 2,
        InfoLogin = 3,
    };

    enum Error {
        ERROR_UNKONWN,
        ERROR_USER_TOKEN,
        ERROR_USER_PASS,
    };

    enum KickReason
    {
        KickReasonSameGeneric = 1,
        KickReasonServerKick = 2,
        KickReasonKickBySelfOtherClient = 3,
    };

	struct NetProxyInfo
	{
		struct
		{
			bool enabled;
			std::string host;
			int port;
			std::string username;
			std::string password;
		}nim;

		struct
		{
			bool enabled;
			std::string host;
			int port;
			std::string username;
			std::string password;
		}http;

	};

    virtual ~ClientEventHandler() {}

    virtual void OnLogin(const std::list<OtherClientInfo>& clients) = 0;
    virtual void OnLogout() = 0;
    virtual void OnLoginInfo(Info info) = 0;
    virtual void OnLoginError(Error error, int code, const std::string& message) = 0;
    virtual void OnKickOut(ClientType client_type, KickReason kick_reason) = 0;
};

} // namespace nim

#endif // NIM_SERVICE_CLIENT_EVENT_HANDLER_H_
