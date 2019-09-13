#include "stdafx.h"
#include "log_helper.h"
#include "../app_helper/file_helper.h"

namespace shared
{

#ifdef _DEBUG
	const LOG_LEVEL kDefaultLogLevel = LV_PRO;
#else
	const LOG_LEVEL kDefaultLogLevel = LV_APP;
#endif
	LOG_LEVEL g_log_level = kDefaultLogLevel;

	void InitAppLog(const std::wstring& log_file_path)
	{
		QLogImpl::GetInstance()->SetLogLevel(g_log_level);
		std::wstring dir;
		nbase::FilePathApartDirectory(log_file_path, dir);
		if (!nbase::FilePathIsExist(dir, true))
			nbase::CreateDirectory(dir);
		QLogImpl::GetInstance()->SetLogFile(log_file_path);
	}

}
