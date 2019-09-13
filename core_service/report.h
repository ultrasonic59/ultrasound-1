#ifndef CORE_SERVICE_REPORT_H_
#define CORE_SERVICE_REPORT_H_

#include <algorithm>
#include <list>
#include <string>

#include "json.h"

namespace core {

enum CONFIRM_STATUS {
	CONFIRM_STATUS_NONE = 0, // 无需审核
	CONFIRM_STATUS_WAIT,     // 未审核
	CONFIRM_STATUS_REJECT,   // 审核未通过
	CONFIRM_STATUS_PASS,     // 审核通过
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
	// 必须输入值
	std::string m_uid;  // 超声编号，全局唯一
	std::string m_name; // 患者姓名
	std::string m_sex;  // 患者性别
	int m_age;  // 患者年龄
	std::string m_parts;      // 检查部位
	std::string m_content;    // 检查描述
	std::string m_conclusion; // 检查结论
	std::string m_department; // 送检科室
	std::string m_dialog_id;  // 门诊号 / 住院号
	std::string m_check_date; // 检查日期
	std::string m_primary;    // 基层医生
	std::string m_primary_id; // 基层医生ID

    std::string m_endpoint;
    std::string m_bucket;
    std::list<std::string> m_images; // 采集图像列表
	std::string m_hospital; // 基层送检医院，这个不在报告里体现，主要是用于查询列表的的显示
	bool m_channel_created = false;   // 通道是否创建，是否有过远程通话
	bool m_clouded = false;           // 是否上云，是否开放给患者

	// 如果有通道，这些就是必填值
	std::string m_channel;      // 通话通道ID
	int m_channel_time = 0; // 通话通道耗时，用秒计时
	std::string m_superior;     // 上级会诊医生
	std::string m_superior_id;  // 上级会诊医生ID

	// 选填输入值
	std::string m_phone;      // 患者电话，没有就不能向用户开放查询
	std::string m_bed;        // 床号
	std::string m_device;     // 仪器ID
	std::string m_recorder;   // 记录员名字
	std::string m_dia_doctor; // 送检医生

	// 如果有通道，这些由服务器计算赋值
	bool m_isfree = false;        // 是否收费  有无通话 / 有无免费码匹配，由服务器计算赋值
	int m_fee;            // 报告费用，有效远程诊断才会产生，由服务器计算赋值
	int m_province_s_id;  // 上级医院所属省id，仅仅用于统计查询
	int m_city_s_id;      // 上级医院所属市id，仅仅用于统计查询
	int m_county_s_id;    // 上级医院所属县id，仅仅用于统计查询
	int m_hospital_s_id;  // 上级医院id，仅仅用于统计查询
	std::string m_hospital_s_name;

	// 其它
	bool m_updated = false;   // 报告是否被更改，由服务器计算赋值
	bool m_confirmed = false; // 是否被审核，暂时没有使用
	std::string m_video;      // 远程录制视屏
	std::string m_others;     // 备注，以后备用

	// 继承基层医院属性，仅仅用于统计查询，由服务器附加，对用户不可见
	int m_province_id; // 基层医院所属省id
	int m_city_id;     // 基层医院所属市id
	int m_county_id;   // 基层医院所属县id
	int m_hospital_id; // 基层医院id，仅仅用于统计查询

	// 新增 ***
	std::string m_patient_id; // 住院号 ***
	std::string m_ruds_reason; // 会诊原因 ***
	std::string m_ruds_target; // 会诊目的 ***
	std::string m_title; // 报告标题， 打印应该是 hospital + title ***
	CONFIRM_STATUS m_confirm_status; // 审核状态 0 / 1 / 2 / 3  无需审核 / 未审核 / 未通过 / 通过 ***
	std::string m_confirm_msg; // 审核意见 ***
	std::string m_report_url; // 远程报告的保存地址，用于二维码使用  ***
	int m_bedside_type; // 床旁超声标识 1 / 0 是否床旁超声

	// 上级医生属性
	std::string m_ruds_report_title;  // 上级医院统一报告title
	std::string m_ruds_report_bedside_title; // 上级医院统一床旁报告title
	int m_ruds_title_request;   // 远程会诊，上级医院1 / 0是 / 否强制使用上级医院的title到下级报告中 ***
};

} // namespace core

#endif // CORE_SERVICE_REPORT_H_
