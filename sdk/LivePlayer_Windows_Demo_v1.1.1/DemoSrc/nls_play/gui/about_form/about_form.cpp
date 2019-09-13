#include "about_form.h"

using namespace ui;

const LPCTSTR AboutForm::kClassName = L"AboutForm";

AboutForm::AboutForm()
{

}

AboutForm::~AboutForm()
{

}

std::wstring AboutForm::GetSkinFolder()
{
	return L"about";
}

std::wstring AboutForm::GetSkinFile()
{
	return L"about.xml";
}

std::wstring AboutForm::GetWindowClassName() const
{
	return kClassName;
}

std::wstring AboutForm::GetWindowId() const
{
	return kClassName;
}

UINT AboutForm::GetClassStyle() const
{
	return (UI_CLASSSTYLE_FRAME | CS_DBLCLKS);
}

void AboutForm::InitWindow()
{
	SetTaskbarTitle(L"��������");
	m_pRoot->AttachBubbledEvent(ui::kEventClick, nbase::Bind(&AboutForm::OnClicked, this, std::placeholders::_1));

	Label* version = (Label*) FindControl(L"version");
	{
		int ver = 0;
		std::wstring vf;
		//nim_ui::UserConfig::GetInstance()->GetAppLocalVersion(ver, vf);
		version->SetText(nbase::StringPrintf(L"�汾: %s.%d", vf.c_str(), ver));
	}
}

bool AboutForm::OnClicked( ui::EventArgs* arg )
{
	std::wstring name = arg->pSender->GetName();
	return false;
}