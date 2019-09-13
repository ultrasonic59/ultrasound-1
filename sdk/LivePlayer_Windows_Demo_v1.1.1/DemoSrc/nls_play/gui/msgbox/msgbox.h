#pragma once

enum MsgBoxRet
{
	MB_YES,
	MB_NO
};

typedef std::function<void(MsgBoxRet)> MsgboxCallback;

class MsgBox : public ui::WindowImplBase//WindowEx
{
public:
	friend MsgBox* ShowMsgBox(HWND hwnd, const std::wstring &content, MsgboxCallback cb,
		const std::wstring &title = L"��ʾ", const std::wstring &yes = L"ȷ��", const std::wstring &no = L"ȡ��", const std::wstring& wnd_id = L"");
	friend void CloseMsgBox(const std::wstring& wnd_id);

public:
	MsgBox(const std::wstring& wnd_id);
	virtual ~MsgBox();

	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual ui::UILIB_RESOURCETYPE GetResourceType() const;
	virtual std::wstring GetZIPFileName() const;
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	virtual void OnEsc(BOOL &bHandled);
	virtual void Close(UINT nRet = IDOK) override;

	virtual std::wstring GetWindowClassName() const override;
	virtual std::wstring GetWindowId() const /*override*/;
	virtual UINT GetClassStyle() const override;
	virtual void InitWindow() override;

private:
	bool OnClicked(ui::EventArgs* msg);
	void SetTitle(const std::wstring &str);
	void SetContent(const std::wstring &str);
	void SetButton(const std::wstring &yes, const std::wstring &no);
	void Show(HWND hwnd, MsgboxCallback cb);
	void EndMsgBox(MsgBoxRet ret);

public:
	static const LPCTSTR kClassName;
	static std::map<std::wstring, MsgBox*> kMsgBoxList;

private:
	ui::Label*		title_;
	ui::RichEdit*	content_;
	ui::Button*		btn_yes_;
	ui::Button*		btn_no_;

	MsgboxCallback	msgbox_callback_;
	std::wstring	window_id_;
};
