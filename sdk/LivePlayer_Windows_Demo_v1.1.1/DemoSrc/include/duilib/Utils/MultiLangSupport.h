#ifndef UI_UTILS_MULTILANGSUPPORT_H_
#define UI_UTILS_MULTILANGSUPPORT_H_


namespace ui 
{

class UILIB_API MutiLanSupport
{
public:
	MutiLanSupport() {};
	virtual ~MutiLanSupport() {};

public:
    static MutiLanSupport* GetInstance();
	std::wstring GetStringViaID(const std::wstring& id);
	bool LoadStringTable(const std::wstring& file_path);

private:
	void ClearAll();
	bool AnalyzeStringTable(const std::vector<std::wstring>& list);

private:	//��Ա    
	std::map<std::wstring,std::wstring>  string_table_;	//�ַ����б�
	std::wstring	new_lang_setting_;//�û��������ù��Ľ�������������Ϣ���˳������Żᱣ�棩
};

}
#endif //UI_UTILS_MULTILANGSUPPORT_H_
