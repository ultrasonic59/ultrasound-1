#pragma once

#include "module/window/window_ex.h"
#include "controls/ui_bitmap_control.h"

using namespace nbase;

typedef enum 
{
	PlayerStateStarting = 1,
	PlayerStatePlaying,
	PlayerStatePaused,
	PlayerStateCompleted,
	PlayerStateStopping,
	PlayerStateStopped,
	PlayerStateError,
} PlayerState;

class MainForm : public nim_comp::WindowEx
{
public:
	MainForm(const std::wstring& wnd_id = L"");
	~MainForm();

	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual ui::Control* CreateControl(const std::wstring& pstrClass) override;
	virtual ui::UILIB_RESOURCETYPE GetResourceType() const;
	virtual std::wstring GetZIPFileName() const;

	virtual std::wstring GetWindowClassName() const override;
	virtual std::wstring GetWindowId() const override;
	virtual UINT GetClassStyle() const override;

	/**
	* 窗口初始化函数
	* @return void	无返回值
	*/
	virtual void InitWindow() override;
	/**
	* 拦截并处理底层窗体消息
	* @param[in] uMsg 消息类型
	* @param[in] wParam 附加参数
	* @param[in] lParam 附加参数
	* @return LRESULT 处理结果
	*/
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	/**
	* 拦截并处理窗口关闭消息
	* @param[in] uMsg 消息类型
	* @param[in] wParam 附加参数
	* @param[in] lParam 附加参数
	* @param[in] bHandled 为true表示父类不需要再处理本消息了，否则反之
	* @return LRESULT 处理结果
	*/
	virtual void Close(UINT nRet = IDOK) override;
	virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
	/**
	* 处理所有控件的所有消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	virtual bool Notify(ui::EventArgs* msg);
	/**
	* 处理所有控件单击消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	virtual bool OnClicked(ui::EventArgs* msg);
	/**
	* 处理所有控件选中消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	virtual bool OnSelected(ui::EventArgs* msg);
	/**
	* 处理ESC键消息
	* @param[in] msg 消息的相关信息
	* @return bool true 继续传递控件消息，false 停止传递控件消息
	*/
	virtual void OnEsc(BOOL &bHandled);
	 /**
	 * 处理窗体最大化消息
	 * @param[in] max 是否最大化
	 * @return void 无返回值
	 */
	void OnMaxWindow(bool max);
	
private:
	/**
	* 文件被选择后的回调函数
	* @param[in] ret		 是否选择了文件
	* @param[in] file_path	 选择的文件的路径
	* @return void 无返回值
	*/
	void OnFileSelected(BOOL ret, std::wstring file_path);
	/**
	* 开始视频播放
	* @return void	无返回值
	*/
	bool StartPlay();
	/**
	* 开始视频播放
	* @return void	无返回值
	*/
	void Shutdown();
	/**
	* 刷新播放时间和进度条
	* @return void	无返回值
	*/
	void RefreshPlayTime();
	/**
	* 更新播放器状态
	* @return void	无返回值
	*/
	void ChangePlayerState(PlayerState new_state);
	/**
	* 播放结果回调
	* @return void	无返回值
	*/
	static void PlayerMessageCB(_HNLPSERVICE hNLPService, ST_NELP_MESSAGE msg);
	static void UIPlayerMessageCB(_HNLPSERVICE hNLPService, ST_NELP_MESSAGE msg);
	/**
	* 视频数据回调
	* @return void	无返回值
	*/
	static void VideoFrameCB(_HNLPSERVICE hNLPService, ST_NELP_FRAME *frame);
	/**
	* 读取配置文件
	* @return void	无返回值
	*/
	void ReadConfigurations();

public:
	static const LPTSTR kClassName;
	static std::map<_HNLPSERVICE, MainForm*> InstFormMap;
	static unsigned int InstanceNum; //播放器窗口个数

private:
	std::wstring       wnd_id_; //当前窗口ID
	_HNLPSERVICE       nelp_handle_ = NULL;
	bool               is_max_window_ = false;
	bool               playing_ = false;
	long long          total_duration_ = 0; //总时长，可用于区分是直播开始点播
	float              volume_ = 0.0f;
	bool               muted_ = false;
	bool               video_rendered_ = false;
	int                playback_timeout_ = -1;
	PlayerState        state_ = PlayerStateStopped;
	WeakCallbackFlag   refresh_time_flag_;

	ui::Combo*         source_type_ = NULL;
	ui::RichEdit*      url_edit_ = NULL;
	ui::Button*        select_file_path_ = NULL;
	ui::VBox*		   volume_box_ = NULL;
	ui::TabBox*		   display_box_ = NULL;
	ui::TabBox*		   play_pause_box_ = NULL;
	ui::BitmapControl* canvas_ = NULL;
	ui::Button*        btn_start_ = NULL;
	ui::Button*        btn_stop_ = NULL;
	ui::Slider*        playing_progress_ = NULL;
	ui::Slider*        volume_slider_ = NULL;
	ui::Label*         label_duration_ = NULL;
	ui::Button*        btn_snapshot_ = NULL;
	ui::Button*        btn_volume_ = NULL;
	ui::Button*        btn_mute_ = NULL;
	ui::Button*        btn_select_file_ = NULL;
};
