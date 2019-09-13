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
    std::string m_uid; // 全局ID
    std::string m_name; // 姓名
    std::string m_password; // 用戶密碼, 如果为空则使用电话号码，由role表保存用于登录验证
	int m_hospital_id; // 医院id
    std::string m_phone; // 电话
	int m_free; // 行医身份： 0 / 自由执业  1 / 医院职工  2 / 其它

	int m_birth; // 生日，年
    std::string m_second_hospital; // 如果不是医院职工，工作挂靠机构的名称
    std::string m_skill; // 擅长
    std::string m_description; // 简介
    std::string m_sex; // 性别
    std::string m_id; // 身份证号
    std::string m_certificate; // 职业证书
    std::string m_title; // 职称
    std::string m_url_cert; // 证书url
    std::string m_url_thumb; // 头像url
    std::string m_visit_code; // 访问码，用于临时授权用户访问
    int m_online; // 在线状态码 0 / 离线 1 / 在线  2 / 工作中, 请稍等  3 / 休息中，勿扰

	int m_price; // 单次收费, 暂时不使用
    std::list<int> m_publish; // 发布范围，由省市县医院id组成集合
    std::list<std::string> m_frees; // 免费码集合，给持有该码的医院不收费
    std::list<std::string> m_controlled; // 受控用户id集合

    // 继承医院属性
    std::string m_province; // 所属省
    std::string m_city; // 所属市
    std::string m_county; // 所属县
    std::string m_hospital; // 医院名称
	int m_province_id; // 所属省id
	int m_city_id; // 所属市id
	int m_county_id; // 所属县id
	int m_union_id; // 所属医联体id

	// 新增 ***
	std::string m_ruds_report_title;  // 医院统一报告title, 可以为空。如果为空，则默认指定，必须填入到报告的字段中 ***
	std::string m_ruds_report_bedside_title; // 医院统一床旁报告title, 可以为空。如果为空，则默认指定，必须填入到报告的字段中 ***
	int m_ruds_title_request;   // 远程会诊，上级医院1 / 0是 / 否强制使用自己的title到下级报告中 ***
	int m_ruds_confirm_request; // 远程会诊1 / 0是否需要审核，上级医院自行定制 ***
};

} // namespace core

#endif // CORE_SERVICE_SUPERIOR_USER_H_
