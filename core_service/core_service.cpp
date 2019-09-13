#include "core_service/core_service.h"

#include <assert.h>

#include <algorithm>
#include <chrono>
#include <fstream>

#include "core_service/core_service_delegate.h"
#include "json.h"
#include "net_service/net_service.h"

namespace core {

bool CoreService::Initialize() {
    bool res = net::NetService::GetInstance()->Initialize();
    if (!res) 
        return res;

    CoreServiceDelegate::InitializeServerAPI();

    m_delegate = new CoreServiceDelegate;

    return res;
}

void CoreService::Shutdown() {
    RemoveAllEventHandlers();

    if (m_delegate) {
        m_delegate->ClearActiveRequests();
        delete m_delegate;
        m_delegate = nullptr;
    }

    net::NetService::GetInstance()->Shutdown();
}

void CoreService::SetProxyInfo(const std::string & host, const int port, const std::string & username, const std::string & password, bool enabled)
{
	if (m_delegate) {
		m_delegate->SetProxyInfo(host, port, username, password, enabled);
	}
}

void CoreService::AddLoginEventHandler(LoginEventHandler* handler) {
    std::lock_guard<std::mutex> guard(m_login_handlers_mutex);
    m_login_handlers.push_back(handler);
}

void CoreService::RemoveLoginEventHandler(LoginEventHandler* handler) {
    std::lock_guard<std::mutex> guard(m_login_handlers_mutex);
    m_login_handlers.remove(handler);
}

void CoreService::AddUserEventHandler(UserEventHandler* handler) {
    std::lock_guard<std::mutex> guard(m_user_handlers_mutex);
    m_user_handlers.push_back(handler);
}

void CoreService::RemoveUserEventHandler(UserEventHandler* handler) {
    std::lock_guard<std::mutex> guard(m_user_handlers_mutex);
    m_user_handlers.remove(handler);
}

void CoreService::AddReportEventHandler(ReportEventHandler* handler) {
    std::lock_guard<std::mutex> guard(m_report_handlers_mutex);
    m_report_handlers.push_back(handler);
}

void CoreService::RemoveReportEventHandler(ReportEventHandler* handler) {
    std::lock_guard<std::mutex> guard(m_report_handlers_mutex);
    m_report_handlers.remove(handler);
}

void CoreService::AddUpgradeEventHandler(UpgradeEventHandler* handler)
{
	std::lock_guard<std::mutex> guard(m_upgrade_handlers_mutex);
	m_upgrade_handlers.push_back(handler);
}

void CoreService::RemoveUpgradeEventHandler(UpgradeEventHandler* handler)
{
	std::lock_guard<std::mutex> guard(m_upgrade_handlers_mutex);
	m_upgrade_handlers.remove(handler);
}

void CoreService::AddTemplateEventHandler(TemplateEventHandler * handler)
{
    std::lock_guard<std::mutex> guard(m_template_handlers_mutex);
    m_template_handlers.push_back(handler);
}

void CoreService::RemoveTemplateEventHandler(TemplateEventHandler * handler)
{
    std::lock_guard<std::mutex> guard(m_template_handlers_mutex);
    m_template_handlers.remove(handler);
}

void CoreService::RemoveAllEventHandlers() {
    {
        std::lock_guard<std::mutex> guard(m_login_handlers_mutex);
        m_login_handlers.clear();
    }
    {
        std::lock_guard<std::mutex> guard(m_user_handlers_mutex);
        m_user_handlers.clear();
    }
    {
        std::lock_guard<std::mutex> guard(m_report_handlers_mutex);
        m_report_handlers.clear();
    }
}

void CoreService::OnCoreLoginSuccessed() {
    std::lock_guard<std::mutex> guard(m_login_handlers_mutex);
    for (LoginEventHandler* handler : m_login_handlers) {
        handler->OnCoreLoginSuccessed();
    }
}

void CoreService::OnCoreLoginFailed(int error_code, const std::string& message) {
    std::lock_guard<std::mutex> guard(m_login_handlers_mutex);
    for (LoginEventHandler* handler : m_login_handlers) {
        handler->OnCoreLoginFailed(error_code, message);
    }
}

void CoreService::OnFetchTokenSuccessed() {
    std::lock_guard<std::mutex> guard(m_login_handlers_mutex);
    for (LoginEventHandler* handler : m_login_handlers) {
        handler->OnFetchTokenSuccessed();
    }
}

void CoreService::OnFetchTokenFailed(int error_code, const std::string& message) {
    std::lock_guard<std::mutex> guard(m_login_handlers_mutex);
    for (LoginEventHandler* handler : m_login_handlers) {
        handler->OnFetchTokenFailed(error_code, message);
    }
}

void CoreService::OnGetPrimaryUserInfoSuccessed(const PrimaryUser& user) {
	std::lock_guard<std::mutex> guard(m_user_handlers_mutex);
	for (UserEventHandler* handler : m_user_handlers) {
		handler->OnGetPrimaryUserInfoSuccessed(user);
	}
}

void CoreService::OnGetPrimaryUserInfoFailed(int error_code, const std::string& message) {
	std::lock_guard<std::mutex> guard(m_user_handlers_mutex);
	for (UserEventHandler* handler : m_user_handlers) {
		handler->OnGetPrimaryUserInfoFailed(error_code, message);
	}
}

void CoreService::OnGetSuperiorUserInfoSuccessed(const SuperiorUser& user) {
	std::lock_guard<std::mutex> guard(m_user_handlers_mutex);
	for (UserEventHandler* handler : m_user_handlers) {
		handler->OnGetSuperiorUserInfoSuccessed(user);
	}
}

void CoreService::OnGetSuperiorUserInfoFailed(int error_code, const std::string& message) {
	std::lock_guard<std::mutex> guard(m_user_handlers_mutex);
	for (UserEventHandler* handler : m_user_handlers) {
		handler->OnGetSuperiorUserInfoFailed(error_code, message);
	}
}

void CoreService::OnUpdatePasswordSuccessed(const std::string& uid,
  const std::string& new_password) {
	m_current_login_info.password = m_current_login_info.new_password;

	std::lock_guard<std::mutex> guard(m_user_handlers_mutex);
	for (UserEventHandler* handler : m_user_handlers) {
		handler->OnUpdatePasswordSuccessed(m_current_login_info.uid, m_current_login_info.password);
	}
}

void CoreService::OnUpdatePasswordFailed(int error_code, const std::string& message) {
	std::lock_guard<std::mutex> guard(m_user_handlers_mutex);
	for (UserEventHandler* handler : m_user_handlers) {
		handler->OnUpdatePasswordFailed(error_code, message);
	}
}

void CoreService::OnQuerySubscriberSuperiorUserSuccessed(std::list<SuperiorUser> users) {
	std::lock_guard<std::mutex> guard(m_user_handlers_mutex);
	for (UserEventHandler* handler : m_user_handlers) {
		handler->OnQuerySubscriberSuperiorUserSuccessed(users);
	}
}

void CoreService::OnQuerySubscriberSuperiorUserFailed(int error_code, const std::string& message) {
	std::lock_guard<std::mutex> guard(m_user_handlers_mutex);
	for (UserEventHandler* handler : m_user_handlers) {
		handler->OnQuerySubscriberSuperiorUserFailed(error_code, message);
	}
}

void CoreService::OnQuerySuperiorUserSuccessed(std::list<SuperiorUser> users) {
	std::lock_guard<std::mutex> guard(m_user_handlers_mutex);
	for (UserEventHandler* handler : m_user_handlers) {
		handler->OnQuerySuperiorUserSuccessed(users);
	}
}

void CoreService::OnQuerySuperiorUserFailed(int error_code, const std::string& message) {
	std::lock_guard<std::mutex> guard(m_user_handlers_mutex);
	for (UserEventHandler* handler : m_user_handlers) {
		handler->OnQuerySuperiorUserFailed(error_code, message);
	}
}

void CoreService::OnUpdatePrimaryUserInfoSuccessed() {
    std::lock_guard<std::mutex> guard(m_user_handlers_mutex);
    for (UserEventHandler* handler : m_user_handlers) {
        handler->OnUpdatePrimaryUserInfoSuccessed();
    }
}

void CoreService::OnUpdatePrimaryUserInfoFailed(int error_code, const std::string& message) {
    std::lock_guard<std::mutex> guard(m_user_handlers_mutex);
    for (UserEventHandler* handler : m_user_handlers) {
        handler->OnUpdatePrimaryUserInfoFailed(error_code, message);
    }
}

void CoreService::OnUpdateSuperiorUserInfoSuccessed() {
    std::lock_guard<std::mutex> guard(m_user_handlers_mutex);
    for (UserEventHandler* handler : m_user_handlers) {
        handler->OnUpdateSuperiorUserInfoSuccessed();
    }
}

void CoreService::OnUpdateSuperiorUserInfoFailed(int error_code, const std::string& message) {
    std::lock_guard<std::mutex> guard(m_user_handlers_mutex);
    for (UserEventHandler* handler : m_user_handlers) {
        handler->OnUpdateSuperiorUserInfoFailed(error_code, message);
    }
}

void CoreService::OnQuerySubscriberSuperiorUserOnlineStatusSuccessed(std::list<SuperiorUserStatus> users){
    std::lock_guard<std::mutex> guard(m_user_handlers_mutex);
    for (UserEventHandler* handler : m_user_handlers) {
        handler->OnQuerySubscriberSuperiorUserOnlineStatusSuccessed(users);
    }
}
void CoreService::OnQuerySubscriberSuperiorUserOnlineStatusFailed(int error_code, const std::string& message){
std::lock_guard<std::mutex> guard(m_user_handlers_mutex);
    for (UserEventHandler* handler : m_user_handlers) {
        handler->OnQuerySubscriberSuperiorUserOnlineStatusFailed(error_code, message);
    }
}

void CoreService::OnQueryControlPrimaryUserSuccessed(std::list<PrimaryUser> users) {
    std::lock_guard<std::mutex> guard(m_user_handlers_mutex);
    for (UserEventHandler* handler : m_user_handlers) {
        handler->OnQueryControlPrimaryUserSuccessed(users);
    }
}

void CoreService::OnQueryControlPrimaryUserFailed(int error_code, const std::string& message) {
    std::lock_guard<std::mutex> guard(m_user_handlers_mutex);
    for (UserEventHandler* handler : m_user_handlers) {
        handler->OnQueryControlPrimaryUserFailed(error_code, message);
    }
}

void CoreService::OnCreateReportSuccessed() {
	std::lock_guard<std::mutex> guard(m_report_handlers_mutex);
	for (ReportEventHandler* handler : m_report_handlers) {
		handler->OnCreateReportSuccessed();
	}
}

void CoreService::OnCreateReportFailed(int error_code, const std::string& message) {
	std::lock_guard<std::mutex> guard(m_report_handlers_mutex);
	for (ReportEventHandler* handler : m_report_handlers) {
		handler->OnCreateReportFailed(error_code, message);
	}
}

void CoreService::OnUpdateReportSuccessed() {
	std::lock_guard<std::mutex> guard(m_report_handlers_mutex);
	for (ReportEventHandler* handler : m_report_handlers) {
		handler->OnUpdateReportSuccessed();
	}
}

void CoreService::OnUpdateReportFailed(int error_code, const std::string& message) {
	std::lock_guard<std::mutex> guard(m_report_handlers_mutex);
	for (ReportEventHandler* handler : m_report_handlers) {
		handler->OnUpdateReportFailed(error_code, message);
	}
}

void CoreService::OnConfirmReportSuccessed(const core::ConfirmReportData& data) {
	std::lock_guard<std::mutex> guard(m_report_handlers_mutex);
	for (ReportEventHandler* handler : m_report_handlers) {
		handler->OnConfirmReportSuccessed(data);
	}
}

void CoreService::OnConfirmReportFailed(int error_code, const std::string& message) {
	std::lock_guard<std::mutex> guard(m_report_handlers_mutex);
	for (ReportEventHandler* handler : m_report_handlers) {
		handler->OnConfirmReportFailed(error_code, message);
	}
}

void CoreService::OnQueryReportSuccessed(std::list<Report> reports) {
	std::lock_guard<std::mutex> guard(m_report_handlers_mutex);
	for (ReportEventHandler* handler : m_report_handlers) {
		handler->OnQueryReportSuccessed(reports);
	}
}

void CoreService::OnQueryReportFailed(int error_code, const std::string& message) {
	std::lock_guard<std::mutex> guard(m_report_handlers_mutex);
	for (ReportEventHandler* handler : m_report_handlers) {
		handler->OnQueryReportFailed(error_code, message);
	}
}

void CoreService::OnCheckUpgradeSuccessed(AppUpgradeInfo upgradeInfo)
{
	std::lock_guard<std::mutex> guard(m_upgrade_handlers_mutex);
	for (UpgradeEventHandler* handler : m_upgrade_handlers) {
		handler->OnCheckUpgradeSuccessed(upgradeInfo);
	}
}

void CoreService::OnCheckUpgradeFailed(int error_code, const std::string& message)
{
	std::lock_guard<std::mutex> guard(m_upgrade_handlers_mutex);
	for (UpgradeEventHandler* handler : m_upgrade_handlers) {
		handler->OnCheckUpgradeFailed(error_code, message);
	}
}

void CoreService::OnGetTemplateSuccessed(const std::list<ReportTemplate>& list)
{
    std::lock_guard<std::mutex> guard(m_upgrade_handlers_mutex);
    for (TemplateEventHandler* handler : m_template_handlers) {
        handler->OnGetTemplateSuccessed(list);
    }
}

void CoreService::OnGetTemplateFailed(int error_code, const std::string& message)
{
    std::lock_guard<std::mutex> guard(m_upgrade_handlers_mutex);
    for (TemplateEventHandler* handler : m_template_handlers) {
        handler->OnGetTemplateFailed(error_code, message);
    }
}

void CoreService::OnQueryTemplateSuccessed(const std::list<ReportTemplate>& list)
{
    std::lock_guard<std::mutex> guard(m_upgrade_handlers_mutex);
    for (TemplateEventHandler* handler : m_template_handlers) {
        handler->OnQueryTemplateSuccessed(list);
    }
}

void CoreService::OnQueryTemplateFailed(int error_code, const std::string & message)
{
    std::lock_guard<std::mutex> guard(m_upgrade_handlers_mutex);
    for (TemplateEventHandler* handler : m_template_handlers) {
        handler->OnQueryTemplateFailed(error_code, message);
    }
}
void CoreService::OnCreateTemplateSuccessed()
{
    std::lock_guard<std::mutex> guard(m_upgrade_handlers_mutex);
    for (TemplateEventHandler* handler : m_template_handlers) {
        handler->OnCreateTemplateSuccessed();
    }
}

void CoreService::OnCreateTemplateFailed(int error_code, const std::string& message)
{
    std::lock_guard<std::mutex> guard(m_upgrade_handlers_mutex);
    for (TemplateEventHandler* handler : m_template_handlers) {
        handler->OnCreateTemplateFailed(error_code, message);
    }
}

void CoreService::OnUpdateTemplateSuccessed()
{
    std::lock_guard<std::mutex> guard(m_upgrade_handlers_mutex);
    for (TemplateEventHandler* handler : m_template_handlers) {
        handler->OnUpdateTemplateSuccessed();
    }
}

void CoreService::OnUpdateTemplateFailed(int error_code, const std::string& message)
{
    std::lock_guard<std::mutex> guard(m_upgrade_handlers_mutex);
    for (TemplateEventHandler* handler : m_template_handlers) {
        handler->OnUpdateTemplateFailed(error_code, message);
    }
}

void CoreService::OnDeleteTemplateSuccessed()
{
    std::lock_guard<std::mutex> guard(m_upgrade_handlers_mutex);
    for (TemplateEventHandler* handler : m_template_handlers) {
        handler->OnDeleteTemplateSuccessed();
    }
}

void CoreService::OnDeleteTemplateFailed(int error_code, const std::string& message)
{
    std::lock_guard<std::mutex> guard(m_upgrade_handlers_mutex);
    for (TemplateEventHandler* handler : m_template_handlers) {
        handler->OnDeleteTemplateFailed(error_code, message);
    }
}


void CoreService::Login(const Role role, const std::string& uid, const std::string& password) {
    m_current_login_info.role = role;
    m_current_login_info.uid = uid;
    m_current_login_info.password = password;

    if (m_delegate) {
        m_delegate->Login(uid, password);
    }
}

void CoreService::GetUserInfo(const std::string& uid, bool primary) {
	if (m_delegate) {
		if (primary)
			m_delegate->QueryPrimaryUserInfo(uid);
	}
}

void CoreService::GetCurrentUserInfo() {
    if (m_delegate) {
        if (m_current_login_info.role == Primary)
            m_delegate->GetPrimaryUserInfo(m_current_login_info.uid);
        else if (m_current_login_info.role == Superior)
            m_delegate->GetSuperiorUserInfo(m_current_login_info.uid);
    }
}

void CoreService::UpdateCurrentUserInfo(const Json::Value& data) {
    if (m_delegate) {
        if (m_current_login_info.role == Primary)
            m_delegate->UpdatePrimaryUserInfo(data);
        else if (m_current_login_info.role == Superior)
            m_delegate->UpdateSuperiorUserInfo(data);
    }
}

void CoreService::UpdatePassword(const std::string& old_password, 
  const std::string& new_password) {
	m_current_login_info.new_password = new_password;
	if (m_delegate) {
		m_delegate->UpdatePassword(old_password, new_password);
	}
}

void CoreService::QuerySubscriberSuperiorUser() {
	if (m_delegate) {
		m_delegate->QuerySubscriberSuperiorUser();
	}
}

void CoreService::QuerySuperiorUser(std::map<std::string, std::string> conditions) {
	if (m_delegate) {
		m_delegate->QuerySuperiorUser(conditions);
	}
}

void CoreService::QuerySubscriberSuperiorUserOnlineStatus(std::list<core::SuperiorUser> superior_users){
    if (m_delegate) {
		m_delegate->QuerySubscriberSuperiorUserOnlineStatus(superior_users);
	}
}

void CoreService::QueryControlPrimaryUser() {
    if (m_delegate) {
        m_delegate->QueryControlPrimaryUser();
    }
}

void CoreService::CreateReport(const std::string& report_id) {
    m_current_report.New(report_id);
    m_is_report_saved = false;
}

void CoreService::SaveReport() {
	if (m_delegate) {
		Json::Value data;
        m_current_report.SaveToJson(data, true);

		if (m_is_report_saved)
			m_delegate->UpdateReport(data);
		else
			m_delegate->CreateReport(data);

        m_is_report_saved = true;
	}
}

void CoreService::LoadReport(const Report& report) {
    m_current_report.Clear();
    m_current_report = report;
    m_is_report_saved = true;
}

void CoreService::QueryReport(std::map<std::string, std::string> conditions) {
	if (m_delegate) {
		m_delegate->QueryReport(conditions);
	}
}

void CoreService::ConfirmReport(const Report& report) {
	if (m_delegate) {
		Json::Value data;
		data["uid"] = report.GetUid();
		data["content"] = report.GetContent();
		data["conclusion"] = report.GetConclusion();
		data["confirm_status"] = report.GetConfirmStatus();
		data["confirm_msg"] = report.GetConfirmMsg();

		m_delegate->ConfirmReport(data, report.GetPrimaryId());
	}
}

void CoreService::UpdateReport(Report& report) {
	if (m_delegate) {
		Json::Value data;
		report.SaveToJson(data, true);

		m_delegate->UpdateReport(data);
	}
}

void CoreService::CheckUpgrade(const std::string& app)
{
	if (m_delegate) {
		m_delegate->CheckUpgrade(app);
	}
}

void CoreService::GetTemplates(std::map<std::string, std::string> conditions)
{
    if (m_delegate) {
        m_delegate->GetTemplates(conditions);
    }
}

void CoreService::CreateTemplate(const Json::Value & data)
{
    if (m_delegate) {
        m_delegate->CreateTemplate(data);
    }
}

void CoreService::UpdateTemplate(const Json::Value & data)
{
    if (m_delegate) {
        m_delegate->UpdateTemplate(data);
    }
}

void CoreService::QueryTemplate(std::map<std::string, std::string> conditions)
{
    if (m_delegate) {
        m_delegate->QueryTemplate(conditions);
    }
}

void CoreService::DeleteTemplate(std::map<std::string, std::string> conditions)
{
    if (m_delegate) {
        m_delegate->DeleteTemplate(conditions);
    }
}

} // namespace core