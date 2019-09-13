﻿/** @file nim_cpp_team.cpp
  * @brief 群组功能；主要包括查询群信息、查询群成员信息、加人、踢人等功能
  * @copyright (c) 2015-2017, NetEase Inc. All rights reserved
  * @author towik, Oleg, Harrison
  * @date 2015/2/1
  */

#include "nim_cpp_team.h"
#include "nim_sdk_util.h"
#include "nim_json_util.h"
#include "nim_string_util.h"
#include "nim_cpp_global.h"

namespace nim
{
#ifdef NIM_SDK_DLL_IMPORT
typedef void(*nim_team_reg_team_event_cb)(const char *json_extension, nim_team_event_cb_func cb, const void *user_data);
typedef void(*nim_team_create_team_async)(const char *team_info, const char *jsonlist_uids, const char *invitation_postscript, const char *json_extension, nim_team_event_cb_func cb, const void* user_data);
typedef void(*nim_team_query_team_info_online_async)(const char *tid, const char *json_extension, nim_team_event_cb_func cb, const void* user_data);
typedef void(*nim_team_invite_async)(const char *tid, const char *jsonlist_uids, const char *invitation_postscript, const char *json_extension, nim_team_event_cb_func cb, const void* user_data);
typedef void(*nim_team_kick_async)(const char *tid, const char *jsonlist_uids, const char *json_extension, nim_team_event_cb_func cb, const void* user_data);
typedef void(*nim_team_leave_async)(const char *tid, const char *json_extension, nim_team_event_cb_func cb, const void* user_data);
typedef void(*nim_team_dismiss_async)(const char *tid, const char *json_extension, nim_team_event_cb_func cb, const void* user_data);
typedef void(*nim_team_update_team_info_async)(const char *tid, const char *json_info, const char *json_extension, nim_team_event_cb_func cb_func, const void* user_data);
typedef void(*nim_team_apply_join_async)(const char *tid, const char *reason, const char *json_extension, nim_team_event_cb_func cb, const void* user_data);
typedef void(*nim_team_pass_join_apply_async)(const char *tid, const char *applicant_id, const char *json_extension, nim_team_event_cb_func cb, const void* user_data);
typedef void(*nim_team_reject_join_apply_async)(const char *tid, const char *applicant_id, const char *reason, const char *json_extension, nim_team_event_cb_func cb, const void* user_data);
typedef void(*nim_team_add_managers_async)(const char *tid, const char *jsonlist_admin_ids, const char *json_extension, nim_team_event_cb_func cb, const void* user_data);
typedef void(*nim_team_remove_managers_async)(const char *tid, const char *jsonlist_admin_ids, const char *json_extension, nim_team_event_cb_func cb, const void* user_data);
typedef void(*nim_team_transfer_team_async)(const char *tid, const char *new_owner, bool is_leave, const char *json_extension, nim_team_event_cb_func cb, const void* user_data);
typedef void(*nim_team_update_my_property_async)(const char *info, const char *json_extension, nim_team_event_cb_func cb, const void* user_data);
typedef void(*nim_team_update_other_nick_async)(const char *info, const char *json_extension, nim_team_event_cb_func cb, const void* user_data);
typedef void(*nim_team_accept_invitation_async)(const char *tid, const char *invitor, const char *json_extension, nim_team_event_cb_func cb, const void* user_data);
typedef void(*nim_team_reject_invitation_async)(const char *tid, const char *invitor, const char *reason, const char *json_extension, nim_team_event_cb_func cb, const void* user_data);

typedef void(*nim_team_query_all_my_teams_async)(const char *json_extension, nim_team_query_all_my_teams_cb_func cb, const void* user_data);
typedef void(*nim_team_query_all_my_teams_info_async)(const char *json_extension, nim_team_query_all_my_teams_info_cb_func cb, const void* user_data);
#ifdef NIMAPI_UNDER_WIN_DESKTOP_ONLY
typedef void(*nim_team_query_my_all_member_infos_async)(const char *json_extension,	nim_team_query_my_all_member_infos_cb_func cb, const void *user_data);
#endif
typedef void(*nim_team_query_team_members_async)(const char *tid, bool include_user_info, const char *json_extension, nim_team_query_team_members_cb_func cb, const void* user_data);
typedef void(*nim_team_query_team_member_async)(const char *tid, const char *user_id, const char *json_extension, nim_team_query_team_member_cb_func cb, const void *user_data);
typedef void(*nim_team_query_team_info_async)(const char *tid, const char *json_extension, nim_team_query_team_info_cb_func cb, const void* user_data);
typedef char*(*nim_team_query_team_member_block)(const char *tid, const char *user_id);
typedef char*(*nim_team_query_team_info_block)(const char *tid);

typedef void(*nim_team_mute_member_async)(const char *tid, const char *member_id, bool set_mute, const char *json_extension, nim_team_opt_cb_func cb, const void *user_data);
typedef void(*nim_team_query_mute_list_online_async)(const char *tid, 	const char *json_extension, nim_team_query_mute_list_cb_func cb, const void *user_data);
typedef void(*nim_team_mute_async)(const char *tid, bool set_mute, const char *json_extension, nim_team_opt_cb_func cb, const void *user_data);
typedef void(*nim_team_msg_ack_read)(const char *tid, const char *json_msgs, const char *json_extension, nim_team_opt_cb_func cb, const void *user_data);
typedef void(*nim_team_msg_query_unread_list)(const char *tid, const char *json_msg, const char *json_extension, nim_team_opt_cb_func cb, const void *user_data);
#else
#include "nim_team.h"
#endif

static void CallbackTeamEvent(int res_code, int notification_id, const char *tid, const char *result, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		Team::TeamEventCallback* cb_pointer = (Team::TeamEventCallback*)user_data;
		if (*cb_pointer)
		{
			TeamEvent team_event;
			ParseTeamEvent(res_code, PCharToString(tid), (nim::NIMNotificationId)notification_id, PCharToString(result), team_event);
			(*cb_pointer)(team_event);
		}
	}
}

static void CallbackTeamChange(int res_code, int notification_id, const char *tid, const char *result, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		Team::TeamEventCallback* cb_pointer = (Team::TeamEventCallback*)user_data;
		if (*cb_pointer)
		{
			TeamEvent team_event;
			ParseTeamEvent(res_code, PCharToString(tid), (nim::NIMNotificationId)notification_id, PCharToString(result), team_event);
			(*cb_pointer)(team_event);
		}
		delete cb_pointer;
	}
}

static void CallbackQueryMyTeams(int team_count, const char *result, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		Team::QueryAllMyTeamsCallback* cb_pointer = (Team::QueryAllMyTeamsCallback*)user_data;
		if (*cb_pointer)
		{
			std::list<std::string> team_id_list;
			JsonStrArrayToList(GetJsonValueFromJsonString(PCharToString(result)), team_id_list);
			(*cb_pointer)(team_count, team_id_list);
		}
		delete cb_pointer;
	}
}

static void CallbackQueryTeamMembers(const char *tid, int member_count, bool include_user_info, const char *result, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		Team::QueryTeamMembersCallback* cb_pointer = (Team::QueryTeamMembersCallback*)user_data;
		if (*cb_pointer)
		{
			std::list<nim::TeamMemberProperty> team_member_info_list;
			ParseTeamMemberPropertysJson(PCharToString(result), team_member_info_list);
			(*cb_pointer)(PCharToString(tid), member_count, team_member_info_list);
		}
		delete cb_pointer;
	}
}

static void CallbackQueryTeamMember(const char *tid, const char *id, const char *result, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		Team::QueryTeamMemberCallback* cb_pointer = (Team::QueryTeamMemberCallback*)user_data;
		if (*cb_pointer)
		{
			nim::TeamMemberProperty prop;
			ParseTeamMemberPropertyJson(PCharToString(result), prop);
			(*cb_pointer)(prop);
		}
		delete cb_pointer;
	}
}

static void CallbackQueryAllMyTeamsInfo(int team_count, const char *result, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		Team::QueryAllMyTeamsInfoCallback* cb_pointer = (Team::QueryAllMyTeamsInfoCallback*)user_data;
		if (*cb_pointer)
		{
			std::list<nim::TeamInfo> team_info_list;
			ParseTeamInfosJson(PCharToString(result), team_info_list);
			(*cb_pointer)(team_count, team_info_list);
		}
		delete cb_pointer;
	}
}

#ifdef NIMAPI_UNDER_WIN_DESKTOP_ONLY
static void CallbackQueryMyAllMemberInfos(int count, const char *result, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		Team::QueryMyAllMemberInfosCallback* cb_pointer = (Team::QueryMyAllMemberInfosCallback*)user_data;
		if (*cb_pointer)
		{
			std::list<nim::TeamMemberProperty> my_infos_list;
			ParseTeamMemberPropertysJson(PCharToString(result), my_infos_list);
			(*cb_pointer)(count, my_infos_list);
		}
		delete cb_pointer;
	}
}
#endif

static void CallbackQueryTeamInfo(const char *tid, const char *result, const char *json_extension, const void *callback)
{
	if (callback)
	{
		Team::QueryTeamInfoCallback* cb_pointer = (Team::QueryTeamInfoCallback*)callback;
		if (*cb_pointer)
		{
			nim::TeamInfo team_info;
			ParseTeamInfoJson(PCharToString(result), team_info);
			(*cb_pointer)(PCharToString(tid), team_info);
		}
		delete cb_pointer;
	}
}

static Team::TeamEventCallback g_cb_team_event_ = nullptr;
void Team::RegTeamEventCb(const TeamEventCallback& cb, const std::string& json_extension)
{
	g_cb_team_event_ = cb;
	return NIM_SDK_GET_FUNC(nim_team_reg_team_event_cb)(json_extension.c_str(), &CallbackTeamEvent, &g_cb_team_event_);
}

bool Team::CreateTeamAsync(const TeamInfo& team_info
	, const std::list<std::string>& ids
	, const std::string& invitation_postscript
	, const TeamEventCallback& cb
	, const std::string& json_extension/* = ""*/)
{
	if (team_info.GetType() == kNIMTeamTypeNormal && ids.empty())
		return false;

	TeamEventCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new TeamEventCallback(cb);
	}
	std::string uids;
	StrListToJsonString(ids, uids);
	NIM_SDK_GET_FUNC(nim_team_create_team_async)(team_info.ToJsonString().c_str()
		, uids.c_str()
		, invitation_postscript.c_str()
		, json_extension.c_str()
		, &CallbackTeamChange
		, cb_pointer);

	return true;
}

bool Team::InviteAsync(const std::string& tid
	, const std::list<std::string>& ids
	, const std::string& invitation_postscript
	, const TeamEventCallback& cb
	, const std::string& json_extension/* = ""*/)
{
	if (tid.empty() || ids.empty())
		return false;

	TeamEventCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new TeamEventCallback(cb);
	}
	std::string uids;
	StrListToJsonString(ids, uids);
	NIM_SDK_GET_FUNC(nim_team_invite_async)(tid.c_str()
		, uids.c_str()
		, invitation_postscript.c_str()
		, json_extension.c_str()
		, &CallbackTeamChange
		, cb_pointer);

	return true;
}

bool Team::KickAsync(const std::string& tid
	, const std::list<std::string>& ids
	, const TeamEventCallback& cb
	, const std::string& json_extension/* = ""*/)
{
	if (tid.empty() || ids.empty())
		return false;

	TeamEventCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new TeamEventCallback(cb);
	}
	std::string uids;
	StrListToJsonString(ids, uids);
	NIM_SDK_GET_FUNC(nim_team_kick_async)(tid.c_str()
		, uids.c_str()
		, json_extension.c_str()
		, &CallbackTeamChange
		, cb_pointer);

	return true;
}

bool Team::LeaveAsync(const std::string& tid, const TeamEventCallback& cb, const std::string& json_extension/* = ""*/)
{
	if (tid.empty())
		return false;

	TeamEventCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new TeamEventCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_team_leave_async)(tid.c_str()
		, json_extension.c_str()
		, &CallbackTeamChange
		, cb_pointer);

	return true;
}

bool Team::DismissAsync(const std::string& tid, const TeamEventCallback& cb, const std::string& json_extension/* = ""*/)
{
	if (tid.empty())
		return false;

	TeamEventCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new TeamEventCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_team_dismiss_async)(tid.c_str()
		, json_extension.c_str()
		, &CallbackTeamChange
		, cb_pointer);

	return true;
}

bool Team::UpdateTeamInfoAsync(const std::string& tid
	, const TeamInfo& team_info
	, const TeamEventCallback& cb
	, const std::string& json_extension/* = ""*/)
{
	if (tid.empty() || team_info.GetTeamID().empty())
		return false;

	TeamEventCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new TeamEventCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_team_update_team_info_async)(tid.c_str()
		, team_info.ToJsonString().c_str()
		, json_extension.c_str()
		, &CallbackTeamChange
		, cb_pointer);

	return true;
}

bool Team::ApplyJoinAsync(const std::string& tid
	, const std::string& reason
	, const TeamEventCallback& cb
	, const std::string& json_extension/* = ""*/)
{
	if (tid.empty())
		return false;

	TeamEventCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new TeamEventCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_team_apply_join_async)(tid.c_str()
		, reason.c_str()
		, json_extension.c_str()
		, &CallbackTeamChange
		, cb_pointer);

	return true;
}

bool Team::PassJoinApplyAsync(const std::string& tid
	, const std::string& applicant_id
	, const TeamEventCallback& cb
	, const std::string& json_extension/* = ""*/)
{
	if (tid.empty() || applicant_id.empty())
		return false;

	TeamEventCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new TeamEventCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_team_pass_join_apply_async)(tid.c_str()
		, applicant_id.c_str()
		, json_extension.c_str()
		, &CallbackTeamChange
		, cb_pointer);

	return true;
}

bool Team::RejectJoinApplyAsync(const std::string& tid
	, const std::string& applicant_id
	, const std::string& reason
	, const TeamEventCallback& cb
	, const std::string& json_extension/* = ""*/)
{
	if (tid.empty() || applicant_id.empty())
		return false;

	TeamEventCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new TeamEventCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_team_reject_join_apply_async)(tid.c_str()
		, applicant_id.c_str()
		, reason.c_str()
		, json_extension.c_str()
		, &CallbackTeamChange
		, cb_pointer);

	return true;
}

bool Team::AddManagersAsync(const std::string& tid
	, const std::list<std::string>& ids
	, const TeamEventCallback& cb
	, const std::string& json_extension/* = ""*/)
{
	if (tid.empty() || ids.empty())
		return false;

	TeamEventCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new TeamEventCallback(cb);
	}
	std::string uids;
	StrListToJsonString(ids, uids);
	NIM_SDK_GET_FUNC(nim_team_add_managers_async)(tid.c_str()
		, uids.c_str()
		, json_extension.c_str()
		, &CallbackTeamChange
		, cb_pointer);

	return true;
}

bool Team::RemoveManagersAsync(const std::string& tid
	, const std::list<std::string>& ids
	, const TeamEventCallback& cb
	, const std::string& json_extension/* = ""*/)
{
	if (tid.empty() || ids.empty())
		return false;

	TeamEventCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new TeamEventCallback(cb);
	}
	std::string uids;
	StrListToJsonString(ids, uids);
	NIM_SDK_GET_FUNC(nim_team_remove_managers_async)(tid.c_str()
		, uids.c_str()
		, json_extension.c_str()
		, &CallbackTeamChange
		, cb_pointer);

	return true;
}

bool Team::TransferTeamAsync(const std::string& tid
	, const std::string& new_owner_id
	, bool is_leave
	, const TeamEventCallback& cb
	, const std::string& json_extension/* = ""*/)
{
	if (tid.empty() || new_owner_id.empty())
		return false;

	TeamEventCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new TeamEventCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_team_transfer_team_async)(tid.c_str()
		, new_owner_id.c_str()
		, is_leave
		, json_extension.c_str()
		, &CallbackTeamChange
		, cb_pointer);

	return true;
}

bool Team::UpdateMyPropertyAsync(const TeamMemberProperty& prop, const TeamEventCallback& cb, const std::string& json_extension/* = ""*/)
{
	if (prop.GetTeamID().empty())
		return false;

	TeamEventCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new TeamEventCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_team_update_my_property_async)(prop.ToJsonString().c_str()
		, json_extension.c_str()
		, &CallbackTeamChange
		, cb_pointer);

	return true;
}

bool Team::UpdateOtherNickAsync(const TeamMemberProperty& prop, const TeamEventCallback& cb, const std::string& json_extension/* = ""*/)
{
	if (prop.GetTeamID().empty() || prop.GetAccountID().empty())
		return false;

	TeamEventCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new TeamEventCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_team_update_other_nick_async)(prop.ToJsonString().c_str()
		, json_extension.c_str()
		, &CallbackTeamChange
		, cb_pointer);

	return true;
}

bool Team::AcceptInvitationAsync(const std::string& tid
	, const std::string& invitor_id
	, const TeamEventCallback& cb
	, const std::string& json_extension/* = ""*/)
{
	if (tid.empty() || invitor_id.empty())
		return false;

	TeamEventCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new TeamEventCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_team_accept_invitation_async)(tid.c_str()
		, invitor_id.c_str()
		, json_extension.c_str()
		, &CallbackTeamChange
		, cb_pointer);

	return true;
}

bool Team::RejectInvitationAsync(const std::string& tid
	, const std::string& invitor_id
	, const std::string& reason
	, const TeamEventCallback& cb
	, const std::string& json_extension/* = ""*/)
{
	if (tid.empty() || invitor_id.empty())
		return false;

	TeamEventCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new TeamEventCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_team_reject_invitation_async)(tid.c_str()
		, invitor_id.c_str()
		, reason.c_str()
		, json_extension.c_str()
		, &CallbackTeamChange
		, cb_pointer);

	return true;
}

void Team::QueryAllMyTeamsAsync(const QueryAllMyTeamsCallback& cb, const std::string& json_extension/* = ""*/)
{
	QueryAllMyTeamsCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new QueryAllMyTeamsCallback(cb);
	}
	return NIM_SDK_GET_FUNC(nim_team_query_all_my_teams_async)(json_extension.c_str(), &CallbackQueryMyTeams, cb_pointer);
}

void Team::QueryAllMyTeamsInfoAsync(const QueryAllMyTeamsInfoCallback& cb, const std::string& json_extension/* = ""*/)
{
	QueryAllMyTeamsInfoCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new QueryAllMyTeamsInfoCallback(cb);
	}
	return NIM_SDK_GET_FUNC(nim_team_query_all_my_teams_info_async)(json_extension.c_str(), &CallbackQueryAllMyTeamsInfo, cb_pointer);
}

#ifdef NIMAPI_UNDER_WIN_DESKTOP_ONLY
void Team::QueryMyAllMemberInfosAsync( const QueryMyAllMemberInfosCallback& cb, const std::string& json_extension /*= ""*/ )
{
	QueryMyAllMemberInfosCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new QueryMyAllMemberInfosCallback(cb);
	}
	return NIM_SDK_GET_FUNC(nim_team_query_my_all_member_infos_async)(json_extension.c_str(), &CallbackQueryMyAllMemberInfos, cb_pointer);
}
#endif

bool Team::QueryTeamMembersAsync(const std::string& tid, const QueryTeamMembersCallback& cb, const std::string& json_extension/* = ""*/)
{
	if (tid.empty())
		return false;

	QueryTeamMembersCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new QueryTeamMembersCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_team_query_team_members_async)(tid.c_str(), true, json_extension.c_str(), &CallbackQueryTeamMembers, cb_pointer);

	return true;
}

bool Team::QueryTeamMemberAsync(const std::string& tid
	, const std::string& id
	, const QueryTeamMemberCallback& cb
	, const std::string& json_extension/* = ""*/)
{
	if (tid.empty() || id.empty())
		return false;

	QueryTeamMemberCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new QueryTeamMemberCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_team_query_team_member_async)(tid.c_str()
		, id.c_str()
		, json_extension.c_str()
		, &CallbackQueryTeamMember
		, cb_pointer);

	return true;
}

TeamMemberProperty Team::QueryTeamMemberBlock(const std::string& tid, const std::string& id)
{
	const char *team_member_info = NIM_SDK_GET_FUNC(nim_team_query_team_member_block)(tid.c_str(), id.c_str());
	nim::TeamMemberProperty prop;
	ParseTeamMemberPropertyJson(PCharToString(team_member_info), prop);
	Global::FreeBuf((void *)team_member_info);
	return prop;
}

bool Team::QueryTeamInfoAsync(const std::string& tid, const QueryTeamInfoCallback& cb, const std::string& json_extension/* = ""*/)
{
	if (tid.empty())
		return false;

	QueryTeamInfoCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new QueryTeamInfoCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_team_query_team_info_async)(tid.c_str()
		, json_extension.c_str()
		, &CallbackQueryTeamInfo
		, cb_pointer);

	return true;
}

TeamInfo Team::QueryTeamInfoBlock(const std::string& tid)
{
	const char *team_info = NIM_SDK_GET_FUNC(nim_team_query_team_info_block)(tid.c_str());
	nim::TeamInfo tinfo;
	ParseTeamInfoJson(PCharToString(team_info), tinfo);
	Global::FreeBuf((void *)team_info);
	return tinfo;
}

bool Team::QueryTeamInfoOnlineAsync(const std::string& tid, const TeamEventCallback& cb, const std::string& json_extension/* = ""*/)
{
	if (tid.empty())
		return false;

	TeamEventCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new TeamEventCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_team_query_team_info_online_async)(tid.c_str()
		, json_extension.c_str()
		, &CallbackTeamChange
		, cb_pointer);

	return true;
}

bool Team::ParseTeamInfo(const std::string& json_team_info, TeamInfo& team_info)
{
	return ParseTeamInfoJson(json_team_info, team_info);
}

void Team::UnregTeamCb()
{
	g_cb_team_event_ = nullptr;
}

bool Team::MuteMemberAsync(const std::string& tid, const std::string& member_id, bool set_mute, const TeamEventCallback& cb, const std::string& json_extension/* = ""*/)
{
	if (tid.empty() || member_id.empty())
		return false;

	TeamEventCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new TeamEventCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_team_mute_member_async)(tid.c_str()
		, member_id.c_str()
		, set_mute
		, json_extension.c_str()
		, &CallbackTeamChange
		, cb_pointer);

	return true;
}

bool Team::MuteAsync(const std::string& tid, bool set_mute, const TeamEventCallback& cb, const std::string& json_extension/* = ""*/)
{
	if (tid.empty())
		return false;

	TeamEventCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new TeamEventCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_team_mute_async)(tid.c_str()
		, set_mute
		, json_extension.c_str()
		, &CallbackTeamChange
		, cb_pointer);

	return true;
}

static void CallbackQueryMembersOnline(int res_code, int count, const char *tid, const char *result, const char *json_extension, const void *user_data)
{
	if (user_data)
	{
		Team::QueryTeamMembersOnlineCallback* cb_pointer = (Team::QueryTeamMembersOnlineCallback*)user_data;
		if (*cb_pointer)
		{
			Json::Value values;
			Json::Reader reader;
			std::list<TeamMemberProperty> members;
			if (reader.parse(PCharToString(result), values) && values.isArray())
			{
				auto size = values.size();
				for (int i = 0; i < size; i++)
				{
					TeamMemberProperty prop;
					ParseTeamMemberPropertyJson(values[i], prop);
					members.push_back(prop);
				}
			}
			(*cb_pointer)((NIMResCode)res_code, PCharToString(tid), members);
		}
		delete cb_pointer;
	}
}

bool Team::QueryMuteListOnlineAsync(const std::string& tid, const QueryTeamMembersOnlineCallback& cb, const std::string& json_extension/* = ""*/)
{
	if (tid.empty())
		return false;

	QueryTeamMembersOnlineCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new QueryTeamMembersOnlineCallback(cb);
	}
	NIM_SDK_GET_FUNC(nim_team_query_mute_list_online_async)(tid.c_str()
		, json_extension.c_str()
		, &CallbackQueryMembersOnline
		, cb_pointer);

	return true;
}

void Team::TeamMsgAckRead(const std::string& tid, const std::list<IMMessage>& msgs, const TeamEventCallback& cb, const std::string& json_extension/* = ""*/)
{
	TeamEventCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new TeamEventCallback(cb);
	}
	std::string json_msgs;
	Json::Value values;
	for (auto iter = msgs.begin(); iter != msgs.end(); ++iter)
	{
		values.append(iter->ToJsonObject(false));
	}
	NIM_SDK_GET_FUNC(nim_team_msg_ack_read)(tid.c_str()
		, values.toStyledString().c_str()
		, json_extension.c_str()
		, &CallbackTeamChange
		, cb_pointer);
}

void Team::TeamMsgQueryUnreadList(const std::string& tid, const IMMessage& msg, const TeamEventCallback& cb, const std::string& json_extension/* = ""*/)
{
	TeamEventCallback* cb_pointer = nullptr;
	if (cb)
	{
		cb_pointer = new TeamEventCallback(cb);
	}

	NIM_SDK_GET_FUNC(nim_team_msg_query_unread_list)(tid.c_str()
		, msg.ToJsonString(false).c_str()
		, json_extension.c_str()
		, &CallbackTeamChange
		, cb_pointer);
}

}
