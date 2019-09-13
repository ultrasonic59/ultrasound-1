#ifndef CORE_SERVICE_REPORT_EVENT_HANDLER_H_
#define CORE_SERVICE_REPORT_EVENT_HANDLER_H_

#include "core_service/report.h"

namespace core {

struct ConfirmReportData {
	std::string superior_name;
	std::string superior_id;
	std::string uid;
	std::string content;
	std::string conclusion;
	CONFIRM_STATUS confirm_status;
	std::string confirm_msg;
	std::string primary_id;
};

class ReportEventHandler {
public:
    virtual ~ReportEventHandler() {}

	virtual void OnCreateReportSuccessed() = 0;
	virtual void OnCreateReportFailed(int error_code, const std::string& message) = 0;

	virtual void OnUpdateReportSuccessed() = 0;
	virtual void OnUpdateReportFailed(int error_code, const std::string& message) = 0;

	virtual void OnConfirmReportSuccessed(const ConfirmReportData& data) = 0;
	virtual void OnConfirmReportFailed(int error_code, const std::string& message) = 0;

	virtual void OnQueryReportSuccessed(std::list<Report> reports) = 0;
	virtual void OnQueryReportFailed(int error_code, const std::string& message) = 0;
};

} // namespace core 

#endif // CORE_SERVICE_REPORT_EVENT_HANDLER_H_