#include "resource.h"
#include "main_form.h"
#include "gui/toast/toast.h"

using namespace ui;

std::map<_HNLPSERVICE, MainForm*> MainForm::InstFormMap;

bool MainForm::StartPlay()
{
	if (nelp_handle_)
		Shutdown();

	NELP_RET ret = NELP_OK;
	std::wstring log_path = QPath::GetLocalAppDataDir(APP_DATA_DIR);
	ret = Nelp_Create(nbase::UTF16ToUTF8(log_path).c_str(), &nelp_handle_);
	if (ret != NELP_OK)
		return false;
	InstFormMap[nelp_handle_] = this;

	EN_NELP_BUFFER_STRATEGY stratergy = EN_NELP_TOP_SPEED;
	ST_NELP_PARAM param = {0};
	if (source_type_->GetCurSel() == 0)
		stratergy = EN_NELP_TOP_SPEED;
	else if (source_type_->GetCurSel() == 1 || source_type_->GetCurSel() == 2)
		stratergy = EN_NELP_ANTI_JITTER;
	std::string url = url_edit_->GetUTF8Text();
	param.paPlayUrl = (char*)url.c_str();
	param.enBufferStrategy = stratergy;
	ret = Nelp_InitParam(nelp_handle_, &param);
	if (ret != NELP_OK)
		goto fail;
	Nelp_RegisterMessageCB(nelp_handle_, MainForm::PlayerMessageCB);
	Nelp_RegisterGetVideoFrameCB(nelp_handle_, EN_ARGB8888, MainForm::VideoFrameCB);
	Nelp_SetVolume(nelp_handle_, volume_);
	Nelp_SetMute(nelp_handle_, muted_);
	ReadConfigurations();
	Nelp_SetPlaybackTimeout(nelp_handle_, playback_timeout_);
	ret = Nelp_PrepareToPlay(nelp_handle_);
	if (ret != NELP_OK)
		goto fail;
	return true;

fail:
	Shutdown();
	return false;
}


void MainForm::Shutdown()
{
	Nelp_Stop(nelp_handle_);
	Nelp_Shutdown(nelp_handle_);
	InstFormMap.erase(nelp_handle_);
	nelp_handle_ = NULL;
	playing_ = false;
}

void FormatTime(long long seconds_in, int &hour, int &minute, int &second)
{
	second = seconds_in % 60;
	seconds_in /= 60;
	minute = seconds_in % 60;
	seconds_in /= 60;
	hour = seconds_in % 60;
}

void MainForm::RefreshPlayTime()
{
	long long current_duration = Nelp_GetCurrentPlaybackTime(nelp_handle_);
	int cur_h = 0, cur_m = 0, cur_s = 0;
	FormatTime(current_duration, cur_h, cur_m, cur_s);
	if (source_type_->GetCurSel() > 0 && total_duration_ > 0)
	{
		int max_value = playing_progress_->GetMaxValue();
		playing_progress_->SetValue(current_duration * 1.0 / total_duration_ * max_value);
		int total_h = 0, total_m = 0, total_s = 0;
		FormatTime(total_duration_, total_h, total_m, total_s);
		label_duration_->SetText(nbase::StringPrintf(L"%02d:%02d:%02d / %02d:%02d:%02d", cur_h, cur_m, cur_s, total_h, total_m, total_s));
	}
	else
		label_duration_->SetText(nbase::StringPrintf(L"%02d:%02d:%02d", cur_h, cur_m, cur_s));
}

void MainForm::PlayerMessageCB(_HNLPSERVICE hNLPService, ST_NELP_MESSAGE msg)
{
	nbase::ThreadManager::PostTask(kThreadUI, nbase::Bind(&MainForm::UIPlayerMessageCB, hNLPService, msg)); //抛到UI线程执行
}

void MainForm::UIPlayerMessageCB(_HNLPSERVICE hNLPService, ST_NELP_MESSAGE msg)
{
	auto iter = InstFormMap.find(hNLPService);
	if (iter == InstFormMap.end())
		return;

	NELP_RET ret = NELP_OK;
	MainForm* main_form = iter->second;
	switch (msg.iWhat)
	{
	case NELP_MSG_ERROR: //开始播放失败或者播放过程中出错
		main_form->Shutdown();
		main_form->ChangePlayerState(PlayerStateError);
		break;
	case NELP_MSG_PREPARED: //预处理完成，才执行Nelp_Start
		if (Nelp_Start(main_form->nelp_handle_) == NELP_OK) //开始播放成功
		{
			main_form->playing_ = true; //修改标志
			main_form->total_duration_ = Nelp_GetDuration(main_form->nelp_handle_); //获取总时长
			main_form->ChangePlayerState(PlayerStatePlaying);
		}
		break;
	case NELP_MSG_COMPLETED:
		main_form->Shutdown();
		nim_comp::Toast::ShowToast(L"播放完成", 3000, main_form->m_hWnd);
		main_form->ChangePlayerState(PlayerStateCompleted);
		break;
	case NELP_MSG_VIDEO_SIZE_CHANGED:
	{
		std::wstring tip = nbase::StringPrintf(L"视频尺寸变化：%dx%d", msg.iArg1, msg.iArg2);
		nim_comp::Toast::ShowToast(tip, 3000, main_form->m_hWnd);
		break;
	}
	case NELP_MSG_VIDEO_RENDERING_START:
		main_form->btn_snapshot_->SetEnabled(true);
		main_form->video_rendered_ = true;
		main_form->display_box_->SelectItem(L"video_display"); //有视频帧过来了，显示视频画面
		break;
	case NELP_MSG_AUDIO_RENDERING_START:
		break;
	case NELP_MSG_BUFFERING_START:
		break;
	case NELP_MSG_BUFFERING_END:
		break;
	case NELP_MSG_SEEK_COMPLETED:
		break;
	case NELP_MSG_PLAYBACK_STATE_CHANGED:
		break;
	case NELP_MSG_AUDIO_DEVICE_OPEN_FAILED:
		break;
	case NELP_MSG_VIDEO_PARSE_ERROR:
		break;
	default:
		break;
	}
}

//VideoFrameCB是在主线程执行的
void MainForm::VideoFrameCB(_HNLPSERVICE hNLPService, ST_NELP_FRAME *frame)
{
	auto iter = InstFormMap.find(hNLPService);
	if (iter == InstFormMap.end())
		return;

	MainForm* main_form = iter->second;
	if (frame && frame->enMFormat == EN_ARGB8888)
		main_form->canvas_->Update((char*)frame->puaUsrData, frame->iWidth, frame->iHeight);
}

void MainForm::ReadConfigurations()
{
	const wchar_t* category = L"Configurations";
	std::wstring file_name = nbase::win32::GetCurrentModuleDirectory() + L"configurations.ini";
	playback_timeout_ = GetPrivateProfileInt(category, L"pull_timeout", playback_timeout_, file_name.c_str());
}
