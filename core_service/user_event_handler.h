#ifndef CORE_SERVICE_USER_EVENT_HANDLER_H_
#define CORE_SERVICE_USER_EVENT_HANDLER_H_

#include <list>
#include <string>

#include "core_service/primary_user.h"
#include "core_service/superior_user.h"
#include "core_service/superior_user_status.h"

namespace core {

class UserEventHandler {
public:
    virtual ~UserEventHandler() {}

	virtual void OnGetPrimaryUserInfoSuccessed(const PrimaryUser& user) = 0;
	virtual void OnGetPrimaryUserInfoFailed(int error_code, const std::string& message) = 0;

	virtual void OnGetSuperiorUserInfoSuccessed(const SuperiorUser& user) = 0;
	virtual void OnGetSuperiorUserInfoFailed(int error_code, const std::string& message) = 0;

	virtual void OnUpdatePasswordSuccessed(const std::string& uid, const std::string& new_password) = 0;
	virtual void OnUpdatePasswordFailed(int error_code, const std::string& message) = 0;

	virtual void OnQuerySubscriberSuperiorUserSuccessed(std::list<SuperiorUser> users) = 0;
	virtual void OnQuerySubscriberSuperiorUserFailed(int error_code, const std::string& message) = 0;

	virtual void OnQuerySuperiorUserSuccessed(std::list<SuperiorUser> users) = 0;
	virtual void OnQuerySuperiorUserFailed(int error_code, const std::string& message) = 0;

    virtual void OnUpdatePrimaryUserInfoSuccessed() = 0;
    virtual void OnUpdatePrimaryUserInfoFailed(int error_code, const std::string& message) = 0;

    virtual void OnUpdateSuperiorUserInfoSuccessed() = 0;
    virtual void OnUpdateSuperiorUserInfoFailed(int error_code, const std::string& message) = 0;

	virtual void OnQuerySubscriberSuperiorUserOnlineStatusSuccessed(std::list<SuperiorUserStatus> users) = 0;
	virtual void OnQuerySubscriberSuperiorUserOnlineStatusFailed(int error_code, const std::string& message) = 0;

    virtual void OnQueryControlPrimaryUserSuccessed(std::list<PrimaryUser> users) = 0;
    virtual void OnQueryControlPrimaryUserFailed(int error_code, const std::string& message) = 0;
};

} // namespace core 

#endif // CORE_SERVICE_USER_EVENT_HANDLER_H_