#ifndef CORE_SERVICE_CORE_SERVICE_DELEGATE_H_
#define CORE_SERVICE_CORE_SERVICE_DELEGATE_H_

#include <list>
#include <map>
#include <mutex>

#include "net_service/http/http_request_event_handler.h"

namespace core {

class CoreServiceDelegate : public net::HttpRequestEventHandler {
public:
    static void InitializeServerAPI();

    void ClearActiveRequests();
	void SetProxyInfo(const std::string& host, const int port, const std::string& username, const std::string& password, bool enabled);

    void Login(const std::string& uid, const std::string& password);
    void GetPrimaryUserInfo(const std::string& uid);
    void GetSuperiorUserInfo(const std::string& uid);
	void UpdatePassword(const std::string& old_password, const std::string& new_password);
	void QuerySubscriberSuperiorUser();
	void QuerySuperiorUser(std::map<std::string, std::string> conditions);
	void QueryPrimaryUserInfo(const std::string& uid);
    void UpdatePrimaryUserInfo(const Json::Value& data);
    void UpdateSuperiorUserInfo(const Json::Value& data);
    void QuerySubscriberSuperiorUserOnlineStatus(std::list<core::SuperiorUser> superior_users);

	void CreateReport(const Json::Value& data);
	void UpdateReport(const Json::Value& data);
	void ConfirmReport(const Json::Value& data, const std::string& peer_uid);
	void QueryReport(std::map<std::string, std::string> conditions);

    void GetTemplates(std::map<std::string, std::string> conditions);
    void CreateTemplate(const Json::Value& data);
    void UpdateTemplate(const Json::Value& data);
    void QueryTemplate(std::map<std::string, std::string> conditions);
    void DeleteTemplate(std::map<std::string, std::string> conditions);

	void CheckUpgrade(const std::string & app);

    void QueryControlPrimaryUser();

    // net::HttpRequestEventHandler
    void OnResponse(net::HttpRequest* request) override;

private:
    enum ServerAPI {
        LoginAPI,
        PrimaryFetchTokenAPI,
        SuperiorFetchTokenAPI,
        PrimaryGetMyInfoAPI,
        SuperiorGetMyInfoAPI,
		UpdatePasswordAPI,
		QuerySubscriberSuperiorUserAPI,
		QuerySuperiorUserAPI,
		QueryPrimaryUserInfoAPI,
        UpdatePrimaryUserInfoAPI,
        UpdateSuperiorUserInfoAPI,
		CreateReportAPI,
		UpdateReportAPI,
		ConfirmReportAPI,
		QueryReportAPI,
        QuerySubscriberSuperiorUserOnlineAPI,
		CheckAppUpgradeAPI,
        QueryControlPrimaryUserAPI,
        GetTemplateAPI,
        QueryTemplateAPI,
        CreateTemplateAPI,
        UpdateTemplateAPI,
        DeleteTemplateAPI
    };

    struct ErrorInfo {
        int error_code = -1;
        std::string message;
    };

	void PushActiveRequest(int64_t request_id, ServerAPI api);
	bool PopActiveRequest(int64_t request_id, ServerAPI* api);

    void FetchPrimaryToken();
    void FetchSuperiorToken();

    void HandleLoginAPIResponse(net::HttpRequest* request);
    void HandlePrimaryFetchTokenAPIResponse(net::HttpRequest* request);
    void HandleSuperiorFetchTokenAPIResponse(net::HttpRequest* request);

    void HandlePrimaryGetMyInfoAPIResponse(net::HttpRequest* request);
    void HandleSuperiorGetMyInfoAPIResponse(net::HttpRequest* request);
	void HandleUpdatePasswordAPIResponse(net::HttpRequest* request);
	void HandleQuerySubscriberSuperiorUserAPIResponse(net::HttpRequest* request);
	void HandleQuerySuperiorUserAPIResponse(net::HttpRequest* request);
	void HandleQueryPrimaryUserInfoAPIResponse(net::HttpRequest* request);
    void HandleUpdatePrimaryUserInfoAPIResponse(net::HttpRequest* request);
    void HandleUpdateSuperiorUserInfoAPIResponse(net::HttpRequest* request);

	void HandleCreateReportAPIResponse(net::HttpRequest* request);
	void HandleUpdateReportAPIResponse(net::HttpRequest* request);
	void HandleConfirmReportAPIResponse(net::HttpRequest* request);
	void HandleQueryReportAPIResponse(net::HttpRequest* request);
    void HandleQuerySubscriberSuperiorUserOnlineStatusAPIResponse(net::HttpRequest* request);

	void HandleCheckAppUpgradeAPIResponse(net::HttpRequest* request);

    void HandleQueryControlPrimaryUserAPIResponse(net::HttpRequest* request);

    void HandleGetTemplateAPIResponse(net::HttpRequest* request);
    void HandleQueryTemplateAPIResponse(net::HttpRequest* request);
    void HandleCreateTemplateAPIResponse(net::HttpRequest* request);
    void HandleUpdateTemplateAPIResponse(net::HttpRequest* request);
    void HandleDeleteTemplateAPIResponse(net::HttpRequest* request);

    bool GetErrorInfo(net::HttpRequest* request, ErrorInfo* info);

	std::mutex m_active_requests_mutex;
    std::map<int64_t, ServerAPI> m_active_requests;

    static std::map<ServerAPI, const char*> m_server_API;
};

} // namespace core

#endif // CORE_SERVICE_CORE_SERVICE_DELEGATE_H_