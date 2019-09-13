#include "stdafx.h"
#include "register_tool.h"

RegisterTool::RegisterTool(void)
{
}

RegisterTool::~RegisterTool(void)
{
}


//读写注册表方法
BOOL RegisterTool::RegSetValue(HKEY root_key, 
	const std::wstring& sub_key, 
	const std::wstring& value, 
	DWORD type, 
	const std::wstring& data)
{
	HKEY hKEY;
	long ret = 0;
	ret = ::RegOpenKeyEx(root_key, sub_key.c_str(), 0, KEY_ALL_ACCESS, &hKEY);
	if (ret != ERROR_SUCCESS)  // 该键值不存在
	{
		::RegCloseKey(hKEY);
		DWORD label;
		//创建
		long ret = ::RegCreateKeyEx(root_key, sub_key.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKEY, &label);
		if (ret != ERROR_SUCCESS)
		{
			return FALSE;
		}        
	}

	DWORD bufsize = data.size()+1;
	wchar_t* data_buf  = new wchar_t[bufsize];
	ZeroMemory(data_buf, sizeof(wchar_t) * bufsize);
	wcscpy_s(data_buf, bufsize, data.c_str());
	ret = ::RegSetValueEx(hKEY, value.c_str(), NULL, type, (LPBYTE)data_buf, bufsize * sizeof(wchar_t));
	if (ret != ERROR_SUCCESS)
	{
		::RegCloseKey(hKEY);
		delete [] data_buf;
		return FALSE;
	}

	::RegCloseKey(hKEY);
	delete [] data_buf;

	return TRUE;
}

BOOL RegisterTool::RegQueryValue(HKEY root_key, 
	const std::wstring& sub_key, 
	const std::wstring& value,
	std::wstring& out_data)
{
	HKEY hKEY;
	long ret = 0;
	ret = ::RegOpenKeyEx(root_key, sub_key.c_str(), 0, KEY_READ, &hKEY);
	if (ret != ERROR_SUCCESS)  //该键值不存在
	{
		::RegCloseKey(hKEY);
		return FALSE;
	}

	DWORD datasize  = MAX_PATH*10;
	wchar_t  data_buf[MAX_PATH*10];
	memset(data_buf,0,datasize);
	DWORD dType = REG_SZ;
	ret = ::RegQueryValueEx(hKEY, value.c_str(), NULL, &dType, (LPBYTE)data_buf, &datasize);
	if (ret != ERROR_SUCCESS)
	{
		::RegCloseKey(hKEY);
		return FALSE;
	}

	datasize = datasize/sizeof(wchar_t);
	data_buf[datasize] = 0;
	out_data.clear();
	out_data.append(data_buf);

	::RegCloseKey(hKEY);

	return TRUE;
}

BOOL RegisterTool::RegDeleteKey(HKEY root_key, const std::wstring& sub_key)
{
	long ret = ::RegDeleteKey(root_key, sub_key.c_str());
	if (ret != ERROR_SUCCESS)  //删除该键值失败
	{
		return FALSE;
	}

	return TRUE;
}

BOOL RegisterTool::RegDeleteValue(HKEY root_key, const std::wstring& sub_key, const std::wstring& value)
{
	HKEY hKEY;
	long ret = 0;
	ret = ::RegOpenKeyEx(root_key, sub_key.c_str(), 0, KEY_ALL_ACCESS, &hKEY);
	if (ret != ERROR_SUCCESS)  //该键值不存在
	{
		::RegCloseKey(hKEY);
		return FALSE;
	}

	ret = ::RegDeleteValue(hKEY, value.c_str());
	if (ret != ERROR_SUCCESS)  //删除该Value失败
	{
		::RegCloseKey(hKEY);
		return FALSE;
	}

	::RegCloseKey(hKEY);

	return TRUE;
}
