#pragma once
#include "shared/util.h"
#include <DbgHelp.h>

BOOL CALLBACK MyMiniDumpCallback(PVOID, const PMINIDUMP_CALLBACK_INPUT input, PMINIDUMP_CALLBACK_OUTPUT output)
{
	if(input == NULL || output == NULL)
		return FALSE;

	BOOL ret = FALSE;
	switch(input->CallbackType)
	{
	case IncludeModuleCallback:
	case IncludeThreadCallback:
	case ThreadCallback:
	case ThreadExCallback:
		ret = TRUE;
		break;
	case ModuleCallback:
		{
			if( !(output->ModuleWriteFlags & ModuleReferencedByMemory) )
			{
				output->ModuleWriteFlags &= ~ModuleWriteModule;
			}
			ret = TRUE;
		}
		break;
	default:
		break;
	}

	return ret;
}

void WriteDump(EXCEPTION_POINTERS* exp, const std::wstring &path)
{
	HANDLE h = ::CreateFile(path.c_str(), GENERIC_WRITE | GENERIC_READ, FILE_SHARE_WRITE | FILE_SHARE_READ, 
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	MINIDUMP_EXCEPTION_INFORMATION info;
	info.ThreadId = ::GetCurrentThreadId();
	info.ExceptionPointers = exp;
	info.ClientPointers = NULL;

	MINIDUMP_CALLBACK_INFORMATION mci; 
	mci.CallbackRoutine = (MINIDUMP_CALLBACK_ROUTINE)MyMiniDumpCallback; 
	mci.CallbackParam = 0; 

	MINIDUMP_TYPE mdt = (MINIDUMP_TYPE)(MiniDumpWithIndirectlyReferencedMemory | MiniDumpScanMemory); 

	MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), h, mdt, &info, NULL, &mci);
	::CloseHandle(h);
}

LONG WINAPI MyUnhandledExceptionFilter(EXCEPTION_POINTERS* exp)
{
	nbase::Time::TimeStruct qt = nbase::Time::Now().ToTimeStruct(true);
	std::wstring file = nbase::StringPrintf(L"%04d%02d%02d_%02d%02d%02d.dmp", 
		qt.year_, qt.month_, qt.day_of_month_, qt.hour_, qt.minute_, qt.second_);
	std::wstring dir = QPath::GetLocalAppDataDir(APP_DATA_DIR);
	dir.append(file);

	WriteDump(exp, dir);

	QCommand::RestartApp(L"/" CMD_APP_CRASH L" 1");

	return EXCEPTION_EXECUTE_HANDLER;
}