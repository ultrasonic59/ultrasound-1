#ifndef _UNICORN_WIN32_UTIL_LOG_UTIL_H_
#define _UNICORN_WIN32_UTIL_LOG_UTIL_H_
#pragma once

#include <string>
#include "log.h"

namespace shared
{

void InitAppLog(const std::wstring& log_file_path);
void DoClearLog();

}

#endif //_UNICORN_WIN32_UTIL_LOG_UTIL_H_