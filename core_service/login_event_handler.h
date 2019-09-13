#ifndef CORE_SERVICE_LOGIN_EVENT_HANDLER_H_
#define CORE_SERVICE_LOGIN_EVENT_HANDLER_H_

namespace core {

class LoginEventHandler {
public:
    virtual ~LoginEventHandler() {}

    enum ErrorCode {
        ErrorUnknown = -1,
        ErrorRole = -2
    };

    virtual void OnCoreLoginSuccessed() = 0;
    virtual void OnCoreLoginFailed(int error_code, const std::string& message) = 0;
    virtual void OnFetchTokenSuccessed() = 0;
    virtual void OnFetchTokenFailed(int error_code, const std::string& message) = 0;
};

} // namespace core 

#endif // CORE_SERVICE_LOGIN_EVENT_HANDLER_H_