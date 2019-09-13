/*
 *
 *	Author       HarrisonFeng
 *	Email        harrisonfeng@corp.netease.com
 *	Date         2013-8-7
 *	Copyright    Hangzhou, Netease Inc.
 *	Brief        注册表操作函数
 *
 */

#ifndef YIXIN_WIN_UTIL_REGISTER_TOOL_H_
#define YIXIN_WIN_UTIL_REGISTER_TOOL_H_

#include <atlbase.h>
#include <atlwin.h>
#include <string>

class RegisterTool
{
public:
	RegisterTool(void);
	~RegisterTool(void);

public:
	//读写注册表方法
	static BOOL RegSetValue(HKEY root_key, const std::wstring& sub_key, const std::wstring& value, DWORD type, const std::wstring& data);
	static BOOL RegQueryValue(HKEY root_key, const std::wstring& sub_key, const std::wstring& value, std::wstring& data);
	static BOOL RegDeleteKey(HKEY root_key, const std::wstring& sub_key);
	static BOOL RegDeleteValue(HKEY root_key, const std::wstring& sub_key, const std::wstring& value);
};

#endif // YIXIN_WIN_UTIL_REGISTER_TOOL_H_
