#include "msgbox.h"
#include "Resource.h"

using namespace ui;

MsgBox* ShowMsgBox(HWND hwnd, const std::wstring &content, MsgboxCallback cb,
	const std::wstring &title, const std::wstring &yes, const std::wstring &no, const std::wstring& wnd_id)
{
	MsgBox* msgbox = NULL;
	if (!wnd_id.empty())
	{
		auto iter = MsgBox::kMsgBoxList.find(wnd_id);
		if (iter != MsgBox::kMsgBoxList.end())
			msgbox = iter->second;
	}
	if (msgbox == NULL)
	{
		msgbox = new MsgBox(wnd_id);
		HWND hWnd = msgbox->Create(hwnd, L"", WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX, 0);
		if (hWnd == NULL)
		{
			delete msgbox;
			return NULL;
		}
	}
	msgbox->SetTitle(title);
	msgbox->SetContent(content);
	msgbox->SetButton(yes, no);
	msgbox->Show(hwnd, cb);
	return msgbox;
}

void CloseMsgBox(const std::wstring& wnd_id)
{
	if (wnd_id.empty())
		return;
	auto iter = MsgBox::kMsgBoxList.find(wnd_id);
	if (iter != MsgBox::kMsgBoxList.end())
		iter->second->Close();
}

const LPCTSTR MsgBox::kClassName = L"MsgBox";
std::map<std::wstring, MsgBox*> MsgBox::kMsgBoxList;

MsgBox::MsgBox(const std::wstring& wnd_id) : window_id_(wnd_id)
{

}

MsgBox::~MsgBox()
{
}

std::wstring MsgBox::GetSkinFolder()
{
	return L"msgbox";
}

std::wstring MsgBox::GetSkinFile()
{
	return L"msgbox.xml";
}

ui::UILIB_RESOURCETYPE MsgBox::GetResourceType() const
{
	return ui::UILIB_FILE;
}

std::wstring MsgBox::GetZIPFileName() const
{
	return (L"msgbox.zip");
}

std::wstring MsgBox::GetWindowClassName() const
{
	return kClassName;
}

std::wstring MsgBox::GetWindowId() const
{
	return kClassName;
}

UINT MsgBox::GetClassStyle() const
{
	return (UI_CLASSSTYLE_FRAME | CS_DBLCLKS);
}

LRESULT MsgBox::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_DESTROY)
	{
		if (!window_id_.empty())
			MsgBox::kMsgBoxList.erase(window_id_);
	}
	return __super::HandleMessage(uMsg, wParam, lParam);
}

void MsgBox::OnEsc(BOOL &bHandled)
{
	bHandled = TRUE;
	EndMsgBox(MB_NO);
}
void MsgBox::Close(UINT nRet)
{
	// ��ʾ��ر�֮ǰ��Enable�����ڣ���ֹ��������������ȥ��
	HWND hWndParent = GetWindowOwner(m_hWnd);
	if (hWndParent)
	{
		::EnableWindow(hWndParent, TRUE);
		::SetFocus(hWndParent);
	}

	__super::Close(nRet);
}

void MsgBox::InitWindow()
{
	if (!window_id_.empty())
		MsgBox::kMsgBoxList[window_id_] = this; //��ӵ��б�

	SetIcon(IDI_NLS_PLAY);
	::SetWindowTextW(m_hWnd, L"��ʾ");

	m_pRoot->AttachBubbledEvent(ui::kEventClick, nbase::Bind(&MsgBox::OnClicked, this, std::placeholders::_1));

	title_ = (Label*)FindControl(L"title");
	content_ = (RichEdit*)FindControl(L"content");
	btn_yes_ = (Button*)FindControl(L"btn_yes");
	btn_no_ = (Button*)FindControl(L"btn_no");
}

bool MsgBox::OnClicked(ui::EventArgs* msg)
{
	std::wstring name = msg->pSender->GetName();
	if (name == L"btn_yes")
	{
		EndMsgBox(MB_YES);
	}
	else if (name == L"btn_no")
	{
		EndMsgBox(MB_NO);
	}
	else if (name == L"closebtn")
	{
		EndMsgBox(MB_NO);
	}
	return true;
}

void MsgBox::SetTitle(const std::wstring &str)
{
	title_->SetText(str);
}

void MsgBox::SetContent(const std::wstring &str)
{
	content_->SetText(str);
}

void MsgBox::SetButton(const std::wstring &yes, const std::wstring &no)
{
	if (!yes.empty())
	{
		btn_yes_->SetText(yes);
		btn_yes_->SetVisible(true);
		btn_yes_->SetFocus();
	}
	else
	{
		btn_yes_->SetVisible(false);
	}

	if (!no.empty())
	{
		btn_no_->SetText(no);
		btn_no_->SetVisible(true);
		if (yes.empty())
			btn_no_->SetFocus();
	}
	else
	{
		btn_no_->SetVisible(false);
	}
}

void MsgBox::Show(HWND hwnd, MsgboxCallback cb)
{
	msgbox_callback_ = cb;

	::EnableWindow(hwnd, FALSE);
	CenterWindow();
	ShowWindow(true);
	::SetForegroundWindow(m_hWnd);
}

void MsgBox::EndMsgBox(MsgBoxRet ret)
{
	this->Close(0);

	if (msgbox_callback_)
	{
		nbase::ThreadManager::PostTask(kThreadUI, nbase::Bind(msgbox_callback_, ret));
	}
}
