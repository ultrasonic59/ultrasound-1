#ifndef CORE_SERVICE_SUPERIOR_USER_H_
#define CORE_SERVICE_SUPERIOR_USER_H_

#include <list>
#include <string>

#include "json.h"

namespace core {

class SuperiorUser {
public:

	bool LoadFromJson(const Json::Value& value);
	void SaveToJson(Json::Value& value);

	const std::string& GetProvince() const { return m_province; }
	const std::string& GetHospital() const { return m_hospital; }
	int GetHospitalId() const { return m_hospital_id; }
	const std::string& GetUid() const { return m_uid; }
	const std::string& GetDescription() const { return m_description; }
	const std::string& GetTitle() const { return m_title; }
	int GetOnline() const { return m_online; }
	const std::string& GetCountry() const { return m_county; }
	const std::string& GetName() const { return m_name; }
	int GetPrice() const { return m_price; }
	const std::string& GetCity() const { return m_city; }
	const std::string& GetSkill() const { return m_skill; }
	int GetBirth() const { return m_birth; }
	const std::string& GetPhone() const { return m_phone; }
    const std::string& GetSex() const { return m_sex; }
    const std::string& GetId() const { return m_id; }

	const std::string& GetRudsReportTitle() const { return m_ruds_report_title; }
	const std::string& GetRudsReportBedsideTitle() const { return m_ruds_report_bedside_title; }
	bool GetRudsTitleRequest() const { return m_ruds_title_request == 1; }
	bool GetRudsConfirmRequest() const { return  m_ruds_confirm_request == 1; }

private:

    // Proterties form server.
    std::string m_uid; // ȫ��ID
    std::string m_name; // ����
    std::string m_password; // �Ñ��ܴa, ���Ϊ����ʹ�õ绰���룬��role�������ڵ�¼��֤
	int m_hospital_id; // ҽԺid
    std::string m_phone; // �绰
	int m_free; // ��ҽ��ݣ� 0 / ����ִҵ  1 / ҽԺְ��  2 / ����

	int m_birth; // ���գ���
    std::string m_second_hospital; // �������ҽԺְ���������ҿ�����������
    std::string m_skill; // �ó�
    std::string m_description; // ���
    std::string m_sex; // �Ա�
    std::string m_id; // ���֤��
    std::string m_certificate; // ְҵ֤��
    std::string m_title; // ְ��
    std::string m_url_cert; // ֤��url
    std::string m_url_thumb; // ͷ��url
    std::string m_visit_code; // �����룬������ʱ��Ȩ�û�����
    int m_online; // ����״̬�� 0 / ���� 1 / ����  2 / ������, ���Ե�  3 / ��Ϣ�У�����

	int m_price; // �����շ�, ��ʱ��ʹ��
    std::list<int> m_publish; // ������Χ����ʡ����ҽԺid��ɼ���
    std::list<std::string> m_frees; // ����뼯�ϣ������и����ҽԺ���շ�
    std::list<std::string> m_controlled; // �ܿ��û�id����

    // �̳�ҽԺ����
    std::string m_province; // ����ʡ
    std::string m_city; // ������
    std::string m_county; // ������
    std::string m_hospital; // ҽԺ����
	int m_province_id; // ����ʡid
	int m_city_id; // ������id
	int m_county_id; // ������id
	int m_union_id; // ����ҽ����id

	// ���� ***
	std::string m_ruds_report_title;  // ҽԺͳһ����title, ����Ϊ�ա����Ϊ�գ���Ĭ��ָ�����������뵽������ֶ��� ***
	std::string m_ruds_report_bedside_title; // ҽԺͳһ���Ա���title, ����Ϊ�ա����Ϊ�գ���Ĭ��ָ�����������뵽������ֶ��� ***
	int m_ruds_title_request;   // Զ�̻���ϼ�ҽԺ1 / 0�� / ��ǿ��ʹ���Լ���title���¼������� ***
	int m_ruds_confirm_request; // Զ�̻���1 / 0�Ƿ���Ҫ��ˣ��ϼ�ҽԺ���ж��� ***
};

} // namespace core

#endif // CORE_SERVICE_SUPERIOR_USER_H_
