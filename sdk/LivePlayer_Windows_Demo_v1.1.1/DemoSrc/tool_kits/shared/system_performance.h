#pragma once

class SystemPerformanceCollector
{
public:
	//CPU
	static __int64 CompareFileTimeEx(FILETIME time1, FILETIME time2); //返回两个FILETIME的时间差，用于计算CPU使用率
	static int GetTotalCPUUsingPercent(); //返回系统CPU使用总百分比
	static int GetProcessCPUPercent(DWORD process_id); //返回指定进程的CPU占用率

	//内存
	static int GetTotalMemoryUsingPercent(); //返回系统总内存使用百分比
	static DWORD GetProcessTotalWorkingSet(DWORD process_id); //返回指定进程的总工作集大小（KB）
	static DWORD GetProcessPrivateWorkingSet(DWORD process_id); //返回指定进程的专用工作集大小（KB）
	static DWORD GetTotalPhysMemorySize(); //返回系统总物理内存（KB）
};
