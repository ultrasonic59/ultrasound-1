#include "main.h"
#include "resource.h"
#include "app_dump.h"
#include "base/util/at_exit.h"
#include "shared/modal_wnd/async_do_modal.h"
#include "shared/tool.h"
#include "gui/main_form/main_form.h"
#include "gui/msgbox/msgbox.h"
#include "module/window/windows_manager.h"

//#if defined(VLD) && (defined(DEBUG) || defined(_DEBUG))
//#include <vld.h>
//#pragma comment(lib,"vld.lib")
//#endif

using namespace ui;

void _InitLog()
{
	QLogImpl::GetInstance()->SetLogLevel(LV_APP);
	std::wstring dir = QPath::GetLocalAppDataDir(APP_DATA_DIR);
	QLogImpl::GetInstance()->SetLogFile(dir + L"nls_play.log");

	const long m2 = 2 * 1024 * 1024, m1 = 1024 * 1024;
	QLogImpl::GetInstance()->HalfTo(m2, m1);

	StdClosure task = nbase::Bind(&_InitLog);
	nbase::ThreadManager::PostDelayedTask(kThreadGlobalMisc, task, nbase::TimeDelta::FromMinutes(10));
}

void MainThread::Init()
{
	nbase::ThreadManager::RegisterThread(kThreadUI);
	PreMessageLoop();

	_InitLog();

	std::wstring theme_dir = QPath::GetAppPath();
	ui::GlobalManager::Startup(theme_dir + L"themes\\default", ui::CreateControlCallback());

	std::wstring app_crash = QCommand::Get(CMD_APP_CRASH);
	if (app_crash.empty())
	{
		int num = _wtoi(QCommand::Get(L"instance_num").c_str());
		if (num == 0 || num == 1)
			nim_comp::WindowsManager::SingletonShow<MainForm>(MainForm::kClassName, true, false);
		else if (num <= 4)
		{
			auto CenterWindowEx = ToWeakCallback([](Window* wnd, int num, int total)
			{
				if (num <= 0 || total <= 0 || num > total)
					return;

				HWND m_hWnd = wnd->GetHWND();
				UiRect rcDlg;
				::GetWindowRect(m_hWnd, &rcDlg);
				UiRect rcArea;
				UiRect rcCenter;
				HWND hWnd = m_hWnd;
				HWND hWndCenter = ::GetWindowOwner(m_hWnd);
				if (hWndCenter != NULL)
					hWnd = hWndCenter;

				// 处理多显示器模式下屏幕居中
				MONITORINFO oMonitor = {};
				oMonitor.cbSize = sizeof(oMonitor);
				::GetMonitorInfo(::MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST), &oMonitor);
				rcArea = oMonitor.rcWork;

				if (hWndCenter == NULL)
					rcCenter = rcArea;
				else if (::IsIconic(hWndCenter))
					rcCenter = rcArea;
				else
					::GetWindowRect(hWndCenter, &rcCenter);

				int DlgWidth = rcDlg.right - rcDlg.left;
				int DlgHeight = rcDlg.bottom - rcDlg.top;

				// Find dialog's upper left based on rcCenter
				int xLeft = (rcCenter.left + rcCenter.right) / 2 - DlgWidth / 2;
				int yTop = (rcCenter.top + rcCenter.bottom) / 2 - DlgHeight / 2;
				int deficit = num - (total + 1) / 2;
				xLeft += deficit * 50;
				yTop += deficit * 50;

				// The dialog is outside the screen, move it inside
				if (xLeft < rcArea.left) xLeft = rcArea.left;
				else if (xLeft + DlgWidth > rcArea.right) xLeft = rcArea.right - DlgWidth;
				if (yTop < rcArea.top) yTop = rcArea.top;
				else if (yTop + DlgHeight > rcArea.bottom) yTop = rcArea.bottom - DlgHeight;

				::SetWindowPos(m_hWnd, NULL, xLeft, yTop, -1, -1, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
			});
			auto CreateWindowTask = ToWeakCallback([this, num, CenterWindowEx](int id)
			{
				std::wstring wnd_id = nbase::Int64ToString16(id);
				MainForm *window = (MainForm*)(nim_comp::WindowsManager::GetInstance()->GetWindow(MainForm::kClassName, wnd_id));
				if (window == NULL)
				{
					window = new MainForm(wnd_id);
					window->Create(NULL, MainForm::kClassName, WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX, 0, false);
					CenterWindowEx(window, id, num);
					window->ShowWindow();
				}
			});
			for (int i = 0; i < num; i++)
				nbase::ThreadManager::PostDelayedTask(nbase::Bind(CreateWindowTask, i + 1), nbase::TimeDelta::FromSeconds(i));
		}
		else
		{
			::MessageBox(NULL, L"最多允许同时开4个播放器", _T("nls play"), MB_OK | MB_ICONERROR);
			::PostQuitMessage(0);
		}
	}
	else
	{
		MsgboxCallback cb = [](MsgBoxRet) 
		{
			::PostQuitMessage(0);
		};
		ShowMsgBox(NULL, L"程序崩溃了", cb, L"提示", L"确定", L"");
	}
}

void MainThread::Cleanup()
{
	ui::GlobalManager::Shutdown();

	PostMessageLoop();
	SetThreadWasQuitProperly(true);
	nbase::ThreadManager::UnregisterThread();
}

void MainThread::PreMessageLoop()
{
	misc_thread_.reset( new MiscThread(kThreadGlobalMisc, "Global Misc Thread") );
	misc_thread_->Start();

	play_thread_.reset(new MiscThread(kThreadPlaying, "Playing Thread"));
	play_thread_->Start();
}

void MainThread::PostMessageLoop()
{
	play_thread_->Stop();
	play_thread_.reset(NULL);

	misc_thread_->Stop();
	misc_thread_.reset(NULL);
}

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPWSTR lpszCmdLine, int nCmdShow)
{
	nbase::AtExitManager at_manager;

	_wsetlocale(LC_ALL, L"chs");

#ifdef _DEBUG
	AllocConsole();
	FILE* fp = NULL;
	freopen_s(&fp, "CONOUT$", "w+t", stdout);
	wprintf_s(L"Command:\n%s\n\n", lpszCmdLine);
#endif

	srand( (unsigned int) time(NULL) );

	::SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);

	std::wstring cmd_data = lpszCmdLine;
	QCommand::ParseCommand(lpszCmdLine);

	HRESULT hr = ::OleInitialize(NULL);
	if( FAILED(hr) )
		return 0;

	//将live_player路径添加到系统path环境变量，则sdk的所有dll都可以正常加载了
	std::wstring sdk_path = QPath::GetAppPath() + L"live_player\\";
	if (!shared::tools::AddPathToEnvironmentVariable(sdk_path))
	{
		MessageBox(NULL, L"未找到sdk目录", L"提示", MB_OK);
		return 0;
	}

	QLOG_APP(L"-----------------start cmd:{0}") << cmd_data;
	{
		MainThread thread; // 创建主线程
		thread.RunOnCurrentThreadWithLoop(nbase::MessageLoop::kUIMessageLoop); // 执行主线程循环
	}
	QLOG_APP(L"exit ui loop");

	CancelAllAsyncModalDialogs();

	QLOG_APP(L"app exit----------------");

#ifdef _DEBUG
	FreeConsole();
	fclose(fp);
#endif

	::OleUninitialize();

	return 0;
}


MiscThread::MiscThread(EnThreadId thread_id, const char* name)
	: FrameworkThread(name), thread_id_(thread_id)
{
}

MiscThread::~MiscThread(void)
{
}

void MiscThread::Init()
{
	nbase::ThreadManager::RegisterThread(thread_id_);
}

void MiscThread::Cleanup()
{
	nbase::ThreadManager::UnregisterThread();
	QLOG_APP(L"MiscThread Cleanup");
}


