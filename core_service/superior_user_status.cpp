#include "superior_user_status.h"

bool core::SuperiorUserStatus::LoadFromJson(const Json::Value& value)
{
    m_uid = value["uid"].asString();
    m_online_aos = value["online_aos"].asString();
    m_online_pc = value["online_pc"].asString();

    return true;
}

