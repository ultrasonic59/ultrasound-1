#include "stdafx.h"
#include "util.h"
#include "shared/log/log.h"
#include "shellapi.h"

std::wstring QPath::GetAppPath()
{
	return nbase::win32::GetCurrentModuleDirectory();
}

std::wstring QPath::GetLocalAppDataDir(const std::wstring& app_data_dir)
{
	return nbase::win32::GetLocalAppDataDir() + app_data_dir;
}

//
//
std::map<std::wstring,std::wstring> QCommand::key_value_;

void QCommand::ParseCommand( const std::wstring &cmd )
{
	LPWSTR *szArgList;
	int argCount;
	szArgList = ::CommandLineToArgvW(cmd.c_str(), &argCount);

	key_value_.clear();
	for (int i = 0; i < argCount;)
	{
		std::wstring command;
		std::wstring param;

		command = szArgList[i];
		if (command[0] == L'/')
			command.erase(command.begin()); //ȥ�������־'/'
		i++;

		while (i < argCount)
		{
			std::wstring temp = szArgList[i];
			if (!temp.empty() && temp[0] != L'/') //����command�����һ������
				param += temp + L" "; //ͬһ������Ķ������֮���ÿո����
			else if (!temp.empty() && temp[0] == L'/') //���Ǹ��µ�����
				break;
			i++;
		}

		if (!command.empty())
		{
			if (!param.empty() && param.back() == ' ')
				param.pop_back(); //ȥ��paramĩβ�Ŀո�
			key_value_[command] = param; //��������Ͳ���
		}
	}
	::LocalFree(szArgList);
}

std::wstring QCommand::Get( const std::wstring &key )
{
	std::map<std::wstring,std::wstring>::const_iterator i = key_value_.find(key);
	if(i == key_value_.end())
		return L"";
	else
		return i->second;
}

void QCommand::Set( const std::wstring &key, const std::wstring &value )
{
	key_value_[key] = value;
}

void QCommand::Erase(const std::wstring &key)
{
	key_value_.erase(key);
}

bool QCommand::AppStartWidthCommand( const std::wstring &app, const std::wstring &cmd )
{
	HINSTANCE hInst = ::ShellExecuteW(NULL, L"open", app.c_str(), cmd.c_str(), NULL, SW_SHOWNORMAL);
	return (int)hInst > 32;
}

bool QCommand::RestartApp(const std::wstring &cmd)
{
	wchar_t app[1024] = { 0 };
	GetModuleFileName(NULL, app, 1024);
	HINSTANCE hInst = ::ShellExecuteW(NULL, L"open", app, cmd.c_str(), NULL, SW_SHOWNORMAL);
	return (int)hInst > 32;
}