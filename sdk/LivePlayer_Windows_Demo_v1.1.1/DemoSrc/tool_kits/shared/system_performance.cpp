#include "stdafx.h"
#include "system_performance.h"
#include "log/log.h"
#include <Psapi.h>

__int64 SystemPerformanceCollector::CompareFileTimeEx(FILETIME time1, FILETIME time2)
{
	__int64 a = __int64(time1.dwHighDateTime) << 32 | time1.dwLowDateTime;
	__int64 b = __int64(time2.dwHighDateTime) << 32 | time2.dwLowDateTime;
	return   b - a;
}

int SystemPerformanceCollector::GetTotalCPUUsingPercent()
{
	int cpu_percent = 0;

	HANDLE hEvent;
	BOOL res;

	// 统计起始时间
	FILETIME preidleTime;
	FILETIME prekernelTime;
	FILETIME preuserTime;
	// 统计结束时间
	FILETIME idleTime;
	FILETIME kernelTime;
	FILETIME userTime;

	res = ::GetSystemTimes(&preidleTime, &prekernelTime, &preuserTime);
	if (!res)
	{
		QLOG_ERR(L"GetSystemTimes error: {0}.") << ::GetLastError();
		return 0;
	}

	hEvent = CreateEvent(NULL, FALSE, FALSE, NULL); // 初始值为 nonsignaled ，并且每次触发后自动设置为nonsignaled  
	while (1)
	{
		WaitForSingleObject(hEvent, 500); //等待500毫秒  
		res = ::GetSystemTimes(&idleTime, &kernelTime, &userTime);
		if (!res)
		{
			QLOG_ERR(L"GetSystemTimes error: {0}.") << ::GetLastError();
			return 0;
		}

		__int64 idle = CompareFileTimeEx(preidleTime, idleTime);
		__int64 kernel = CompareFileTimeEx(prekernelTime, kernelTime);
		__int64 user = CompareFileTimeEx(preuserTime, userTime);

		//kernel + user是所有核心的CPU总时间。
		//例如：统计周期为1秒，CPU为4核心，由于FILETIME以100纳秒为单位，因此kernel + user的值应该是40000000，可以打印验证。
		cpu_percent = (int)((kernel + user - idle) * 100 / (kernel + user));//（总的时间-空闲时间）/总的时间=占用cpu的时间就是使用率
		if (cpu_percent < 0)
			cpu_percent = 0;
		if (cpu_percent > 100)
			cpu_percent = 100;

		preidleTime = idleTime;
		prekernelTime = kernelTime;
		preuserTime = userTime;
		break;
	}

	return cpu_percent;
}

int SystemPerformanceCollector::GetProcessCPUPercent(DWORD process_id)
{
	HANDLE process_handle = ::OpenProcess(PROCESS_QUERY_INFORMATION, false, process_id); //取得进程句柄
	if (process_handle == NULL)
	{
		QLOG_ERR(L"Get Process Handle error: {0}.") << ::GetLastError();
		return 0;
	}

	FILETIME preSingleCoreTotalTime; //单核的CPU总时间
	FILETIME preProcessKernelTime; //进程的内核时间
	FILETIME preProcessUserTime; //进程的用户时间
	FILETIME processCreateTime; //仅为填充参数，不使用
	FILETIME processExitTime; //仅为填充参数，不使用
	::GetSystemTimeAsFileTime(&preSingleCoreTotalTime); //获取系统当前时间
	BOOL res = ::GetProcessTimes(process_handle, &processCreateTime, &processExitTime, &preProcessKernelTime, &preProcessUserTime);
	if (!res)
	{
		QLOG_ERR(L"GetProcessTimes error: {0}.") << ::GetLastError();
		return 0;
	}

	HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, NULL); // 初始值为 nonsignaled ，并且每次触发后自动设置为nonsignaled
	int process_cpu_percent = 0;
	while (1)
	{
		WaitForSingleObject(hEvent, 500); //等待500毫秒 

		FILETIME singleCoreTotalTime; //单核的CPU总时间
		FILETIME processKernelTime;
		FILETIME processUserTime;
		::GetSystemTimeAsFileTime(&singleCoreTotalTime); //获取系统当前时间
		res = ::GetProcessTimes(process_handle, &processCreateTime, &processExitTime, &processKernelTime, &processUserTime);
		if (!res)
		{
			QLOG_ERR(L"GetSystemTimes error: {0}.") << ::GetLastError();
			return 0;
		}

		SYSTEM_INFO system_info;
		GetSystemInfo(&system_info);
		int core_num = system_info.dwNumberOfProcessors; //核心个数

		__int64 total_cpu_time = CompareFileTimeEx(preSingleCoreTotalTime, singleCoreTotalTime) * core_num; //CPU总时间 = 单核CPU时间 × 核心数
		__int64 process_kernel = CompareFileTimeEx(preProcessKernelTime, processKernelTime);
		__int64 process_user = CompareFileTimeEx(preProcessUserTime, processUserTime);

		process_cpu_percent = (int)((process_kernel + process_user) * 100 / total_cpu_time); //进程总CPU时间 / 总CPU时间 = 进程的CPU占用率
		if (process_cpu_percent < 0)
			process_cpu_percent = 0;
		if (process_cpu_percent > 100)
			process_cpu_percent = 100;

		preSingleCoreTotalTime = singleCoreTotalTime;
		preProcessKernelTime = processKernelTime;
		preProcessUserTime = processUserTime;
		break;
	}

	CloseHandle(process_handle);

	return process_cpu_percent;
}

int SystemPerformanceCollector::GetTotalMemoryUsingPercent()
{
	MEMORYSTATUSEX mem_status_ex = { sizeof(MEMORYSTATUSEX) };
	::GlobalMemoryStatusEx(&mem_status_ex);
	return mem_status_ex.dwMemoryLoad;
}

DWORD SystemPerformanceCollector::GetProcessTotalWorkingSet(DWORD process_id)
{
	HANDLE process_handle = ::OpenProcess(PROCESS_QUERY_INFORMATION, false, process_id); //取得进程句柄
	if (process_handle == NULL)
	{
		QLOG_ERR(L"Get Process Handle error: {0}.") << ::GetLastError();
		return 0;
	}

	PROCESS_MEMORY_COUNTERS pmc;
	int memoryInKB = 0;

	if (GetProcessMemoryInfo(process_handle, &pmc, sizeof(pmc)))
		memoryInKB = pmc.WorkingSetSize / 1024; //单位为KB
	else
		QLOG_ERR(L"Get Process Memory Info error: {0}.") << ::GetLastError();

	CloseHandle(process_handle);

	return memoryInKB;
}

//进程的工作集分为专用工作集和共享工作集。专用工作集大小是进程的实际内存拥有量。
DWORD SystemPerformanceCollector::GetProcessPrivateWorkingSet(DWORD process_id)
{
	//取得进程句柄
	// 要求操作进程的权限：PROCESS_QUERY_INFORMATION and PROCESS_VM_READ
	HANDLE process_handle = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, process_id); 
	if (process_handle == NULL)
	{
		QLOG_ERR(L"Get Process Handle error: {0}.") << ::GetLastError();
		return 0;
	}

	//获取一页的大小（单位：字节）
	PERFORMANCE_INFORMATION performanceInfo;
	memset(&performanceInfo, 0, sizeof(performanceInfo));
	if (!::GetPerformanceInfo(&performanceInfo, sizeof(performanceInfo)))
	{
		::CloseHandle(process_handle);
		return 0;
	}
	int page_size_in_byte = performanceInfo.PageSize;

	BOOL bRet = TRUE;
	PSAPI_WORKING_SET_INFORMATION workSetInfo;
	PBYTE pByte = NULL;
	PSAPI_WORKING_SET_BLOCK *pWorkSetBlock = workSetInfo.WorkingSetInfo;
	memset(&workSetInfo, 0, sizeof(workSetInfo));

	bRet = ::QueryWorkingSet(process_handle, &workSetInfo, sizeof(workSetInfo)); // 第一次调用获取实际缓冲区大小
	ULONG_PTR NumberOfEntries = workSetInfo.NumberOfEntries;
	if (!bRet) //调用失败
	{
		bool err_bad_len = ::GetLastError() == ERROR_BAD_LENGTH;
		int try_count = 3;
		while (err_bad_len && try_count > 0) //需要重新分配缓冲区
		{
			try_count--;
			DWORD realSize = sizeof(NumberOfEntries) + NumberOfEntries * sizeof(PSAPI_WORKING_SET_BLOCK);
			realSize += (realSize / page_size_in_byte + 10) * sizeof(PSAPI_WORKING_SET_BLOCK);
			try
			{
				if (pByte)
					delete[] pByte;
				pByte = new BYTE[realSize];
				memset(pByte, 0, realSize);
				pWorkSetBlock = (PSAPI_WORKING_SET_BLOCK *)(pByte + sizeof(workSetInfo.NumberOfEntries));
				// 重新获取
				bRet = ::QueryWorkingSet(process_handle, pByte, realSize);
				if (!bRet)
				{
					err_bad_len = ::GetLastError() == ERROR_BAD_LENGTH;
					if (err_bad_len)
						NumberOfEntries = ((PSAPI_WORKING_SET_INFORMATION*)pByte)->NumberOfEntries;
					delete[] pByte; // 清理内存
					pByte = NULL;
					continue;
				}
				break;
			}
			catch (std::exception *e) // 分配内存失败
			{
				QLOG_ERR(L"Query working set error: {0}") << e->what();
				::CloseHandle(process_handle);
				return 0;
			}
		}
		if (!bRet)
		{
			::CloseHandle(process_handle);
			return 0;
		}
	}

	SIZE_T workSetPrivate = 0;
	for (ULONG_PTR i = 0; i < NumberOfEntries; ++i)
	{
		if (!pWorkSetBlock[i].Shared) // 如果不是共享页，就是专用页
			workSetPrivate += page_size_in_byte;
	}
	if (pByte)
		delete[] pByte;

	::CloseHandle(process_handle);
	return (DWORD)(workSetPrivate / 1024);
}

DWORD SystemPerformanceCollector::GetTotalPhysMemorySize()
{
	MEMORYSTATUSEX mem_status_ex = { sizeof(MEMORYSTATUSEX) };
	::GlobalMemoryStatusEx(&mem_status_ex);
	return (DWORD)(mem_status_ex.ullTotalPhys / 1024);
}


