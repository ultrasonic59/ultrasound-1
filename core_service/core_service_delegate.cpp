#include <assert.h>

#include "core_service/core_service.h"
#include "net_service/net_service.h"
#include "core_service/core_service_delegate.h"

namespace core {

const char kClientVersion[] = "2.0.1";

std::map<CoreServiceDelegate::ServerAPI, const char*> CoreServiceDelegate::m_server_API;

void CoreServiceDelegate::InitializeServerAPI() {
    if (1) {
        m_server_API[LoginAPI] = "https://47.107.14.251:16666/ruds/api/v1/auth/login.action";
        m_server_API[PrimaryFetchTokenAPI] = "https://47.107.14.251:16666/ruds/api/v1/primary/service/sign.action";
        m_server_API[SuperiorFetchTokenAPI] = "https://47.107.14.251:16666/ruds/api/v1/superior/service/sign.action";

        m_server_API[PrimaryGetMyInfoAPI] = "https://47.107.14.251:16666/ruds/api/v1/primary/getMyInfo.action";
        m_server_API[SuperiorGetMyInfoAPI] = "https://47.107.14.251:16666/ruds/api/v1/superior/getMyInfo.action";
        m_server_API[UpdatePasswordAPI] = "https://47.107.14.251:16666/ruds/api/v1/role/updatepwd.action";
        m_server_API[QuerySubscriberSuperiorUserAPI] = "https://47.107.14.251:16666/ruds/api/v1/admin/superior/queryPubs.action";
        m_server_API[QuerySuperiorUserAPI] = "https://47.107.14.251:16666/ruds/api/v1/admin/superior/query.action";
        m_server_API[QueryPrimaryUserInfoAPI] = "https://47.107.14.251:16666/ruds/api/v1/admin/primary/query.action";
        m_server_API[UpdatePrimaryUserInfoAPI] = "https://47.107.14.251:16666/ruds/api/v1/admin/primary/update.action";
        m_server_API[UpdateSuperiorUserInfoAPI] = "https://47.107.14.251:16666/ruds/api/v1/admin/superior/update.action";

        m_server_API[CreateReportAPI] = "https://47.107.14.251:16666/ruds/api/v2/report/create.action";
        m_server_API[UpdateReportAPI] = "https://47.107.14.251:16666/ruds/api/v2/report/update.action";
		m_server_API[ConfirmReportAPI] = "https://47.107.14.251:16666/ruds/api/v2/report/confirm.action";
        m_server_API[QueryReportAPI] = "https://47.107.14.251:16666/ruds/api/v2/admin/report/query.action";
        m_server_API[QuerySubscriberSuperiorUserOnlineAPI] = "https://47.107.14.251:16666/ruds/api/v1/role/online/querySub.action";

        m_server_API[CheckAppUpgradeAPI] = "https://47.107.14.251:16666/ruds/api/v1/app/query.action";

        m_server_API[QueryControlPrimaryUserAPI] = "https://47.107.14.251:16666/ruds/api/v1/superior/getMyControls.action";
        m_server_API[GetTemplateAPI] = "https://47.107.14.251:16666/ruds/api/v2/template/all/get.action";
        m_server_API[QueryTemplateAPI] = "https://47.107.14.251:16666/ruds/api/v2/template/query.action";
        m_server_API[CreateTemplateAPI] = "https://47.107.14.251:16666/ruds/api/v2/template/create.action";
        m_server_API[UpdateTemplateAPI] = "https://47.107.14.251:16666/ruds/api/v2/template/update.action";
        m_server_API[DeleteTemplateAPI] = "https://47.107.14.251:16666/ruds/api/v2/template/delete.action";

    }
    else {
        m_server_API[LoginAPI] = "http://47.106.176.78:16666/ruds/api/v1/auth/login.action";
        m_server_API[PrimaryFetchTokenAPI] = "http://47.106.176.78:16666/ruds/api/v1/primary/service/sign.action";
        m_server_API[SuperiorFetchTokenAPI] = "http://47.106.176.78:16666/ruds/api/v1/superior/service/sign.action";

        m_server_API[PrimaryGetMyInfoAPI] = "http://47.106.176.78:16666/ruds/api/v1/primary/getMyInfo.action";
        m_server_API[SuperiorGetMyInfoAPI] = "http://47.106.176.78:16666/ruds/api/v1/superior/getMyInfo.action";
        m_server_API[UpdatePasswordAPI] = "http://47.106.176.78:16666/ruds/api/v1/role/updatepwd.action";
        m_server_API[QuerySubscriberSuperiorUserAPI] = "http://47.106.176.78:16666/ruds/api/v1/admin/superior/queryPubs.action";
        m_server_API[QuerySuperiorUserAPI] = "http://47.106.176.78:16666/ruds/api/v1/admin/superior/query.action";
        m_server_API[QueryPrimaryUserInfoAPI] = "http://47.107.14.251:16666/ruds/api/v1/admin/primary/query.action";
        m_server_API[UpdatePrimaryUserInfoAPI] = "http://47.106.176.78:16666/ruds/api/v1/admin/primary/update.action";
        m_server_API[UpdateSuperiorUserInfoAPI] = "http://47.106.176.78:16666/ruds/api/v1/admin/superior/update.action";

        m_server_API[CreateReportAPI] = "http://47.106.176.78:16666/ruds/api/v2/report/create.action";
        m_server_API[UpdateReportAPI] = "http://47.106.176.78:16666/ruds/api/v2/report/update.action";
		m_server_API[ConfirmReportAPI] = "http://47.106.176.78:16666/ruds/api/v2/report/confirm.action";
        m_server_API[QueryReportAPI] = "http://47.106.176.78:16666/ruds/api/v2/admin/report/query.action";
        m_server_API[QuerySubscriberSuperiorUserOnlineAPI] = "http://47.106.176.78:16666/ruds/api/v1/role/online/querySub.action";

        m_server_API[CheckAppUpgradeAPI] = "http://47.106.176.78:16666/ruds/api/v1/app/query.action";

        m_server_API[QueryControlPrimaryUserAPI] = "http://47.106.176.78:16666/ruds/api/v1/superior/getMyControls.action";
        m_server_API[GetTemplateAPI] = "http://47.106.176.78:16666/ruds/api/v2/template/all/get.action";
        m_server_API[QueryTemplateAPI] = "http://47.106.176.78:16666/ruds/api/v2/template/query.action";
    }

}

void CoreServiceDelegate::PushActiveRequest(int64_t request_id, ServerAPI api) {
    std::lock_guard<std::mutex> guard(m_active_requests_mutex);
    m_active_requests[request_id] = api;
}

bool CoreServiceDelegate::PopActiveRequest(int64_t request_id, ServerAPI* api) {
    std::lock_guard<std::mutex> guard(m_active_requests_mutex);

    if (m_active_requests.count(request_id) > 0) {
        if (api)
            *api = m_active_requests[request_id];
        m_active_requests.erase(request_id);
        return true;
    }

    return false;
}

void CoreServiceDelegate::ClearActiveRequests() {
    std::lock_guard<std::mutex> guard(m_active_requests_mutex);
    m_active_requests.clear();
}

void CoreServiceDelegate::SetProxyInfo(const std::string & host, const int port, const std::string & username, const std::string & password, bool enabled)
{
    net::NetProxyInfo net_proxy_info;
    net_proxy_info.enabled = enabled;
    net_proxy_info.host = host;
    net_proxy_info.port = port;
    net_proxy_info.username = username;
    net_proxy_info.password = password;
    net::NetService::GetInstance()->SetProxyInfo(net_proxy_info);
}

void CoreServiceDelegate::Login(const std::string& uid, const std::string& password) {

    net::HttpRequest* request =
        net::NetService::GetInstance()->CreateRequest();

    PushActiveRequest(request->GetRequestId(), LoginAPI);

    request->SetEventHandler(this);
    request->SetMethod(net::HttpRequest::POST);

    std::string api_url = m_server_API[LoginAPI];

    request->SetUrl(api_url.c_str());

    Json::Value data;
    data["client_ver"] = kClientVersion;
    data["uid"] = uid.c_str();
    data["password"] = password.c_str();

    request->SetPostData(data.toStyledString());

    net::NetService::GetInstance()->StartRequest(request);
}

void CoreServiceDelegate::GetPrimaryUserInfo(const std::string& uid) {
    net::HttpRequest* request =
        net::NetService::GetInstance()->CreateRequest();

    PushActiveRequest(request->GetRequestId(), PrimaryGetMyInfoAPI);

    request->SetEventHandler(this);
    request->SetMethod(net::HttpRequest::GET);

    std::string api_url = m_server_API[PrimaryGetMyInfoAPI];

    request->SetUrl(api_url.c_str());

    request->AddHeader("Authorization", CoreService::GetInstance()->m_current_login_info.app_token);

    net::NetService::GetInstance()->StartRequest(request);
}

void CoreServiceDelegate::GetSuperiorUserInfo(const std::string& uid) {
    net::HttpRequest* request =
        net::NetService::GetInstance()->CreateRequest();

    PushActiveRequest(request->GetRequestId(), SuperiorGetMyInfoAPI);

    request->SetEventHandler(this);
    request->SetMethod(net::HttpRequest::GET);

    std::string api_url = m_server_API[SuperiorGetMyInfoAPI];

    request->SetUrl(api_url.c_str());

    request->AddHeader("Authorization", CoreService::GetInstance()->m_current_login_info.app_token);

    net::NetService::GetInstance()->StartRequest(request);
}

void CoreServiceDelegate::UpdatePassword(const std::string& old_password,
    const std::string& new_password) {
    net::HttpRequest* request =
        net::NetService::GetInstance()->CreateRequest();

    PushActiveRequest(request->GetRequestId(), UpdatePasswordAPI);

    request->SetEventHandler(this);
    request->SetMethod(net::HttpRequest::PUT);

    std::string api_url = m_server_API[UpdatePasswordAPI];

    request->SetUrl(api_url.c_str());

    Json::Value data;
    data["new_pwd"] = new_password.c_str();
    data["old_pwd"] = old_password.c_str();

    request->SetUpdateData(data.toStyledString());

    request->AddHeader("Authorization", CoreService::GetInstance()->m_current_login_info.app_token);

    net::NetService::GetInstance()->StartRequest(request);
}

void CoreServiceDelegate::QuerySubscriberSuperiorUser() {
    net::HttpRequest* request =
        net::NetService::GetInstance()->CreateRequest();

    PushActiveRequest(request->GetRequestId(), QuerySubscriberSuperiorUserAPI);

    request->SetEventHandler(this);
    request->SetMethod(net::HttpRequest::GET);

    std::string api_url = m_server_API[QuerySubscriberSuperiorUserAPI];
    int index = 0;
    for (int sub : CoreService::GetInstance()->m_current_primary_user.GetSubscriber()) {
        if (index == 0)
            api_url += "?";

        api_url += "publish=";
        api_url += std::to_string(sub);

        index++;
        if (index != CoreService::GetInstance()->m_current_primary_user.GetSubscriber().size())
            api_url += "&";
    }

    request->SetUrl(api_url.c_str());

    request->AddHeader("Authorization", CoreService::GetInstance()->m_current_login_info.app_token);

    net::NetService::GetInstance()->StartRequest(request);
}

void CoreServiceDelegate::QuerySuperiorUser(std::map<std::string, std::string> conditions) {
    net::HttpRequest* request =
        net::NetService::GetInstance()->CreateRequest();

    PushActiveRequest(request->GetRequestId(), QuerySuperiorUserAPI);

    request->SetEventHandler(this);
    request->SetMethod(net::HttpRequest::GET);

    std::string api_url = m_server_API[QuerySuperiorUserAPI];
    int index = 0;
    for (std::pair<std::string, std::string> param : conditions) {
        if (index == 0)
            api_url += "?";

        api_url += param.first;
        api_url += "=";
        api_url += param.second;

        index++;
        if (index != conditions.size())
            api_url += "&";
    }

    request->SetUrl(api_url.c_str());

    request->AddHeader("Authorization", CoreService::GetInstance()->m_current_login_info.app_token);

    net::NetService::GetInstance()->StartRequest(request);
}

void CoreServiceDelegate::QueryPrimaryUserInfo(const std::string& uid) {
    net::HttpRequest* request =
        net::NetService::GetInstance()->CreateRequest();

    PushActiveRequest(request->GetRequestId(), QueryPrimaryUserInfoAPI);

    request->SetEventHandler(this);
    request->SetMethod(net::HttpRequest::GET);

    std::string api_url = m_server_API[QueryPrimaryUserInfoAPI];
    api_url += "?";

    api_url += "uid=";
    api_url += uid;
    request->SetUrl(api_url.c_str());

    request->AddHeader("Authorization", CoreService::GetInstance()->m_current_login_info.app_token);

    net::NetService::GetInstance()->StartRequest(request);
}

void CoreServiceDelegate::UpdatePrimaryUserInfo(const Json::Value& data) {
    net::HttpRequest* request =
        net::NetService::GetInstance()->CreateRequest();

    PushActiveRequest(request->GetRequestId(), UpdatePrimaryUserInfoAPI);

    request->SetEventHandler(this);
    request->SetMethod(net::HttpRequest::POST);

    std::string api_url = m_server_API[UpdatePrimaryUserInfoAPI];

    request->SetUrl(api_url.c_str());

    request->SetPostData(data.toStyledString());

    request->AddHeader("Authorization", CoreService::GetInstance()->m_current_login_info.app_token);

    net::NetService::GetInstance()->StartRequest(request);
}

void CoreServiceDelegate::UpdateSuperiorUserInfo(const Json::Value& data) {
    net::HttpRequest* request =
        net::NetService::GetInstance()->CreateRequest();

    PushActiveRequest(request->GetRequestId(), UpdateSuperiorUserInfoAPI);

    request->SetEventHandler(this);
    request->SetMethod(net::HttpRequest::POST);

    std::string api_url = m_server_API[UpdateSuperiorUserInfoAPI];

    request->SetUrl(api_url.c_str());

    request->SetPostData(data.toStyledString());

    request->AddHeader("Authorization", CoreService::GetInstance()->m_current_login_info.app_token);

    net::NetService::GetInstance()->StartRequest(request);
}

void CoreServiceDelegate::QuerySubscriberSuperiorUserOnlineStatus(std::list<core::SuperiorUser> superior_users) {
    net::HttpRequest* request =
        net::NetService::GetInstance()->CreateRequest();

    PushActiveRequest(request->GetRequestId(), QuerySubscriberSuperiorUserOnlineAPI);

    request->SetEventHandler(this);
    request->SetMethod(net::HttpRequest::GET);

    std::string api_url = m_server_API[QuerySubscriberSuperiorUserOnlineAPI];

    int index = 0;
    for (core::SuperiorUser user : superior_users) {
        if (index == 0)
            api_url += "?";

        api_url += "uid=";
        api_url += user.GetUid();

        index++;
        if (index != superior_users.size())
            api_url += "&";
    }
    request->SetUrl(api_url.c_str());

    request->AddHeader("Authorization", CoreService::GetInstance()->m_current_login_info.app_token);

    net::NetService::GetInstance()->StartRequest(request);
}

void CoreServiceDelegate::CreateReport(const Json::Value& data) {
    net::HttpRequest* request =
        net::NetService::GetInstance()->CreateRequest();

    PushActiveRequest(request->GetRequestId(), CreateReportAPI);

    request->SetEventHandler(this);
    request->SetMethod(net::HttpRequest::POST);

    std::string api_url = m_server_API[CreateReportAPI];

    request->SetUrl(api_url.c_str());

    request->SetPostData(data.toStyledString());

    request->AddHeader("Authorization", CoreService::GetInstance()->m_current_login_info.app_token);

    net::NetService::GetInstance()->StartRequest(request);
}

void CoreServiceDelegate::UpdateReport(const Json::Value& data) {
    net::HttpRequest* request =
        net::NetService::GetInstance()->CreateRequest();

    PushActiveRequest(request->GetRequestId(), UpdateReportAPI);

    request->SetEventHandler(this);
    request->SetMethod(net::HttpRequest::POST);

    std::string api_url = m_server_API[UpdateReportAPI];

    request->SetUrl(api_url.c_str());

    request->SetPostData(data.toStyledString());

    request->AddHeader("Authorization", CoreService::GetInstance()->m_current_login_info.app_token);

    net::NetService::GetInstance()->StartRequest(request);
}

void CoreServiceDelegate::ConfirmReport(const Json::Value& data, const std::string& peer_uid) {
	net::HttpRequest* request =
		net::NetService::GetInstance()->CreateRequest();

	PushActiveRequest(request->GetRequestId(), ConfirmReportAPI);

	request->SetEventHandler(this);
	request->SetMethod(net::HttpRequest::POST);

	std::string api_url = m_server_API[ConfirmReportAPI];

	request->SetUrl(api_url.c_str());

	request->SetPostData(data.toStyledString());

	request->SetPeerUid(peer_uid);

	request->AddHeader("Authorization", CoreService::GetInstance()->m_current_login_info.app_token);

	net::NetService::GetInstance()->StartRequest(request);
}

void CoreServiceDelegate::QueryReport(std::map<std::string, std::string> conditions) {
    net::HttpRequest* request =
        net::NetService::GetInstance()->CreateRequest();

    PushActiveRequest(request->GetRequestId(), QueryReportAPI);

    request->SetEventHandler(this);
    request->SetMethod(net::HttpRequest::GET);

    std::string api_url = m_server_API[QueryReportAPI];
    int index = 0;
    for (std::pair<std::string, std::string> param : conditions) {
        if (index == 0)
            api_url += "?";

        api_url += param.first;
        api_url += "=";
        api_url += param.second;

        index++;
        if (index != conditions.size())
            api_url += "&";
    }

    request->SetUrl(api_url.c_str());

    request->AddHeader("Authorization", CoreService::GetInstance()->m_current_login_info.app_token);

    net::NetService::GetInstance()->StartRequest(request);
}

void CoreServiceDelegate::GetTemplates(std::map<std::string, std::string> conditions)
{
    net::HttpRequest* request =
        net::NetService::GetInstance()->CreateRequest();

    PushActiveRequest(request->GetRequestId(), GetTemplateAPI);

    request->SetEventHandler(this);
    request->SetMethod(net::HttpRequest::GET);

    std::string api_url = m_server_API[GetTemplateAPI];
    int index = 0;
    for (std::pair<std::string, std::string> param : conditions) {
        if (index == 0)
            api_url += "?";

        api_url += param.first;
        api_url += "=";
        api_url += param.second;

        index++;
        if (index != conditions.size())
            api_url += "&";
    }

    request->SetUrl(api_url.c_str());

    request->AddHeader("Authorization", CoreService::GetInstance()->m_current_login_info.app_token);

    net::NetService::GetInstance()->StartRequest(request);
}

void CoreServiceDelegate::CreateTemplate(const Json::Value & data)
{
    net::HttpRequest* request =
        net::NetService::GetInstance()->CreateRequest();

    PushActiveRequest(request->GetRequestId(), CreateTemplateAPI);

    request->SetEventHandler(this);
    request->SetMethod(net::HttpRequest::POST);

    std::string api_url = m_server_API[CreateTemplateAPI];

    request->SetUrl(api_url.c_str());

    request->SetPostData(data.toStyledString());

    request->AddHeader("Authorization", CoreService::GetInstance()->m_current_login_info.app_token);

    net::NetService::GetInstance()->StartRequest(request);
}

void CoreServiceDelegate::UpdateTemplate(const Json::Value & data)
{
    net::HttpRequest* request =
        net::NetService::GetInstance()->CreateRequest();

    PushActiveRequest(request->GetRequestId(), UpdateTemplateAPI);

    request->SetEventHandler(this);
    request->SetMethod(net::HttpRequest::PUT);

    std::string api_url = m_server_API[UpdateTemplateAPI];

    request->SetUrl(api_url.c_str());

	request->SetUpdateData(data.toStyledString());

    request->AddHeader("Authorization", CoreService::GetInstance()->m_current_login_info.app_token);

    net::NetService::GetInstance()->StartRequest(request);
}

void CoreServiceDelegate::QueryTemplate(std::map<std::string, std::string> conditions)
{
    net::HttpRequest* request =
        net::NetService::GetInstance()->CreateRequest();

    PushActiveRequest(request->GetRequestId(), QueryTemplateAPI);

    request->SetEventHandler(this);
    request->SetMethod(net::HttpRequest::GET);

    std::string api_url = m_server_API[QueryTemplateAPI];
    int index = 0;
    for (std::pair<std::string, std::string> param : conditions) {
        if (index == 0)
            api_url += "?";

        api_url += param.first;
        api_url += "=";
        api_url += param.second;

        index++;
        if (index != conditions.size())
            api_url += "&";
    }

    request->SetUrl(api_url.c_str());

    request->AddHeader("Authorization", CoreService::GetInstance()->m_current_login_info.app_token);

    net::NetService::GetInstance()->StartRequest(request);
}

void CoreServiceDelegate::DeleteTemplate(std::map<std::string, std::string> conditions)
{
    net::HttpRequest* request =
        net::NetService::GetInstance()->CreateRequest();

    PushActiveRequest(request->GetRequestId(), UpdateTemplateAPI);

    request->SetEventHandler(this);
    request->SetMethod(net::HttpRequest::DELETE_METHOD);

    std::string api_url = m_server_API[UpdateTemplateAPI];

    request->SetUrl(api_url.c_str());

    request->SetPostData("");

    request->AddHeader("Authorization", CoreService::GetInstance()->m_current_login_info.app_token);

    net::NetService::GetInstance()->StartRequest(request);
}

void CoreServiceDelegate::CheckUpgrade(const std::string & app)
{
    net::HttpRequest* request =
        net::NetService::GetInstance()->CreateRequest();

    PushActiveRequest(request->GetRequestId(), CheckAppUpgradeAPI);

    request->SetEventHandler(this);
    request->SetMethod(net::HttpRequest::GET);

    std::string api_url = m_server_API[CheckAppUpgradeAPI];

    api_url += "?";
    api_url += "app=";
    api_url += app;

    request->SetUrl(api_url.c_str());

    request->AddHeader("Authorization", CoreService::GetInstance()->m_current_login_info.app_token);

    net::NetService::GetInstance()->StartRequest(request);
}

void CoreServiceDelegate::QueryControlPrimaryUser() {
    net::HttpRequest* request =
        net::NetService::GetInstance()->CreateRequest();

    PushActiveRequest(request->GetRequestId(), QueryControlPrimaryUserAPI);

    request->SetEventHandler(this);
    request->SetMethod(net::HttpRequest::GET);

    std::string api_url = m_server_API[QueryControlPrimaryUserAPI];
    request->SetUrl(api_url.c_str());

    request->AddHeader("Authorization", CoreService::GetInstance()->m_current_login_info.app_token);

    net::NetService::GetInstance()->StartRequest(request);
}

void CoreServiceDelegate::OnResponse(net::HttpRequest* request) {
    if (!request) {
        assert(false);
        return;
    }

    ServerAPI api;
    if (PopActiveRequest(request->GetRequestId(), &api)) {
        switch (api)
        {
        case core::CoreServiceDelegate::LoginAPI:
            HandleLoginAPIResponse(request);
            break;
        case core::CoreServiceDelegate::PrimaryFetchTokenAPI:
            HandlePrimaryFetchTokenAPIResponse(request);
            break;
        case core::CoreServiceDelegate::SuperiorFetchTokenAPI:
            HandleSuperiorFetchTokenAPIResponse(request);
            break;
        case core::CoreServiceDelegate::PrimaryGetMyInfoAPI:
            HandlePrimaryGetMyInfoAPIResponse(request);
            break;
        case core::CoreServiceDelegate::SuperiorGetMyInfoAPI:
            HandleSuperiorGetMyInfoAPIResponse(request);
            break;
        case core::CoreServiceDelegate::UpdatePasswordAPI:
            HandleUpdatePasswordAPIResponse(request);
            break;
        case core::CoreServiceDelegate::QuerySubscriberSuperiorUserAPI:
            HandleQuerySubscriberSuperiorUserAPIResponse(request);
            break;
        case core::CoreServiceDelegate::QuerySuperiorUserAPI:
            HandleQuerySuperiorUserAPIResponse(request);
            break;
        case core::CoreServiceDelegate::QueryPrimaryUserInfoAPI:
            HandleQueryPrimaryUserInfoAPIResponse(request);
            break;
        case core::CoreServiceDelegate::UpdatePrimaryUserInfoAPI:
            HandleUpdatePrimaryUserInfoAPIResponse(request);
            break;
        case core::CoreServiceDelegate::UpdateSuperiorUserInfoAPI:
            HandleUpdateSuperiorUserInfoAPIResponse(request);
            break;
		case core::CoreServiceDelegate::CreateReportAPI:
			HandleCreateReportAPIResponse(request);
			break;
		case core::CoreServiceDelegate::UpdateReportAPI:
			HandleUpdateReportAPIResponse(request);
			break;
		case core::CoreServiceDelegate::ConfirmReportAPI:
			HandleConfirmReportAPIResponse(request);
			break;
		case core::CoreServiceDelegate::QueryReportAPI:
			HandleQueryReportAPIResponse(request);
			break;
        case core::CoreServiceDelegate::QuerySubscriberSuperiorUserOnlineAPI:
            HandleQuerySubscriberSuperiorUserOnlineStatusAPIResponse(request);
            break;
        case core::CoreServiceDelegate::CheckAppUpgradeAPI:
            HandleCheckAppUpgradeAPIResponse(request);
            break;
        case core::CoreServiceDelegate::QueryControlPrimaryUserAPI:
            HandleQueryControlPrimaryUserAPIResponse(request);
            break;
        case core::CoreServiceDelegate::GetTemplateAPI:
            HandleGetTemplateAPIResponse(request);
            break;
        case core::CoreServiceDelegate::QueryTemplateAPI:
            HandleQueryTemplateAPIResponse(request);
            break;
        case core::CoreServiceDelegate::CreateTemplateAPI:
            HandleCreateTemplateAPIResponse(request);
            break;
        case core::CoreServiceDelegate::UpdateTemplateAPI:
            HandleUpdateTemplateAPIResponse(request);
            break;
        case core::CoreServiceDelegate::DeleteTemplateAPI:
            HandleDeleteTemplateAPIResponse(request);
            break;
        default:
            assert(false);
            break;
        }
    }
}

void CoreServiceDelegate::FetchPrimaryToken() {
    net::HttpRequest* request =
        net::NetService::GetInstance()->CreateRequest();

    PushActiveRequest(request->GetRequestId(), PrimaryFetchTokenAPI);

    request->SetEventHandler(this);
    request->SetMethod(net::HttpRequest::GET);

    std::string api_url = m_server_API[PrimaryFetchTokenAPI];

    request->SetUrl(api_url.c_str());

    request->AddHeader("Authorization", CoreService::GetInstance()->m_current_login_info.app_token);

    net::NetService::GetInstance()->StartRequest(request);
}

void CoreServiceDelegate::FetchSuperiorToken() {
    net::HttpRequest* request =
        net::NetService::GetInstance()->CreateRequest();

    PushActiveRequest(request->GetRequestId(), SuperiorFetchTokenAPI);

    request->SetEventHandler(this);
    request->SetMethod(net::HttpRequest::GET);

    std::string api_url = m_server_API[SuperiorFetchTokenAPI];

    request->SetUrl(api_url.c_str());

    request->AddHeader("Authorization", CoreService::GetInstance()->m_current_login_info.app_token);

    net::NetService::GetInstance()->StartRequest(request);
}

void CoreServiceDelegate::HandleLoginAPIResponse(net::HttpRequest* request) {
    if (request->GetErrorCode() == 0 && request->GetResponseCode() == 200) {
        Json::Value value;
        if (request->GetResponseJson(value)) {
            std::string app_token = value["app_token"].asString();

            Json::Value user_info = value["userinfo"];
            std::string uid = user_info["uid"].asString();
            CoreService::Role role = (CoreService::Role)user_info["role"].asInt();

            if (CoreService::GetInstance()->m_current_login_info.role != role) {
                CoreService::GetInstance()->OnCoreLoginFailed(LoginEventHandler::ErrorRole, "");
                return;
            }

            assert(CoreService::GetInstance()->m_current_login_info.uid == uid);

            if (!uid.empty() && !app_token.empty()) {
                CoreService::GetInstance()->m_current_login_info.app_token = "Bearer " + app_token;

                CoreService::GetInstance()->m_current_login_info.is_valid = true;
                CoreService::GetInstance()->OnCoreLoginSuccessed();

                if (CoreService::GetInstance()->m_current_login_info.role == CoreService::Primary)
                    FetchPrimaryToken();
                else if (CoreService::GetInstance()->m_current_login_info.role == CoreService::Superior)
                    FetchSuperiorToken();
            }
            else {
                CoreService::GetInstance()->m_current_login_info.is_valid = false;
                CoreService::GetInstance()->OnCoreLoginFailed(LoginEventHandler::ErrorUnknown, "");
            }

            return;
        }
    }

    ErrorInfo info;
    GetErrorInfo(request, &info);
    CoreService::GetInstance()->OnCoreLoginFailed(info.error_code, info.message);
}

void CoreServiceDelegate::HandlePrimaryFetchTokenAPIResponse(net::HttpRequest* request) {
    if (request->GetErrorCode() == 0 && request->GetResponseCode() == 200) {
        Json::Value value;
        if (request->GetResponseJson(value)) {
            std::string nim_token = value["nim_token"].asString();
            std::string oss_key = value["oss_key"].asString();
            std::string oss_secret = value["oss_secret"].asString();

            std::string uid = value["uid"].asString();
            CoreService::Role role = (CoreService::Role)value["role"].asInt();

            assert(CoreService::GetInstance()->m_current_login_info.role == role);
            assert(CoreService::GetInstance()->m_current_login_info.uid == uid);

            if (!nim_token.empty() && !oss_key.empty() && !oss_secret.empty()) {
                CoreService::GetInstance()->m_current_login_info.nim_token = nim_token;
                CoreService::GetInstance()->m_current_login_info.oss_key = oss_key;
                CoreService::GetInstance()->m_current_login_info.oss_secret = oss_secret;

                CoreService::GetInstance()->m_current_login_info.is_valid = true;
                CoreService::GetInstance()->OnFetchTokenSuccessed();
            }
            else {
                CoreService::GetInstance()->m_current_login_info.is_valid = false;
                CoreService::GetInstance()->OnFetchTokenFailed(LoginEventHandler::ErrorUnknown, "");
            }

            return;
        }
    }

    ErrorInfo info;
    GetErrorInfo(request, &info);
    CoreService::GetInstance()->OnFetchTokenFailed(info.error_code, info.message);
}

void CoreServiceDelegate::HandleSuperiorFetchTokenAPIResponse(net::HttpRequest* request) {
    if (request->GetErrorCode() == 0 && request->GetResponseCode() == 200) {
        Json::Value value;
        if (request->GetResponseJson(value)) {
            std::string nim_token = value["nim_token"].asString();
            std::string oss_key = value["oss_key"].asString();
            std::string oss_secret = value["oss_secret"].asString();

            std::string uid = value["uid"].asString();
            CoreService::Role role = (CoreService::Role)value["role"].asInt();

            //assert(CoreService::GetInstance()->m_current_login_info.role == role);
            assert(CoreService::GetInstance()->m_current_login_info.uid == uid);

            if (!nim_token.empty() && !oss_key.empty() && !oss_secret.empty()) {
                CoreService::GetInstance()->m_current_login_info.nim_token = nim_token;
                CoreService::GetInstance()->m_current_login_info.oss_key = oss_key;
                CoreService::GetInstance()->m_current_login_info.oss_secret = oss_secret;

                CoreService::GetInstance()->m_current_login_info.is_valid = true;
                CoreService::GetInstance()->OnFetchTokenSuccessed();
            }
            else {
                CoreService::GetInstance()->m_current_login_info.is_valid = false;
                CoreService::GetInstance()->OnFetchTokenFailed(LoginEventHandler::ErrorUnknown, "");
            }

            return;
        }
    }

    ErrorInfo info;
    GetErrorInfo(request, &info);
    CoreService::GetInstance()->OnFetchTokenFailed(info.error_code, info.message);
}

void CoreServiceDelegate::HandlePrimaryGetMyInfoAPIResponse(net::HttpRequest* request) {

    if (request->GetErrorCode() == 0 && request->GetResponseCode() == 200) {
        Json::Value value;
        if (request->GetResponseJson(value)) {
            if (CoreService::GetInstance()->m_current_primary_user.LoadFromJson(value)) {
                CoreService::GetInstance()->OnGetPrimaryUserInfoSuccessed(
                    CoreService::GetInstance()->m_current_primary_user);
                return;
            }
        }
    }

    ErrorInfo info;
    GetErrorInfo(request, &info);
    CoreService::GetInstance()->OnGetPrimaryUserInfoFailed(info.error_code, info.message);
}

void CoreServiceDelegate::HandleSuperiorGetMyInfoAPIResponse(net::HttpRequest* request) {
    if (request->GetErrorCode() == 0 && request->GetResponseCode() == 200) {
        Json::Value value;
        if (request->GetResponseJson(value)) {
            if (CoreService::GetInstance()->m_current_superior_user.LoadFromJson(value)) {
                CoreService::GetInstance()->OnGetSuperiorUserInfoSuccessed(
                    CoreService::GetInstance()->m_current_superior_user);
                return;
            }
        }
    }

    ErrorInfo info;
    GetErrorInfo(request, &info);
    CoreService::GetInstance()->OnGetSuperiorUserInfoFailed(info.error_code, info.message);
}

void CoreServiceDelegate::HandleUpdatePasswordAPIResponse(net::HttpRequest* request) {
    if (request->GetErrorCode() == 0 && request->GetResponseCode() == 200) {
        CoreService::GetInstance()->OnUpdatePasswordSuccessed("", "");
        return;
    }

    ErrorInfo info;
    GetErrorInfo(request, &info);
    CoreService::GetInstance()->OnUpdatePasswordFailed(info.error_code, info.message);
}

void CoreServiceDelegate::HandleQuerySubscriberSuperiorUserAPIResponse(net::HttpRequest* request) {
    if (request->GetErrorCode() == 0 && request->GetResponseCode() == 200) {
        // {"superiors":[],"count":0}
        Json::Value value;
        if (request->GetResponseJson(value)) {
            Json::Value superiors = value["superiors"];
            int count = value["count"].asInt();
            std::list<SuperiorUser> users;
            for (Json::Value& json_user : superiors) {
                SuperiorUser user;
                user.LoadFromJson(json_user);
                users.push_back(user);
            }

            CoreService::GetInstance()->OnQuerySubscriberSuperiorUserSuccessed(users);
            return;
        }
    }

    ErrorInfo info;
    GetErrorInfo(request, &info);
    CoreService::GetInstance()->OnQuerySubscriberSuperiorUserFailed(info.error_code, info.message);
}

void CoreServiceDelegate::HandleQuerySuperiorUserAPIResponse(net::HttpRequest* request) {
    if (request->GetErrorCode() == 0 && request->GetResponseCode() == 200) {
        // {"superiors":[],"count":0}
        Json::Value value;
        if (request->GetResponseJson(value)) {
            Json::Value superiors = value["superiors"];
            int count = value["count"].asInt();
            std::list<SuperiorUser> users;
            for (Json::Value& json_user : superiors) {
                SuperiorUser user;
                user.LoadFromJson(json_user);
                users.push_back(user);
            }

            CoreService::GetInstance()->OnQuerySuperiorUserSuccessed(users);
            return;
        }
    }

    ErrorInfo info;
    GetErrorInfo(request, &info);
    CoreService::GetInstance()->OnQuerySuperiorUserFailed(info.error_code, info.message);
}

void CoreServiceDelegate::HandleQueryPrimaryUserInfoAPIResponse(net::HttpRequest * request)
{
    if (request->GetErrorCode() == 0 && request->GetResponseCode() == 200) {
        Json::Value value;
        if (request->GetResponseJson(value)) {
            Json::Value primaries = value["primaries"];
            PrimaryUser user;
            for (Json::Value& json_user : primaries) {
                if (user.LoadFromJson(json_user)) {
                    CoreService::GetInstance()->OnGetPrimaryUserInfoSuccessed(
                        user);
                    return;
                }
            }
        }
    }

    ErrorInfo info;
    GetErrorInfo(request, &info);
    CoreService::GetInstance()->OnGetPrimaryUserInfoFailed(info.error_code, info.message);
}

void CoreServiceDelegate::HandleUpdatePrimaryUserInfoAPIResponse(net::HttpRequest* request) {
    if (request->GetErrorCode() == 0 && request->GetResponseCode() == 200) {
        CoreService::GetInstance()->OnUpdatePrimaryUserInfoSuccessed();
        return;
    }

    ErrorInfo info;
    GetErrorInfo(request, &info);
    CoreService::GetInstance()->OnUpdatePrimaryUserInfoFailed(info.error_code, info.message);
}

void CoreServiceDelegate::HandleUpdateSuperiorUserInfoAPIResponse(net::HttpRequest* request) {
    if (request->GetErrorCode() == 0 && request->GetResponseCode() == 200) {
        CoreService::GetInstance()->OnUpdateSuperiorUserInfoSuccessed();
        return;
    }

    ErrorInfo info;
    GetErrorInfo(request, &info);
    CoreService::GetInstance()->OnUpdateSuperiorUserInfoFailed(info.error_code, info.message);
}

void CoreServiceDelegate::HandleCreateReportAPIResponse(net::HttpRequest* request) {
    if (request->GetErrorCode() == 0 && request->GetResponseCode() == 200) {
        CoreService::GetInstance()->OnCreateReportSuccessed();
        return;
    }

    ErrorInfo info;
    GetErrorInfo(request, &info);
    CoreService::GetInstance()->OnCreateReportFailed(info.error_code, info.message);
}

void CoreServiceDelegate::HandleUpdateReportAPIResponse(net::HttpRequest* request) {
    if (request->GetErrorCode() == 0 && request->GetResponseCode() == 200) {
        CoreService::GetInstance()->OnUpdateReportSuccessed();
        return;
    }

    ErrorInfo info;
    GetErrorInfo(request, &info);
    CoreService::GetInstance()->OnUpdateReportFailed(info.error_code, info.message);
}

void CoreServiceDelegate::HandleConfirmReportAPIResponse(net::HttpRequest* request) {
	if (request->GetErrorCode() == 0 && request->GetResponseCode() == 200) {
		Json::Value value;
		Json::Reader reader;
		reader.parse(request->GetPostData(), value);

		ConfirmReportData data;
		data.superior_id = CoreService::GetInstance()->GetCurrentSuperiorUser().GetUid();
		data.superior_name = CoreService::GetInstance()->GetCurrentSuperiorUser().GetName();
		data.uid = value["uid"].asString();
		data.content = value["content"].asString();
		data.conclusion = value["conclusion"].asString();
		data.confirm_status = (CONFIRM_STATUS)value["confirm_status"].asInt();
		data.confirm_msg = value["confirm_msg"].asString();
		data.primary_id = request->GetPeerUid();

		CoreService::GetInstance()->OnConfirmReportSuccessed(data);
		return;
	}

	ErrorInfo info;
	GetErrorInfo(request, &info);
	CoreService::GetInstance()->OnConfirmReportFailed(info.error_code, info.message);
}

void CoreServiceDelegate::HandleQueryReportAPIResponse(net::HttpRequest* request) {
    if (request->GetErrorCode() == 0 && request->GetResponseCode() == 200) {

        // {"reports":[],"count":0}
        Json::Value value;
        if (request->GetResponseJson(value)) {
            Json::Value json_reports = value["reports"];
            int count = value["count"].asInt();
            std::list<Report> reports;
            for (Json::Value& json_report : json_reports) {
                Report report;
                report.LoadFromJson(json_report);
                reports.push_back(report);
            }

            CoreService::GetInstance()->OnQueryReportSuccessed(reports);
            return;
        }
    }

    ErrorInfo info;
    GetErrorInfo(request, &info);
    CoreService::GetInstance()->OnQueryReportFailed(info.error_code, info.message);
}

void CoreServiceDelegate::HandleQuerySubscriberSuperiorUserOnlineStatusAPIResponse(net::HttpRequest* request) {
    if (request->GetErrorCode() == 0 && request->GetResponseCode() == 200) {

        // {"reports":[],"count":0}
        Json::Value value;
        if (request->GetResponseJson(value)) {
            Json::Value roles = value["roles"];
            int count = value["count"].asInt();
            std::list<SuperiorUserStatus> users_status;
            for (Json::Value& json_role : roles) {
                SuperiorUserStatus user;
                user.LoadFromJson(json_role);
                users_status.push_back(user);
            }

            CoreService::GetInstance()->OnQuerySubscriberSuperiorUserOnlineStatusSuccessed(users_status);
            return;
        }
    }

    ErrorInfo info;
    GetErrorInfo(request, &info);
    CoreService::GetInstance()->OnQueryReportFailed(info.error_code, info.message);
}

void CoreServiceDelegate::HandleCheckAppUpgradeAPIResponse(net::HttpRequest* request) {
    if (request->GetErrorCode() == 0 && request->GetResponseCode() == 200) {

        Json::Value value;
        if (request->GetResponseJson(value)) {
            AppUpgradeInfo upgradeInfo;
            upgradeInfo.LoadFromJson(value);
            CoreService::GetInstance()->OnCheckUpgradeSuccessed(upgradeInfo);
            return;
        }
    }

    ErrorInfo info;
    GetErrorInfo(request, &info);
    CoreService::GetInstance()->OnCheckUpgradeFailed(info.error_code, info.message);
}

void CoreServiceDelegate::HandleQueryControlPrimaryUserAPIResponse(net::HttpRequest* request) {
    if (request->GetErrorCode() == 0 && request->GetResponseCode() == 200) {
        // {"primaries":[],"count":0}
        Json::Value value;
        if (request->GetResponseJson(value)) {
            Json::Value superiors = value["primaries"];
            int count = value["count"].asInt();
            std::list<PrimaryUser> users;
            for (Json::Value& json_user : superiors) {
                PrimaryUser user;
                user.LoadFromJson(json_user);
                users.push_back(user);
            }

            CoreService::GetInstance()->OnQueryControlPrimaryUserSuccessed(users);
            return;
        }
    }

    ErrorInfo info;
    GetErrorInfo(request, &info);
    CoreService::GetInstance()->OnQueryControlPrimaryUserFailed(info.error_code, info.message);
}

void CoreServiceDelegate::HandleGetTemplateAPIResponse(net::HttpRequest* request)
{
    if (request->GetErrorCode() == 0 && request->GetResponseCode() == 200) {
        // {"primaries":[],"count":0}
        Json::Value value;
        if (request->GetResponseJson(value)) {
            Json::Value report_templates = value["content"];
            std::list<ReportTemplate> report_template_list;
            for (Json::Value& report_template : report_templates) {
                ReportTemplate rt;
                rt.LoadFromJson(report_template);
                report_template_list.push_back(rt);
            }

            CoreService::GetInstance()->OnGetTemplateSuccessed(report_template_list);
            return;
        }
    }

    ErrorInfo info;
    GetErrorInfo(request, &info);
    CoreService::GetInstance()->OnGetTemplateFailed(info.error_code, info.message);

}

void CoreServiceDelegate::HandleQueryTemplateAPIResponse(net::HttpRequest * request)
{
    if (request->GetErrorCode() == 0 && request->GetResponseCode() == 200) {
        // {"primaries":[],"count":0}
        Json::Value value;
        if (request->GetResponseJson(value)) {
            Json::Value report_templates = value["content"];
            std::list<ReportTemplate> report_template_list;
            for (Json::Value& report_template : report_templates) {
                ReportTemplate rt;
                rt.LoadFromJson(report_template);
                report_template_list.push_back(rt);
            }

            CoreService::GetInstance()->OnQueryTemplateSuccessed(report_template_list);
            return;
        }
    }

    ErrorInfo info;
    GetErrorInfo(request, &info);
    CoreService::GetInstance()->OnQueryTemplateFailed(info.error_code, info.message);
}

void CoreServiceDelegate::HandleCreateTemplateAPIResponse(net::HttpRequest * request)
{
    if (request->GetErrorCode() == 0 && request->GetResponseCode() == 200) {
        CoreService::GetInstance()->OnCreateTemplateSuccessed();
        return;
    }

    ErrorInfo info;
    GetErrorInfo(request, &info);
    CoreService::GetInstance()->OnCreateTemplateFailed(info.error_code, info.message);
}

void CoreServiceDelegate::HandleUpdateTemplateAPIResponse(net::HttpRequest * request)
{
    if (request->GetErrorCode() == 0 && request->GetResponseCode() == 200) {
        CoreService::GetInstance()->OnUpdateTemplateSuccessed();
        return;
    }

    ErrorInfo info;
    GetErrorInfo(request, &info);
    CoreService::GetInstance()->OnUpdateTemplateFailed(info.error_code, info.message);
}

void CoreServiceDelegate::HandleDeleteTemplateAPIResponse(net::HttpRequest * request)
{
    if (request->GetErrorCode() == 0 && request->GetResponseCode() == 200) {
        CoreService::GetInstance()->OnDeleteTemplateSuccessed();
        return;
    }

    ErrorInfo info;
    GetErrorInfo(request, &info);
    CoreService::GetInstance()->OnDeleteTemplateFailed(info.error_code, info.message);
}

bool CoreServiceDelegate::GetErrorInfo(net::HttpRequest* request, ErrorInfo* info) {
    Json::Value error;
    if (request->GetResponseJson(error) && info) {
        try {
            info->error_code = std::stoi(error["error_code"].asString());
        }
        catch (...) {
            info->error_code = -1;
        }

        info->message = error["message"].asString();

        return true;
    }

    return false;
}

} // namespace core