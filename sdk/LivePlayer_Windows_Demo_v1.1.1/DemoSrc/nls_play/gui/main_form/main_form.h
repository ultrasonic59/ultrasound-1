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
	* ���ڳ�ʼ������
	* @return void	�޷���ֵ
	*/
	virtual void InitWindow() override;
	/**
	* ���ز�����ײ㴰����Ϣ
	* @param[in] uMsg ��Ϣ����
	* @param[in] wParam ���Ӳ���
	* @param[in] lParam ���Ӳ���
	* @return LRESULT ������
	*/
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	/**
	* ���ز������ڹر���Ϣ
	* @param[in] uMsg ��Ϣ����
	* @param[in] wParam ���Ӳ���
	* @param[in] lParam ���Ӳ���
	* @param[in] bHandled Ϊtrue��ʾ���಻��Ҫ�ٴ�����Ϣ�ˣ�����֮
	* @return LRESULT ������
	*/
	virtual void Close(UINT nRet = IDOK) override;
	virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
	/**
	* �������пؼ���������Ϣ
	* @param[in] msg ��Ϣ�������Ϣ
	* @return bool true �������ݿؼ���Ϣ��false ֹͣ���ݿؼ���Ϣ
	*/
	virtual bool Notify(ui::EventArgs* msg);
	/**
	* �������пؼ�������Ϣ
	* @param[in] msg ��Ϣ�������Ϣ
	* @return bool true �������ݿؼ���Ϣ��false ֹͣ���ݿؼ���Ϣ
	*/
	virtual bool OnClicked(ui::EventArgs* msg);
	/**
	* �������пؼ�ѡ����Ϣ
	* @param[in] msg ��Ϣ�������Ϣ
	* @return bool true �������ݿؼ���Ϣ��false ֹͣ���ݿؼ���Ϣ
	*/
	virtual bool OnSelected(ui::EventArgs* msg);
	/**
	* ����ESC����Ϣ
	* @param[in] msg ��Ϣ�������Ϣ
	* @return bool true �������ݿؼ���Ϣ��false ֹͣ���ݿؼ���Ϣ
	*/
	virtual void OnEsc(BOOL &bHandled);
	 /**
	 * �����������Ϣ
	 * @param[in] max �Ƿ����
	 * @return void �޷���ֵ
	 */
	void OnMaxWindow(bool max);
	
private:
	/**
	* �ļ���ѡ���Ļص�����
	* @param[in] ret		 �Ƿ�ѡ�����ļ�
	* @param[in] file_path	 ѡ����ļ���·��
	* @return void �޷���ֵ
	*/
	void OnFileSelected(BOOL ret, std::wstring file_path);
	/**
	* ��ʼ��Ƶ����
	* @return void	�޷���ֵ
	*/
	bool StartPlay();
	/**
	* ��ʼ��Ƶ����
	* @return void	�޷���ֵ
	*/
	void Shutdown();
	/**
	* ˢ�²���ʱ��ͽ�����
	* @return void	�޷���ֵ
	*/
	void RefreshPlayTime();
	/**
	* ���²�����״̬
	* @return void	�޷���ֵ
	*/
	void ChangePlayerState(PlayerState new_state);
	/**
	* ���Ž���ص�
	* @return void	�޷���ֵ
	*/
	static void PlayerMessageCB(_HNLPSERVICE hNLPService, ST_NELP_MESSAGE msg);
	static void UIPlayerMessageCB(_HNLPSERVICE hNLPService, ST_NELP_MESSAGE msg);
	/**
	* ��Ƶ���ݻص�
	* @return void	�޷���ֵ
	*/
	static void VideoFrameCB(_HNLPSERVICE hNLPService, ST_NELP_FRAME *frame);
	/**
	* ��ȡ�����ļ�
	* @return void	�޷���ֵ
	*/
	void ReadConfigurations();

public:
	static const LPTSTR kClassName;
	static std::map<_HNLPSERVICE, MainForm*> InstFormMap;
	static unsigned int InstanceNum; //���������ڸ���

private:
	std::wstring       wnd_id_; //��ǰ����ID
	_HNLPSERVICE       nelp_handle_ = NULL;
	bool               is_max_window_ = false;
	bool               playing_ = false;
	long long          total_duration_ = 0; //��ʱ����������������ֱ����ʼ�㲥
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
