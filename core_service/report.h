#ifndef CORE_SERVICE_REPORT_H_
#define CORE_SERVICE_REPORT_H_

#include <algorithm>
#include <list>
#include <string>

#include "json.h"

namespace core {

enum CONFIRM_STATUS {
	CONFIRM_STATUS_NONE = 0, // �������
	CONFIRM_STATUS_WAIT,     // δ���
	CONFIRM_STATUS_REJECT,   // ���δͨ��
	CONFIRM_STATUS_PASS,     // ���ͨ��
};

class Report {
public:
	bool LoadFromJson(const Json::Value& value);
	void SaveToJson(Json::Value& value, bool save_to_server);

    void New(const std::string& uid);
    void Clear();

    const std::string& GetUid() const { return m_uid; }
    const std::string& GetName() const { return m_name; }
    const std::string& GetSex() const { return m_sex; }
    int GetAge() const { return m_age; }
    const std::string& GetParts() const { return m_parts; }
    const std::string& GetContent() const { return m_content; }
    const std::string& GetConclusion() const { return m_conclusion; }
    const std::string& GetDepartment() const { return m_department; }
    const std::string& GetDialogId() const { return m_dialog_id; }
    const std::string& GetCheckDate() const { return m_check_date; }
    const std::string& GetSuperior() const { return m_superior; }
	const std::string& GetSuperiorId() const { return m_superior_id; }
    const std::string& GetDiaDoctor() const { return m_dia_doctor; }
    bool GetChannelCreated() const { return m_channel_created; }
    std::list<std::string> GetImages() const { return m_images; }
    const std::string& GetPrimary() const { return m_primary; }
	const std::string& GetPrimaryId() const { return m_primary_id; }
    const std::string& GetHospital() const { return m_hospital; }
    const std::string& GetVideo() const { return m_video; }

	const std::string& GetPatientId() const { return m_patient_id; }
	const std::string& GetRudsReason() const { return m_ruds_reason; }
	const std::string& GetRudsTarget() const { return m_ruds_target; }
	const std::string& GetTitle() const { return m_title; }
	CONFIRM_STATUS GetConfirmStatus() const { return m_confirm_status; }
	const std::string& GetConfirmMsg() const { return m_confirm_msg; }
	const std::string& GetReportUrl() const { return m_report_url; }
	bool GetBedsideType() const { return m_bedside_type == 1; }

	int GetHospitalId() const { return m_hospital_id; }
	int GetSuperiorHospitalId() const { return m_hospital_s_id; }
	const std::string& GetSuperiorHospitalName() const { return m_hospital_s_name; }

	bool GetRudsTitleRequest() const { return m_ruds_title_request == 1; }
	const std::string& GetRudsReportTitle() const { return m_ruds_report_title; }
	const std::string& GetRudsReportBedsideTitle() const { return m_ruds_report_bedside_title; }

    void SetName(const std::string& name) { m_name = name; }
    void SetSex(const std::string& sex) { m_sex = sex; }
    void SetAge(int age) { m_age = age; }
    void SetParts(const std::string& parts) { m_parts = parts; }
    void SetContent(const std::string& content) { m_content = content; }
    void SetConclusion(const std::string& conclusion) { m_conclusion = conclusion; }
    void SetDepartment(const std::string& department) { m_department = department; }
    void SetDialogId(const std::string& dialog_id) { m_dialog_id = dialog_id; }
    void SetCheckDate(const std::string& check_date) { m_check_date = check_date; }
    void SetPrimary(const std::string& primary) { m_primary = primary; }
    void SetPrimaryId(const std::string& primary_id) { m_primary_id = primary_id; }
    void SetHospital(const std::string& hospital) { m_hospital = hospital; }
    void SetClouded(bool clouded) { m_clouded = clouded; }

    void SetEndpoint(const std::string& endpoint) { m_endpoint = endpoint; }
    void SetBucket(const std::string& bucket) { m_bucket = bucket; }

    void AddImages(const std::list<std::string>& images) {
        m_images.insert(m_images.end(), images.begin(), images.end());
    }

    void RemoveImage(const std::string& image) {
        m_images.remove(image);
    }

    bool HasImage(const std::string& image) {
        return std::find(m_images.begin(), m_images.end(), image) != m_images.end();
    }

    void ClearImages() { m_images.clear(); };

    void SetChannelCreated(bool channel_created) { m_channel_created = channel_created; }
    void SetChannel(const std::string& channel) { m_channel = channel; }
    void SetChannelTime(int channel_time) { m_channel_time = channel_time; }
    void SetSuperior(const std::string& superior) { m_superior = superior; }
    void SetSuperiorId(const std::string& superior_id) { m_superior_id = superior_id; }

    void SetDiaDoctor(const std::string& dia_doctor) { m_dia_doctor = dia_doctor; }

    void SetHospitalId(int hospital_id) { m_hospital_id = hospital_id; }
	void SetSuperiorHospitalId(int hospital_s_id) { m_hospital_s_id = hospital_s_id; }
	void SetSuperiorHospitalName(const std::string& hospital_s_name) { m_hospital_s_name = hospital_s_name; }

	void SetPatientId(const std::string& patient_id) { m_patient_id = patient_id; }
	void SetRudsReason(const std::string& ruds_reason) { m_ruds_reason = ruds_reason; }
	void SetRudsTarget(const std::string& ruds_target) { m_ruds_target = ruds_target; }
	void SetTitle(const std::string& title) { m_title = title; }
	void SetConfirmStatus(CONFIRM_STATUS confirm_status) { m_confirm_status = confirm_status; }
	void SetConfirmMsg(const std::string& confirm_msg) { m_confirm_msg = confirm_msg; }
	void SetReportUrl(const std::string& report_url) { m_report_url = report_url; }
	void SetBedsideType(bool bedside_type) { m_bedside_type = bedside_type ? 1 : 0; }

	void SetRudsTitleRequest(bool ruds_title_request) { m_ruds_title_request = ruds_title_request ? 1 : 0; }
	void SetRudsReportTitle(const std::string& ruds_report_title) { m_ruds_report_title = ruds_report_title; }
	void SetRudsReportBedsideTitle(const std::string& ruds_report_bedside_title) { m_ruds_report_bedside_title = ruds_report_bedside_title; }

private:
	// ��������ֵ
	std::string m_uid;  // ������ţ�ȫ��Ψһ
	std::string m_name; // ��������
	std::string m_sex;  // �����Ա�
	int m_age;  // ��������
	std::string m_parts;      // ��鲿λ
	std::string m_content;    // �������
	std::string m_conclusion; // ������
	std::string m_department; // �ͼ����
	std::string m_dialog_id;  // ����� / סԺ��
	std::string m_check_date; // �������
	std::string m_primary;    // ����ҽ��
	std::string m_primary_id; // ����ҽ��ID

    std::string m_endpoint;
    std::string m_bucket;
    std::list<std::string> m_images; // �ɼ�ͼ���б�
	std::string m_hospital; // �����ͼ�ҽԺ��������ڱ��������֣���Ҫ�����ڲ�ѯ�б�ĵ���ʾ
	bool m_channel_created = false;   // ͨ���Ƿ񴴽����Ƿ��й�Զ��ͨ��
	bool m_clouded = false;           // �Ƿ����ƣ��Ƿ񿪷Ÿ�����

	// �����ͨ������Щ���Ǳ���ֵ
	std::string m_channel;      // ͨ��ͨ��ID
	int m_channel_time = 0; // ͨ��ͨ����ʱ�������ʱ
	std::string m_superior;     // �ϼ�����ҽ��
	std::string m_superior_id;  // �ϼ�����ҽ��ID

	// ѡ������ֵ
	std::string m_phone;      // ���ߵ绰��û�оͲ������û����Ų�ѯ
	std::string m_bed;        // ����
	std::string m_device;     // ����ID
	std::string m_recorder;   // ��¼Ա����
	std::string m_dia_doctor; // �ͼ�ҽ��

	// �����ͨ������Щ�ɷ��������㸳ֵ
	bool m_isfree = false;        // �Ƿ��շ�  ����ͨ�� / ���������ƥ�䣬�ɷ��������㸳ֵ
	int m_fee;            // ������ã���ЧԶ����ϲŻ�������ɷ��������㸳ֵ
	int m_province_s_id;  // �ϼ�ҽԺ����ʡid����������ͳ�Ʋ�ѯ
	int m_city_s_id;      // �ϼ�ҽԺ������id����������ͳ�Ʋ�ѯ
	int m_county_s_id;    // �ϼ�ҽԺ������id����������ͳ�Ʋ�ѯ
	int m_hospital_s_id;  // �ϼ�ҽԺid����������ͳ�Ʋ�ѯ
	std::string m_hospital_s_name;

	// ����
	bool m_updated = false;   // �����Ƿ񱻸��ģ��ɷ��������㸳ֵ
	bool m_confirmed = false; // �Ƿ���ˣ���ʱû��ʹ��
	std::string m_video;      // Զ��¼������
	std::string m_others;     // ��ע���Ժ���

	// �̳л���ҽԺ���ԣ���������ͳ�Ʋ�ѯ���ɷ��������ӣ����û����ɼ�
	int m_province_id; // ����ҽԺ����ʡid
	int m_city_id;     // ����ҽԺ������id
	int m_county_id;   // ����ҽԺ������id
	int m_hospital_id; // ����ҽԺid����������ͳ�Ʋ�ѯ

	// ���� ***
	std::string m_patient_id; // סԺ�� ***
	std::string m_ruds_reason; // ����ԭ�� ***
	std::string m_ruds_target; // ����Ŀ�� ***
	std::string m_title; // ������⣬ ��ӡӦ���� hospital + title ***
	CONFIRM_STATUS m_confirm_status; // ���״̬ 0 / 1 / 2 / 3  ������� / δ��� / δͨ�� / ͨ�� ***
	std::string m_confirm_msg; // ������ ***
	std::string m_report_url; // Զ�̱���ı����ַ�����ڶ�ά��ʹ��  ***
	int m_bedside_type; // ���Գ�����ʶ 1 / 0 �Ƿ��Գ���

	// �ϼ�ҽ������
	std::string m_ruds_report_title;  // �ϼ�ҽԺͳһ����title
	std::string m_ruds_report_bedside_title; // �ϼ�ҽԺͳһ���Ա���title
	int m_ruds_title_request;   // Զ�̻���ϼ�ҽԺ1 / 0�� / ��ǿ��ʹ���ϼ�ҽԺ��title���¼������� ***
};

} // namespace core

#endif // CORE_SERVICE_REPORT_H_
