#include "resource.h"
#include "main_form.h"
#include "gui/msgbox/msgbox.h"
#include "gui/ui_menu/ui_menu.h"
#include "shared/modal_wnd/file_dialog_ex.h"
#include "module/window/windows_manager.h"

const LPTSTR MainForm::kClassName = L"MainForm";
unsigned int MainForm::InstanceNum = 0;

using namespace ui;

MainForm::MainForm(const std::wstring& wnd_id) : wnd_id_(wnd_id)
{
	MainForm::InstanceNum++;
}

MainForm::~MainForm()
{

}
ui::Control* MainForm::CreateControl(const std::wstring& pstrClass)
{
	if (pstrClass == _T("BitmapControl"))
	{
		return new ui::BitmapControl();
	}
	return NULL;
}

std::wstring MainForm::GetSkinFolder()
{
	return L"main";
}

std::wstring MainForm::GetSkinFile()
{
	return L"main_form.xml";
}

ui::UILIB_RESOURCETYPE MainForm::GetResourceType() const
{
	return ui::UILIB_RESOURCETYPE::UILIB_FILE;
}

std::wstring MainForm::GetZIPFileName() const
{
	return L"";
}

std::wstring MainForm::GetWindowClassName() const
{
	return kClassName;
}

std::wstring MainForm::GetWindowId() const
{
	if (!wnd_id_.empty())
		return wnd_id_;
	return kClassName;
}

UINT MainForm::GetClassStyle() const
{
	return (UI_CLASSSTYLE_FRAME | CS_DBLCLKS);
}

void MainForm::InitWindow()
{
	SetIcon(IDI_NLS_PLAY);
	SetTaskbarTitle(L"网易视频云-播放器");
	m_pRoot->AttachBubbledEvent(ui::kEventAll, nbase::Bind(&MainForm::Notify, this, std::placeholders::_1));
	m_pRoot->AttachBubbledEvent(ui::kEventClick, nbase::Bind(&MainForm::OnClicked, this, std::placeholders::_1));
	m_pRoot->AttachBubbledEvent(ui::kEventSelect, nbase::Bind(&MainForm::OnSelected, this, std::placeholders::_1));

	source_type_ = (Combo*)FindControl(L"source_type");
	source_type_->SelectItem(1); //默认“在线直播”
	url_edit_ = (ui::RichEdit*)FindControl(L"url");
	select_file_path_ = (ui::Button*)FindControl(L"scan_local_file");
	volume_box_ = (ui::VBox*)FindControl(L"volume_box");
	display_box_ = (ui::TabBox*)FindControl(L"display_box");
	canvas_ = (ui::BitmapControl*)FindControl(L"video_display");
	play_pause_box_ = (ui::TabBox*)FindControl(L"start_or_pause_box");
	btn_start_ = (ui::Button*)FindControl(L"start");
	btn_stop_ = (ui::Button*)FindControl(L"stop");
	playing_progress_ = (ui::Slider*)FindControl(L"playing_progress");
	volume_slider_ = (ui::Slider*)FindControl(L"volume_slider");
	volume_ = (float)(volume_slider_->GetValue() / volume_slider_->GetMaxValue());
	label_duration_ = (ui::Label*)FindControl(L"duration");
	btn_snapshot_ = (ui::Button*)FindControl(L"snapshot");
	btn_volume_ = (ui::Button*)FindControl(L"volume_btn");
	btn_mute_ = (ui::Button*)FindControl(L"volume_mute_btn");
	btn_select_file_ = (ui::Button*)FindControl(L"scan_local_file");

	OnMaxWindow(TRUE == IsZoomed(this->GetHWND()));

	std::wstring url = QCommand::Get(VIDEO_FILE);
	if (!url.empty())
	{
		url_edit_->SetText(url);
		ChangePlayerState(PlayerStateStarting);
		StartPlay();
	}
}

LRESULT MainForm::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_SIZE)
	{
		if (wParam == SIZE_RESTORED)
		{
			if (is_max_window_)
				OnMaxWindow(false);
		}
		else if (wParam == SIZE_MAXIMIZED)
		{
			if (!is_max_window_)
				OnMaxWindow(true);
		}
	}
	return __super::HandleMessage(uMsg, wParam, lParam);
}

void MainForm::Close(UINT nRet)
{
	if (nRet != 100)
	{
		MsgboxCallback cb = ToWeakCallback([this](MsgBoxRet ret)
		{
			if (ret == MB_YES)
				Close(100);
		});
		ShowMsgBox(m_hWnd, L"确定关闭播放器？", cb);
	}
	else
		__super::Close(nRet);
}

LRESULT MainForm::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	auto post_close_cb = ToWeakCallback([this]()
	{
		while (playing_)
			Sleep(100);
		::DestroyWindow(m_hWnd); //确保播放器实例销毁之后再真正关闭窗口
		MainForm::InstanceNum--;
		if (MainForm::InstanceNum == 0) //已经没有直播窗口了
		{
			nim_comp::WindowsManager::GetInstance()->DestroyAllWindows(); //销毁其他窗口
			::PostQuitMessage(0); //退出程序
		}
	});

	ShowWindow(false);
	if (playing_)
	{
		Shutdown();
		nbase::ThreadManager::PostDelayedTask(post_close_cb, nbase::TimeDelta::FromSeconds(1)); //等待shudown完成再关闭窗口
	}
	else
		post_close_cb(); //直接关闭窗口
	bHandled = true;
	return 0;
}

bool MainForm::Notify(ui::EventArgs * msg)
{
	std::wstring name = msg->pSender->GetName();
	if (msg->Type == ui::kEventValueChange)
	{
		if (name == L"playing_progress")
		{
			if (nelp_handle_)
			{
				double frac = playing_progress_->GetValue() / playing_progress_->GetMaxValue();
				long long seek_pos = (long long)(total_duration_ * frac);
				Nelp_SeekTo(nelp_handle_, seek_pos);
			}
		}
		else if (name == L"volume_slider")
		{
			volume_ = (float)(volume_slider_->GetValue() / volume_slider_->GetMaxValue());
			if (nelp_handle_)
				Nelp_SetVolume(nelp_handle_, volume_);
		}
	}
	else if (msg->Type == ui::kEventMouseMenu)
	{
		if (name == L"volume_btn")
		{
			muted_ = true;
			Nelp_SetMute(nelp_handle_, true);
			btn_volume_->SetVisible(false);
			btn_mute_->SetVisible(true);
		}
		else if (name == L"volume_mute_btn")
		{
			muted_ = false;
			Nelp_SetMute(nelp_handle_, false);
			btn_volume_->SetVisible(true);
			btn_mute_->SetVisible(false);
		}
		else if (name == L"url")
		{
			CMenuWnd* pMenu = new CMenuWnd(NULL);
			STRINGorID xml(L"url_edit_menu.xml");
			ui::CPoint pt_mouse;
			GetCursorPos(&pt_mouse);
			pMenu->Init(xml, _T("xml"), pt_mouse);
			pMenu->Show();
			pMenu->SetFocus(NULL);

			CMenuElementUI* menu_cut = (CMenuElementUI*)pMenu->FindControl(L"cut");
			CMenuElementUI* menu_copy = (CMenuElementUI*)pMenu->FindControl(L"copy");
			CMenuElementUI* menu_paste = (CMenuElementUI*)pMenu->FindControl(L"paste");
			auto cb = ToWeakCallback([this](EventArgs* args)
			{
				std::wstring name = args->pSender->GetName();
				if (name == L"cut")
					url_edit_->Cut();
				else if (name == L"copy")
					url_edit_->Copy();
				else if (name == L"paste")
					url_edit_->Paste();
				return true;
			});
			menu_cut->AttachSelect(cb);
			menu_copy->AttachSelect(cb);
			menu_paste->AttachSelect(cb);
		}
	}
	else if (msg->Type == ui::kEventKillFocus)
	{
		if (name == L"volume_slider")
		{
			volume_box_->SetVisible(false);
		}
	}
	else if (msg->Type == ui::kEventTextChange)
	{
		if (name == L"url")
		{
			std::string url = url_edit_->GetUTF8Text();
			if(state_ == PlayerStateStarting || state_ == PlayerStatePlaying || state_ == PlayerStatePaused)
			{
				if (!url.empty())
				{
					auto cb = ToWeakCallback([this, url](MsgBoxRet ret)
					{
						if (ret == MB_YES)
						{
							EN_NELP_BUFFER_STRATEGY stratergy = EN_NELP_TOP_SPEED;
							ST_NELP_PARAM param = { 0 };
							if (source_type_->GetCurSel() == 0)
								stratergy = EN_NELP_TOP_SPEED;
							else if (source_type_->GetCurSel() == 1 || source_type_->GetCurSel() == 2)
								stratergy = EN_NELP_ANTI_JITTER;
							param.paPlayUrl = (char*)url.c_str();
							param.enBufferStrategy = stratergy;
							ChangePlayerState(PlayerStateStopping);
							NELP_RET ret = Nelp_SwitchContentUrl(nelp_handle_, &param);
							ChangePlayerState(PlayerStateStopped);
							ChangePlayerState(PlayerStateStarting);
							if (ret != NELP_OK)
								ChangePlayerState(PlayerStateError);
						}
					});
					ShowMsgBox(NULL, L"检测到播放地址已改变，是否立刻切换到新地址播放？", cb, L"提示", L"切换", L"取消", L"switch_content_url");
				}
			}
			else
				btn_start_->SetEnabled(!url.empty());
		}
	}
	return true;
}

int SaveRGB2Bmp(const wchar_t * pFilePath, int iWidth, int iHeight, int iPixelSize, bool mirror, unsigned char *pBuffer)
{
	BITMAPINFOHEADER bih;
	int widthStep = iWidth * iPixelSize;
	int DIBSize = widthStep * iHeight;  //buffer的大小 （字节为单位）  
	bih.biSize = sizeof(BITMAPINFOHEADER); // header size  
	bih.biWidth = iWidth;
	bih.biHeight = mirror ? -iHeight : iHeight;
	bih.biPlanes = 1;
	bih.biBitCount = iPixelSize * 8;
	bih.biCompression = BI_RGB; // no compression 非压缩  
	bih.biSizeImage = DIBSize;
	bih.biXPelsPerMeter = 0;
	bih.biYPelsPerMeter = 0;
	bih.biClrUsed = 0;
	bih.biClrImportant = 0;

	BITMAPFILEHEADER bhh;
	bhh.bfType = ((WORD)('M' << 8) | 'B');  //'BM'  
	bhh.bfSize = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + DIBSize;
	bhh.bfReserved1 = 0;
	bhh.bfReserved2 = 0;
	bhh.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);

	FILE *file;
	_wfopen_s(&file, pFilePath, L"wb");
	if (file)
	{//写入文件  
		fwrite((LPSTR)&bhh, sizeof(BITMAPFILEHEADER), 1, file);
		fwrite((LPSTR)&bih, sizeof(BITMAPINFOHEADER), 1, file);
		fwrite(pBuffer, sizeof(char), DIBSize, file);
		fclose(file);
		return 1;
	}
	return 0;
}

bool MainForm::OnClicked(ui::EventArgs * msg)
{
	std::wstring name = msg->pSender->GetName();
	if (name == L"scan_local_file")
	{
		std::map<LPCTSTR, LPCTSTR> filters;
		std::wstring file_type = L"视频文件(*.avi;*.flv;*.mkv;*.mp4;*.rmvb)";
		std::wstring filter = L"*.avi;*.flv;*.mkv;*.mp4;*.rmvb";
		filters[file_type.c_str()] = filter.c_str();
		std::wstring file_type2 = L"音频文件(*.ape;*.flac;*.mp3;*.wav;*.wma)";
		std::wstring filter2 = L"*.ape;*.flac;*.mp3;*.wav;*.wma";
		filters[file_type2.c_str()] = filter2.c_str();
		CFileDialogEx::FileDialogCallback2 cb = nbase::Bind(&MainForm::OnFileSelected, this, std::placeholders::_1, std::placeholders::_2);
		CFileDialogEx* file_dlg = new CFileDialogEx();
		file_dlg->SetFilter(filters);
		file_dlg->SetParentWnd(m_hWnd);
		file_dlg->AsyncShowOpenFileDlg(cb);
	}
	else if (name == L"volume_btn" || name == L"volume_mute_btn")
	{
		UiRect rect = msg->pSender->GetPos(true);
		UiRect rect_box = UiRect(rect.left - 5, 0, 0, 55);
		volume_box_->SetMargin(rect_box);
		volume_box_->SetVisible(true);
		volume_box_->FindSubControl(L"volume_slider")->SetFocus();
	}
	else if (name == L"snapshot")
	{
		if (nelp_handle_ && playing_)
		{
			ST_NELP_PICTURE* picture = Nelp_GetSnapshot(nelp_handle_, EN_PIC_ARGB8888);
			if (picture)
			{
				ST_NELP_PICTURE picture_copy = *picture;
				int size = picture->iWidth * picture->iHeight * 4;
				picture_copy.puaUsrData = (unsigned char*)malloc(size);
				memcpy(picture_copy.puaUsrData, picture->puaUsrData, size);
				nbase::Time::TimeStruct now = nbase::Time::Now().ToTimeStruct(true);
				std::wstring picture_name = nbase::StringPrintf(L"snapshot_%04d%02d%02d_%02d%02d%02d.bmp", 
					now.year(), now.month(), now.day_of_month(), now.hour(), now.minute(), now.second());
				nbase::ThreadManager::PostTask(kThreadGlobalMisc, ToWeakCallback([this, picture_copy, picture_name]()
				{
					std::wstring path = CFileDialogEx::AsyncShowChooseDirDlg(m_hWnd, L"请选择截图文件的保存位置：");
					if (path.empty() || !nbase::FilePathIsExist(path, true))
					{
						free(picture_copy.puaUsrData);
						return;
					}
					if (path.at(path.length() - 1) != L'\\' || path.at(path.length() - 1) != L'/')
						path.push_back(L'\\');
					path += picture_name;
					SaveRGB2Bmp(path.c_str(), picture_copy.iWidth, picture_copy.iHeight, 4, true, picture_copy.puaUsrData);
					free(picture_copy.puaUsrData);
				}));
			}
		}
	}
	else if (name == L"start" || name == L"refresh")
	{
		ChangePlayerState(PlayerStateStarting);
		if (!StartPlay()) //这里只处理失败情况，收到NELP_MSG_PREPARED消息才算播放成功
			ChangePlayerState(PlayerStateError);
	}
	else if (name == L"pause")
	{
		if (playing_)
		{
			Nelp_Pause(nelp_handle_);
			ChangePlayerState(PlayerStatePaused);
		}
	}
	else if (name == L"resume")
	{
		if (playing_)
		{
			Nelp_Resume(nelp_handle_);
			ChangePlayerState(PlayerStatePlaying);
		}
	}
	else if (name == L"stop")
	{
		ChangePlayerState(PlayerStateStopping);
		Shutdown();
		ChangePlayerState(PlayerStateStopped);
	}
	return false;
}

bool MainForm::OnSelected(ui::EventArgs * msg)
{
	std::wstring name = msg->pSender->GetName();
	if (name == L"source_type")
	{
		int cur_sel = source_type_->GetCurSel();
		if (cur_sel == 0)
		{
			url_edit_->SetPromptText(L"请输入拉流地址");
			select_file_path_->SetVisible(false);
		}
		else if (cur_sel == 1)
		{
			url_edit_->SetPromptText(L"请输入点播地址");
			select_file_path_->SetVisible(false);
		}
		else if (cur_sel == 2)
		{
			url_edit_->SetPromptText(L"请输入本地文件路径");
			select_file_path_->SetVisible(true);
		}
	}
	return true;
}

void MainForm::OnEsc(BOOL &bHandled)
{
	Close();
}

void MainForm::OnMaxWindow(bool _max)
{
	if (is_max_window_ == _max)
		return;
	is_max_window_ = _max;
	if (_max)
	{
		FindControl(L"maxbtn")->SetVisible(false);
		FindControl(L"restorebtn")->SetVisible(true);
	}
	else
	{
		FindControl(L"maxbtn")->SetVisible(true);
		FindControl(L"restorebtn")->SetVisible(false);
	}
}

void MainForm::OnFileSelected(BOOL ret, std::wstring file_path)
{
	if (ret)
		url_edit_->SetText(file_path);
}

void MainForm::ChangePlayerState(PlayerState new_state)
{
	state_ = new_state;
	switch (new_state)
	{
	case PlayerStateStarting:
	{
		display_box_->SelectItem(L"loading"); //视频展示区转圈
		play_pause_box_->SelectItem(L"start"); //显示开始按钮
		btn_start_->SetEnabled(false); //disable开始按钮
		btn_stop_->SetEnabled(true); //enable停止按钮
		//source_type_->SetEnabled(false); //源类型不能修改
		//url_edit_->SetReadOnly(true); //播放地址不能修改
		//btn_select_file_->SetEnabled(false); //disable选择文件按钮
		break;
	}
	case PlayerStatePlaying: //Nelp_Start成功或Nelp_Resume之后
	{
		if (!video_rendered_)
			display_box_->SelectItem(L"logo"); //还没有收到视频帧，先显示logo
		if (source_type_->GetCurSel() > 0)
			play_pause_box_->SelectItem(L"pause"); //点播显示暂停按钮
		else
			play_pause_box_->SelectItem(L"start"); //直播显示开始按钮
		btn_start_->SetEnabled(false); //disable开始按钮
		btn_stop_->SetEnabled(true); //enable停止按钮
		//source_type_->SetEnabled(false); //源类型不能修改
		//url_edit_->SetReadOnly(true); //播放地址不能修改
		//btn_select_file_->SetEnabled(false); //disable选择文件按钮
		if (source_type_->GetCurSel() > 0) //是点播，显示进度条
		{
			playing_progress_->SetEnabled(true);
			playing_progress_->SetVisible(true);
		}
		else
			playing_progress_->SetVisible(false);
		if (!refresh_time_flag_.HasUsed())
		{
			auto refresh_time_cb = refresh_time_flag_.ToWeakCallback([this]()
			{
				RefreshPlayTime(); //开始刷新播放时间
			});
			nbase::ThreadManager::PostRepeatedTask(refresh_time_cb, nbase::TimeDelta::FromMilliseconds(500));
		}
		break;
	}
	case PlayerStatePaused:
	{
		play_pause_box_->SelectItem(L"resume"); //显示继续播放按钮
		btn_start_->SetEnabled(false); //disable开始按钮
		btn_stop_->SetEnabled(true); //enable停止按钮
		//source_type_->SetEnabled(false); //源类型不能修改
		//url_edit_->SetReadOnly(true); //播放地址不能修改
		//btn_select_file_->SetEnabled(false); //disable选择文件按钮
		break;
	}
	case PlayerStateStopping:
	{
		play_pause_box_->SelectItem(L"start"); //显示开始按钮
		btn_start_->SetEnabled(false); //disable开始按钮
		btn_stop_->SetEnabled(false); //disable停止按钮
		//source_type_->SetEnabled(false); //源类型不能修改
		//url_edit_->SetReadOnly(true); //播放地址不能修改
		//btn_select_file_->SetEnabled(false); //disable选择文件按钮
		btn_snapshot_->SetEnabled(false); //禁用截图按钮
		break;
	}
	case PlayerStateCompleted:
	case PlayerStateStopped:
	case PlayerStateError:
	{
		if (new_state == PlayerStateStopped)
			display_box_->SelectItem(L"logo"); //视频展示区显示logo
		else if (new_state == PlayerStateError)
			display_box_->SelectItem(L"refresh_box"); //视频展示区显示刷新按钮
		play_pause_box_->SelectItem(L"start"); //显示开始按钮
		btn_start_->SetEnabled(!url_edit_->GetText().empty()); //enable开始按钮
		btn_stop_->SetEnabled(false); //disable停止按钮
		//source_type_->SetEnabled(true); //源类型可修改
		//url_edit_->SetReadOnly(false); //播放地址可修改
		//btn_select_file_->SetEnabled(true); //enable选择文件按钮
		refresh_time_flag_.Cancel(); //取消刷新时间任务
		playing_progress_->SetEnabled(false);
		if (new_state != PlayerStateCompleted)
		{
			playing_progress_->SetVisible(false); //隐藏进度条
			playing_progress_->SetValue(0);
			label_duration_->SetText(L""); //重置播放时间
			canvas_->Clear();
		}
		video_rendered_ = false;
		btn_snapshot_->SetEnabled(false); //禁用截图按钮
		break;
	}
	}
}

