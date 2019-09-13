#ifndef CORE_SERVICE_SUPERIOR_USER_STATUS_H_
#define CORE_SERVICE_SUPERIOR_USER_STATUS_H_

#include <list>
#include <string>

#include "json.h"

namespace core {

    class SuperiorUserStatus {
    public:
        bool LoadFromJson(const Json::Value& value);

        const std::string& GetUid() const { return m_uid; }
        const std::string& GetOnlineAOS() const { return m_online_aos; }
        const std::string& GetOnlinePC() const { return m_online_pc; }
    private:
        std::string m_uid;
        std::string m_online_aos;
        std::string m_online_pc;
    };
}

#endif//CORE_SERVICE_SUPERIOR_USER_STATUS_H_