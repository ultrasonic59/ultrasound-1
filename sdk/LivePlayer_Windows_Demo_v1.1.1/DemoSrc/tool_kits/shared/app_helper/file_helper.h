#ifndef _UNICORN_WIN32_UTIL_FILE_UTIL_H_
#define _UNICORN_WIN32_UTIL_FILE_UTIL_H_
#pragma once

#include <string>

namespace shared
{
std::wstring GetAppDataPath(const std::wstring& post_filename);
std::wstring GetDesktopPath();
std::wstring GetAppInstallPath();
}

#endif //_UNICORN_WIN32_UTIL_FILE_UTIL_H_