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

	// ͳ����ʼʱ��
	FILETIME preidleTime;
	FILETIME prekernelTime;
	FILETIME preuserTime;
	// ͳ�ƽ���ʱ��
	FILETIME idleTime;
	FILETIME kernelTime;
	FILETIME userTime;

	res = ::GetSystemTimes(&preidleTime, &prekernelTime, &preuserTime);
	if (!res)
	{
		QLOG_ERR(L"GetSystemTimes error: {0}.") << ::GetLastError();
		return 0;
	}

	hEvent = CreateEvent(NULL, FALSE, FALSE, NULL); // ��ʼֵΪ nonsignaled ������ÿ�δ������Զ�����Ϊnonsignaled  
	while (1)
	{
		WaitForSingleObject(hEvent, 500); //�ȴ�500����  
		res = ::GetSystemTimes(&idleTime, &kernelTime, &userTime);
		if (!res)
		{
			QLOG_ERR(L"GetSystemTimes error: {0}.") << ::GetLastError();
			return 0;
		}

		__int64 idle = CompareFileTimeEx(preidleTime, idleTime);
		__int64 kernel = CompareFileTimeEx(prekernelTime, kernelTime);
		__int64 user = CompareFileTimeEx(preuserTime, userTime);

		//kernel + user�����к��ĵ�CPU��ʱ�䡣
		//���磺ͳ������Ϊ1�룬CPUΪ4���ģ�����FILETIME��100����Ϊ��λ�����kernel + user��ֵӦ����40000000�����Դ�ӡ��֤��
		cpu_percent = (int)((kernel + user - idle) * 100 / (kernel + user));//���ܵ�ʱ��-����ʱ�䣩/�ܵ�ʱ��=ռ��cpu��ʱ�����ʹ����
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
	HANDLE process_handle = ::OpenProcess(PROCESS_QUERY_INFORMATION, false, process_id); //ȡ�ý��̾��
	if (process_handle == NULL)
	{
		QLOG_ERR(L"Get Process Handle error: {0}.") << ::GetLastError();
		return 0;
	}

	FILETIME preSingleCoreTotalTime; //���˵�CPU��ʱ��
	FILETIME preProcessKernelTime; //���̵��ں�ʱ��
	FILETIME preProcessUserTime; //���̵��û�ʱ��
	FILETIME processCreateTime; //��Ϊ����������ʹ��
	FILETIME processExitTime; //��Ϊ����������ʹ��
	::GetSystemTimeAsFileTime(&preSingleCoreTotalTime); //��ȡϵͳ��ǰʱ��
	BOOL res = ::GetProcessTimes(process_handle, &processCreateTime, &processExitTime, &preProcessKernelTime, &preProcessUserTime);
	if (!res)
	{
		QLOG_ERR(L"GetProcessTimes error: {0}.") << ::GetLastError();
		return 0;
	}

	HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, NULL); // ��ʼֵΪ nonsignaled ������ÿ�δ������Զ�����Ϊnonsignaled
	int process_cpu_percent = 0;
	while (1)
	{
		WaitForSingleObject(hEvent, 500); //�ȴ�500���� 

		FILETIME singleCoreTotalTime; //���˵�CPU��ʱ��
		FILETIME processKernelTime;
		FILETIME processUserTime;
		::GetSystemTimeAsFileTime(&singleCoreTotalTime); //��ȡϵͳ��ǰʱ��
		res = ::GetProcessTimes(process_handle, &processCreateTime, &processExitTime, &processKernelTime, &processUserTime);
		if (!res)
		{
			QLOG_ERR(L"GetSystemTimes error: {0}.") << ::GetLastError();
			return 0;
		}

		SYSTEM_INFO system_info;
		GetSystemInfo(&system_info);
		int core_num = system_info.dwNumberOfProcessors; //���ĸ���

		__int64 total_cpu_time = CompareFileTimeEx(preSingleCoreTotalTime, singleCoreTotalTime) * core_num; //CPU��ʱ�� = ����CPUʱ�� �� ������
		__int64 process_kernel = CompareFileTimeEx(preProcessKernelTime, processKernelTime);
		__int64 process_user = CompareFileTimeEx(preProcessUserTime, processUserTime);

		process_cpu_percent = (int)((process_kernel + process_user) * 100 / total_cpu_time); //������CPUʱ�� / ��CPUʱ�� = ���̵�CPUռ����
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
	HANDLE process_handle = ::OpenProcess(PROCESS_QUERY_INFORMATION, false, process_id); //ȡ�ý��̾��
	if (process_handle == NULL)
	{
		QLOG_ERR(L"Get Process Handle error: {0}.") << ::GetLastError();
		return 0;
	}

	PROCESS_MEMORY_COUNTERS pmc;
	int memoryInKB = 0;

	if (GetProcessMemoryInfo(process_handle, &pmc, sizeof(pmc)))
		memoryInKB = pmc.WorkingSetSize / 1024; //��λΪKB
	else
		QLOG_ERR(L"Get Process Memory Info error: {0}.") << ::GetLastError();

	CloseHandle(process_handle);

	return memoryInKB;
}

//���̵Ĺ�������Ϊר�ù������͹���������ר�ù�������С�ǽ��̵�ʵ���ڴ�ӵ������
DWORD SystemPerformanceCollector::GetProcessPrivateWorkingSet(DWORD process_id)
{
	//ȡ�ý��̾��
	// Ҫ��������̵�Ȩ�ޣ�PROCESS_QUERY_INFORMATION and PROCESS_VM_READ
	HANDLE process_handle = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, process_id); 
	if (process_handle == NULL)
	{
		QLOG_ERR(L"Get Process Handle error: {0}.") << ::GetLastError();
		return 0;
	}

	//��ȡһҳ�Ĵ�С����λ���ֽڣ�
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

	bRet = ::QueryWorkingSet(process_handle, &workSetInfo, sizeof(workSetInfo)); // ��һ�ε��û�ȡʵ�ʻ�������С
	ULONG_PTR NumberOfEntries = workSetInfo.NumberOfEntries;
	if (!bRet) //����ʧ��
	{
		bool err_bad_len = ::GetLastError() == ERROR_BAD_LENGTH;
		int try_count = 3;
		while (err_bad_len && try_count > 0) //��Ҫ���·��仺����
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
				// ���»�ȡ
				bRet = ::QueryWorkingSet(process_handle, pByte, realSize);
				if (!bRet)
				{
					err_bad_len = ::GetLastError() == ERROR_BAD_LENGTH;
					if (err_bad_len)
						NumberOfEntries = ((PSAPI_WORKING_SET_INFORMATION*)pByte)->NumberOfEntries;
					delete[] pByte; // �����ڴ�
					pByte = NULL;
					continue;
				}
				break;
			}
			catch (std::exception *e) // �����ڴ�ʧ��
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
		if (!pWorkSetBlock[i].Shared) // ������ǹ���ҳ������ר��ҳ
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


