#ifndef SHARED_TOOL_H_
#define SHARED_TOOL_H_

#include <string>
#include <vector>
#include "base/time/time.h"
#include "base/base_types.h"

namespace shared
{
namespace tools
{

// 在FrameworkThread的线程过程中调用
void InitTool();

// os
int			GetOSVersion();
std::string GetOSVersionString();
bool		IsWow64();
bool		GetFileVersion(const wchar_t *file_path, WORD *major_version, WORD *minor_version, WORD *build_number, WORD *revision_number);

// string
std::string	FormatTime(nbase::Time::TimeStruct	t);
std::wstring FormatBytes(double bytes);
std::wstring FormatBps(double bytes);
void SplitString(const std::string &str, const std::string &seperator, std::vector<std::string> &array);
void SplitString(const std::wstring &str, const std::wstring &seperator, std::vector<std::wstring> &array);
std::string StringReverse(const std::string &input);
std::string GetUUID();

//过滤无法显示的表情字符
std::string FilterUCS4(const std::string& str, bool replace_spaces = true);

// regex
bool IsMobilePhoneNumber(const std::string &str);
//号码能否发短信
bool IsChinaPhoneNumber(const std::string &str);
bool IsAuthCode(const std::string &str);

// file
std::wstring GetSpecialFolder(int id);
std::wstring GetResourcePath(void);
void MakeExtendPath(std::wstring &path);
void ClearFile(const wchar_t *path, long file_length_limit);
bool FilePathIsExist(const std::string &utf8_filepath_in, bool is_directory);
// 采用递归的方式查找指定目下的文件
bool FindFile(const std::wstring &dir,const std::wstring &file_desc,std::wstring &file_path);

bool CompressFile(const std::wstring &file_path, std::string &compressed_data);
bool CompressData(const std::string &raw_data, std::string &compressed_data);
bool UncompressData(const std::string &compressed_data, std::string &raw_data);

std::wstring EnsureValidFileName(const std::wstring &file_name, 
	const std::wstring &find = L"\\/:*?\"<>|", 
	const std::wstring &replace = L"_");

bool AddPathToEnvironmentVariable(const std::wstring& path);

// math
int CalcAvg(const std::vector<int> &vec, bool is_revise = true);
int CalcStDev(const std::vector<int> &vec);

// 打开网址
void SafeOpenUrl(const std::wstring &url, int show_cmd);
void SafeOpenUrlEx(const std::string &url, int show_cmd);

bool RestartComputer();
bool RunAppWithRedirection(const std::wstring &app_path, const std::wstring &app_cmd, std::string &output);

// 生成时间戳，精度毫秒
std::string GenerateTimeStamp();

//Register Server
BOOL DllRegisterServer(LPCTSTR lpszFileName);
BOOL DllUnregisterServer(LPCTSTR lpszFileName);

#ifdef WITH_ENCRYPT

//获取字符串的MD5
std::string GetMD5(const std::string &source);
// 计算文件md5
void CalculateFileMd5(const std::wstring& file_path, std::string& md5_value);

#endif

// 进程通信message
void RegisterShowWndForgroudMessage();
UINT GetShowWndForgroudMessageID();

//获取图片类型对应的CLSID
bool GetImageCLSID(const wchar_t* format, CLSID* pCLSID);

//获取物理网卡mac地址
//std::string GetDevInfoMac();
//获取硬盘序列号
//std::string GetDevInfoHardDisk();
//获取主板序列号
//std::string GetDevInfoBIOS();
//获取设备uuid
//std::string GetDevInfoUUID();

//遍历文件下的文件（仅限第一层）
struct FileItem
{
	std::wstring file_;
	long size_;
};

typedef std::vector<std::wstring> StringVector;

typedef std::vector<FileItem> FileItemVector;

class File
{
public:
	//dir最后不需要"\\"
	static void GetFolder(const std::wstring &dir, StringVector &out, bool full_path = false); 
	//dir最后不需要"\\"
	static void GetFile(const std::wstring &dir, std::vector<FileItem> &out, bool full_path = false); 
};


//////////////////////////////////////////////////////////////////////////
//从模块查找并调用有返回值的函数
template<typename ProcPtrType, typename ProcReturnType, typename... ProcArgTypes>
ProcReturnType LoadModuleAndCallProc(const wchar_t* dll_name, const char* proc_name, ProcReturnType err_ret, ProcArgTypes... args)
{
	ProcReturnType ret = err_ret;

	HMODULE hModule = ::LoadLibrary(dll_name);
	if (hModule == nullptr)
		return ret;

	ProcPtrType pProc = (ProcPtrType)::GetProcAddress(hModule, proc_name);
	if (pProc)
		ret = pProc(args...);

	::FreeLibrary(hModule);
	return ret;
}

//从模块查找并调用无返回值的函数
template<typename ProcPtrType, typename... ProcArgTypes>
void LoadModuleAndCallProcVoid(const wchar_t* dll_name, const char* proc_name, bool& call_succeed, ProcArgTypes... args)
{
	call_succeed = false;

	HMODULE hModule = ::LoadLibrary(dll_name);
	if (hModule == nullptr)
		return;

	ProcPtrType pProc = (ProcPtrType)::GetProcAddress(hModule, proc_name);
	if (pProc)
	{
		pProc(args...);
		call_succeed = true;
	}

	::FreeLibrary(hModule);
}


}//namespace tools
}//namespace shared

#endif //SHARED_TOOL_H_
