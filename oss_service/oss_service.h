#ifndef OSS_SERVICE_OSS_SERVICE_H
#define OSS_SERVICE_OSS_SERVICE_H

#include <string>

#include "base/macros.h"

namespace oss {

class OssService {
public:
    SINGLETON_DEFINE(OssService);

    bool Initialize(const std::string& oss_key, const std::string& oss_secret);
    void Shutdown();

    std::string GetCurrentEndpoint();
    std::string GetReportBucket();
	std::string GetPublishBucket();
	std::string GetReportPdfDirectory();

    bool UploadReportImage(const std::string& report_number, const std::string& image_file,
      const std::string& object_name);

    bool DownloadReportImage(const std::string& image_file, const std::string& object_name);
	
	std::string GetReportPdfRootUrl();

	bool UploadReportPdf(const std::string& report_number, const std::string& pdf_file,
		const std::string& object_name);

    bool UploadSignImage(const std::string& image_file, const std::string& object_name);

    bool DownloadSignImage(const std::string& image_file, const std::string& object_name);

    std::string GetSignImageRootUrl();

private:
    OssService() = default;
    ~OssService() = default;

    std::string m_oss_key;
    std::string m_oss_secret;
};

} // namespace oss

#endif // !OSS_SERVICE_OSS_SERVICE_H
