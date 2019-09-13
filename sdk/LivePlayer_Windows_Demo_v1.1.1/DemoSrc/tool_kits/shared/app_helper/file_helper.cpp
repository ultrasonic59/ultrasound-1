#include <assert.h>
#include "stdafx.h"
#include "file_helper.h"
#include "base/win32/path_util.h"
#include "base/file/file_util.h"
#include "base/util/string_util.h"
#include "shlobj.h"

namespace shared
{

std::wstring GetAppDataPath(const std::wstring& post_filename)
{
	std::wstring app_data_path = nbase::win32::GetLocalAppDataDir() + post_filename + L"\\";
	if (!nbase::FilePathIsExist(app_data_path, true)) {
		nbase::CreateDirectory(app_data_path);
	}
	return app_data_path;
}

std::wstring GetDesktopPath()
{
	wchar_t path[MAX_PATH + 1];
	if (SHGetSpecialFolderPathW(NULL, path, CSIDL_DESKTOPDIRECTORY, FALSE) == NOERROR )
	{
		path[MAX_PATH + 1] = (wchar_t)'\0';
	}
	return (std::wstring)path + L"\\";
}

std::wstring GetAppInstallPath()
{
	return nbase::win32::GetCurrentModuleDirectory();
}

}
