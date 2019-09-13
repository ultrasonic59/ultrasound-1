#pragma once

class SystemPerformanceCollector
{
public:
	//CPU
	static __int64 CompareFileTimeEx(FILETIME time1, FILETIME time2); //��������FILETIME��ʱ�����ڼ���CPUʹ����
	static int GetTotalCPUUsingPercent(); //����ϵͳCPUʹ���ܰٷֱ�
	static int GetProcessCPUPercent(DWORD process_id); //����ָ�����̵�CPUռ����

	//�ڴ�
	static int GetTotalMemoryUsingPercent(); //����ϵͳ���ڴ�ʹ�ðٷֱ�
	static DWORD GetProcessTotalWorkingSet(DWORD process_id); //����ָ�����̵��ܹ�������С��KB��
	static DWORD GetProcessPrivateWorkingSet(DWORD process_id); //����ָ�����̵�ר�ù�������С��KB��
	static DWORD GetTotalPhysMemorySize(); //����ϵͳ�������ڴ棨KB��
};
