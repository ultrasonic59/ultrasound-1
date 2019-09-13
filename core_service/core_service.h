#ifndef CORE_SERVICE_CORE_SERVICE_H_
#define CORE_SERVICE_CORE_SERVICE_H_

#include <list>
#include <map>
#include <mutex>
#include <string>

#include "base/macros.h"
#include "core_service/login_event_handler.h"
#include "core_service/primary_user.h"
#include "core_service/report.h"
#include "core_service/report_event_handler.h"
#include "core_service/superior_user.h"
#include "core_service/user_event_handler.h"
#include "core_service/superior_user_status.h"
#include "core_service/app_upgrade_info.h"
#include "core_service/upgrade_event_handler.h"
#include "core_service/report_template.h"
#include "core_service/template_event_handler.h"

namespace core {

class CoreService : public LoginEventHandler,
    public UserEventHandler,
    public ReportEventHandler,
    public UpgradeEventHandler,
    public TemplateEventHandler
{
public:
    SINGLETON_DEFINE(CoreService);

    enum Role
    {
        Primary = 0,
        Superior
    };

    bool Initialize();
    void Shutdown();
    void SetProxyInfo(const std::string& host, const int port, const std::string& username, const std::string& password, bool enabled);

    void AddLoginEventHandler(LoginEventHandler* handler);
    void RemoveLoginEventHandler(LoginEventHandler* handler);
    void AddUserEventHandler(UserEventHandler* handler);
    void RemoveUserEventHandler(UserEventHandler* handler);
    void AddReportEventHandler(ReportEventHandler* handler);
    void RemoveReportEventHandler(ReportEventHandler* handler);
    void AddUpgradeEventHandler(UpgradeEventHandler* handler);
    void RemoveUpgradeEventHandler(UpgradeEventHandler* handler);
    void AddTemplateEventHandler(TemplateEventHandler* handler);
    void RemoveTemplateEventHandler(TemplateEventHandler* handler);

    // LoginEventHandler
    void OnCoreLoginSuccessed() override;
    void OnCoreLoginFailed(int error_code, const std::string& message) override;
    void OnFetchTokenSuccessed() override;
    void OnFetchTokenFailed(int error_code, const std::string& message) override;

    // UserEventHandler
    void OnGetPrimaryUserInfoSuccessed(const PrimaryUser& user) override;
    void OnGetPrimaryUserInfoFailed(int error_code, const std::string& message) override;
    void OnGetSuperiorUserInfoSuccessed(const SuperiorUser& user) override;
    void OnGetSuperiorUserInfoFailed(int error_code, const std::string& message) override;
    void OnUpdatePasswordSuccessed(const std::string& uid, const std::string& new_password) override;
    void OnUpdatePasswordFailed(int error_code, const std::string& message) override;
    void OnQuerySubscriberSuperiorUserSuccessed(std::list<SuperiorUser> users) override;
    void OnQuerySubscriberSuperiorUserFailed(int error_code, const std::string& message) override;
    void OnQuerySuperiorUserSuccessed(std::list<SuperiorUser> users) override;
    void OnQuerySuperiorUserFailed(int error_code, const std::string& message) override;
    void OnUpdatePrimaryUserInfoSuccessed() override;
    void OnUpdatePrimaryUserInfoFailed(int error_code, const std::string& message) override;
    void OnUpdateSuperiorUserInfoSuccessed() override;
    void OnUpdateSuperiorUserInfoFailed(int error_code, const std::string& message) override;
    void OnQuerySubscriberSuperiorUserOnlineStatusSuccessed(std::list<SuperiorUserStatus> users) override;
    void OnQuerySubscriberSuperiorUserOnlineStatusFailed(int error_code, const std::string& message) override;
    void OnQueryControlPrimaryUserSuccessed(std::list<core::PrimaryUser> users) override;
    void OnQueryControlPrimaryUserFailed(int error_code, const std::string& message) override;

    // ReportEventHandler
    void OnCreateReportSuccessed() override;
    void OnCreateReportFailed(int error_code, const std::string& message) override;
    void OnUpdateReportSuccessed() override;
    void OnUpdateReportFailed(int error_code, const std::string& message) override;
    	void OnConfirmReportSuccessed(const core::ConfirmReportData& data) override;
	void OnConfirmReportFailed(int error_code, const std::string& message) override;
    void OnQueryReportSuccessed(std::list<Report> reports) override;
    void OnQueryReportFailed(int error_code, const std::string& message) override;

    void OnCheckUpgradeSuccessed(AppUpgradeInfo upgradeInfo) override;
    void OnCheckUpgradeFailed(int error_code, const std::string& message) override;

    //TemplateEventHandler
    void OnGetTemplateSuccessed(const std::list<ReportTemplate>& list) override;
    void OnGetTemplateFailed(int error_code, const std::string& cs) override;
    void OnQueryTemplateSuccessed(const std::list<ReportTemplate>& list);
    void OnQueryTemplateFailed(int error_code, const std::string& cs);
    void OnCreateTemplateSuccessed() override;
    void OnCreateTemplateFailed(int error_code, const std::string& message) override;
    void OnUpdateTemplateSuccessed() override;
    void OnUpdateTemplateFailed(int error_code, const std::string& message) override;
    void OnDeleteTemplateSuccessed() override;
    void OnDeleteTemplateFailed(int error_code, const std::string& message) override;

    const std::string GetCurrentUid() { return m_current_login_info.uid; }
    const Role GetCurrentRole() { return m_current_login_info.role; }
    const std::string GetCurrentPassword() { return m_current_login_info.password; }
    const std::string GetCurrentAppToken() { return m_current_login_info.app_token; }
    const std::string GetCurrentNimToken() { return m_current_login_info.nim_token; }
    const std::string GetCurrentOssKey() { return m_current_login_info.oss_key; }
    const std::string GetCurrentOssSecret() { return m_current_login_info.oss_secret; }

    PrimaryUser& GetCurrentPrimaryUser() { return m_current_primary_user; };
    SuperiorUser& GetCurrentSuperiorUser() { return m_current_superior_user; }
    Report& GetCurrentReport() { return m_current_report; }

    // Login
    void Login(const Role role, const std::string& uid, const std::string& password);

    // User
    void GetUserInfo(const std::string& uid, bool primary = false);
    void GetCurrentUserInfo();
    void UpdateCurrentUserInfo(const Json::Value& data);
    void UpdatePassword(const std::string& old_password, const std::string& new_password);
    void QuerySubscriberSuperiorUser();
    void QuerySuperiorUser(std::map<std::string, std::string> conditions);
    void QuerySubscriberSuperiorUserOnlineStatus(std::list<core::SuperiorUser> superior_users);
    void QueryControlPrimaryUser();

    // Report
    void CreateReport(const std::string& report_id);
    void SaveReport();
    void LoadReport(const Report& report);
    void QueryReport(std::map<std::string, std::string> conditions);
    void ConfirmReport(const Report& report);
	void UpdateReport(Report& report);

   //upgrade
    void CheckUpgrade(const std::string& app);

    void GetTemplates(std::map<std::string, std::string> conditions);
    void CreateTemplate(const Json::Value& data);
    void UpdateTemplate(const Json::Value& data);
    void QueryTemplate(std::map<std::string, std::string> conditions);
    void DeleteTemplate(std::map<std::string, std::string> conditions);
private:
    CoreService() = default;
    ~CoreService() = default;

    struct LoginInfo
    {
        bool is_valid = false;
        Role role = Primary;
        std::string uid;
        std::string password;
        std::string new_password;
        std::string app_token;
        std::string nim_token;
        std::string oss_key;
        std::string oss_secret;

        void Clear() {
            is_valid = false;
            role = Primary;
            uid.clear();
            password.clear();
            new_password.clear();
            app_token.clear();
            nim_token.clear();
            oss_key.clear();
            oss_secret.clear();
        }
    };

    void RemoveAllEventHandlers();

    std::mutex m_login_handlers_mutex;
    std::mutex m_user_handlers_mutex;
    std::mutex m_report_handlers_mutex;
    std::mutex m_upgrade_handlers_mutex;
    std::mutex m_template_handlers_mutex;
    std::list<LoginEventHandler*> m_login_handlers;
    std::list<UserEventHandler*> m_user_handlers;
    std::list<ReportEventHandler*> m_report_handlers;
    std::list<UpgradeEventHandler*> m_upgrade_handlers;
    std::list<TemplateEventHandler*> m_template_handlers;

    LoginInfo m_current_login_info;
    PrimaryUser m_current_primary_user;
    SuperiorUser m_current_superior_user;
    Report m_current_report;

    bool m_is_report_saved = false;

    friend class CoreServiceDelegate;
    CoreServiceDelegate* m_delegate = nullptr;
};

} // namespace core

#endif // CORE_SERVICE_CORE_SERVICE_H_
