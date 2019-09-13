#include "oss_service/oss_service.h"

#include "oss_api.h"
#include "aos_http_io.h"

namespace oss {

static const char kEndPoint[] = "oss-cn-shenzhen.aliyuncs.com";
static const char kReportBucket[] = "dds-reports";
static const char kPublishBucket[] = "dds-publish";

static const char kReportPdfDirectory[] = "reports";
static const char kSuperiorSignDirectory[] = "sign";

bool OssService::Initialize(const std::string& oss_key, 
  const std::string& oss_secret) {

    if (aos_http_io_initialize(NULL, 0) != AOSE_OK)
        return false;

    m_oss_key = oss_key;
    m_oss_secret = oss_secret;

    return true;
}

void OssService::Shutdown() {
    aos_http_io_deinitialize();
}

std::string OssService::GetCurrentEndpoint() {
    return kEndPoint;
}

std::string OssService::GetReportBucket() {
    return kReportBucket;
}

std::string OssService::GetPublishBucket() {
	return kPublishBucket;
}

std::string OssService::GetReportPdfDirectory() {
	return kReportPdfDirectory;
}

bool OssService::UploadReportImage(const std::string& report_number, const std::string& image_file,
  const std::string& object_name) {
    aos_pool_t *p = NULL;
    aos_pool_create(&p, NULL);
    oss_request_options_t *options = oss_request_options_create(p);

    options->config = oss_config_create(options->pool);
    aos_str_set(&options->config->endpoint, kEndPoint);
    aos_str_set(&options->config->access_key_id, m_oss_key.c_str());
    aos_str_set(&options->config->access_key_secret, m_oss_secret.c_str());
    options->config->is_cname = 0;
    options->ctl = aos_http_controller_create(options->pool, 0);

    aos_table_t *headers = aos_table_make(options->pool, 1);
    apr_table_set(headers, "Content-Type", "image/png");
    aos_string_t bucket;
    aos_str_set(&bucket, kReportBucket);
    aos_string_t object;
    aos_str_set(&object, object_name.c_str());
    aos_string_t file;
    aos_str_set(&file, image_file.c_str());

    aos_table_t *resp_headers = NULL;
    aos_status_t *s = oss_put_object_from_file(options, &bucket, &object, &file,
        headers, &resp_headers);

    bool success = false;

    if (aos_status_is_ok(s)) {
        success = true;
    }
    else {
        printf("put object from file failed, code:%d, error_code:%s, error_msg:%s, request_id:%s\n",
            s->code, s->error_code, s->error_msg, s->req_id);
    }

    aos_pool_destroy(p);
    
    return success;
}

bool OssService::DownloadReportImage(const std::string& image_file, 
  const std::string& object_name) {

    aos_pool_t *p = NULL;
    aos_pool_create(&p, NULL);
    oss_request_options_t *options = oss_request_options_create(p);

    options->config = oss_config_create(options->pool);
    aos_str_set(&options->config->endpoint, kEndPoint);
    aos_str_set(&options->config->access_key_id, m_oss_key.c_str());
    aos_str_set(&options->config->access_key_secret, m_oss_secret.c_str());
    options->config->is_cname = 0;
    options->ctl = aos_http_controller_create(options->pool, 0);

    aos_table_t *headers = aos_table_make(p, 0);
    aos_string_t bucket;
    aos_str_set(&bucket, kReportBucket);
    aos_string_t object;
    aos_str_set(&object, object_name.c_str());
    aos_string_t file;
    aos_str_set(&file, image_file.c_str());

    aos_table_t *resp_headers = NULL;
    aos_status_t *s = oss_get_object_to_file(options, &bucket, &object, headers,
        nullptr, &file, &resp_headers);

    bool success = false;

    if (aos_status_is_ok(s)) {
        success = true;
    }
    else {
        printf("get object to local file failed\n");
    }

    aos_pool_destroy(p);

    return success;
}

std::string OssService::GetReportPdfRootUrl() {
	std::string url = "http://";
	url += kPublishBucket;
	url += ".";
	url += kEndPoint;
	return url;
}

bool OssService::UploadReportPdf(const std::string& report_number, const std::string& pdf_file,
	const std::string& object_name) {

	aos_pool_t *p = NULL;
	aos_pool_create(&p, NULL);
	oss_request_options_t *options = oss_request_options_create(p);

	options->config = oss_config_create(options->pool);
	aos_str_set(&options->config->endpoint, kEndPoint);
	aos_str_set(&options->config->access_key_id, m_oss_key.c_str());
	aos_str_set(&options->config->access_key_secret, m_oss_secret.c_str());
	options->config->is_cname = 0;
	options->ctl = aos_http_controller_create(options->pool, 0);

	aos_table_t *headers = aos_table_make(options->pool, 1);
	apr_table_set(headers, "Content-Type", "application/pdf");
	aos_string_t bucket;
	aos_str_set(&bucket, kPublishBucket);
	aos_string_t object;
	aos_str_set(&object, object_name.c_str());
	aos_string_t file;
	aos_str_set(&file, pdf_file.c_str());

	aos_table_t *resp_headers = NULL;
	aos_status_t *s = oss_put_object_from_file(options, &bucket, &object, &file,
		headers, &resp_headers);

	bool success = false;

	if (aos_status_is_ok(s)) {
		success = true;
	}
	else {
		printf("put object from file failed, code:%d, error_code:%s, error_msg:%s, request_id:%s\n",
			s->code, s->error_code, s->error_msg, s->req_id);
	}

	aos_pool_destroy(p);

	return success;
}

bool OssService::UploadSignImage(const std::string& image_file, 
                                 const std::string& object_name) {
  aos_pool_t *p = NULL;
  aos_pool_create(&p, NULL);
  oss_request_options_t *options = oss_request_options_create(p);

  options->config = oss_config_create(options->pool);
  aos_str_set(&options->config->endpoint, kEndPoint);
  aos_str_set(&options->config->access_key_id, m_oss_key.c_str());
  aos_str_set(&options->config->access_key_secret, m_oss_secret.c_str());
  options->config->is_cname = 0;
  options->ctl = aos_http_controller_create(options->pool, 0);

  aos_table_t *headers = aos_table_make(options->pool, 1);
  apr_table_set(headers, "Content-Type", "image/png");
  aos_string_t bucket;
  aos_str_set(&bucket, kPublishBucket);
  aos_string_t object;
  aos_str_set(&object, object_name.c_str());
  aos_string_t file;
  aos_str_set(&file, image_file.c_str());

  aos_table_t *resp_headers = NULL;
  aos_status_t *s = oss_put_object_from_file(options, &bucket, &object, &file,
                                             headers, &resp_headers);

  bool success = false;

  if (aos_status_is_ok(s)) {
    success = true;
  }
  else {
    printf("put object from file failed, code:%d, error_code:%s, error_msg:%s, request_id:%s\n",
           s->code, s->error_code, s->error_msg, s->req_id);
  }

  aos_pool_destroy(p);

  return success;
}

bool OssService::DownloadSignImage(const std::string& image_file, 
                                   const std::string& object_name) {
  aos_pool_t *p = NULL;
  aos_pool_create(&p, NULL);
  oss_request_options_t *options = oss_request_options_create(p);

  options->config = oss_config_create(options->pool);
  aos_str_set(&options->config->endpoint, kEndPoint);
  aos_str_set(&options->config->access_key_id, m_oss_key.c_str());
  aos_str_set(&options->config->access_key_secret, m_oss_secret.c_str());
  options->config->is_cname = 0;
  options->ctl = aos_http_controller_create(options->pool, 0);

  aos_table_t *headers = aos_table_make(p, 0);
  aos_string_t bucket;
  aos_str_set(&bucket, kPublishBucket);
  aos_string_t object;
  aos_str_set(&object, object_name.c_str());
  aos_string_t file;
  aos_str_set(&file, image_file.c_str());

  aos_table_t *resp_headers = NULL;
  aos_status_t *s = oss_get_object_to_file(options, &bucket, &object, headers,
                                           nullptr, &file, &resp_headers);

  bool success = false;

  if (aos_status_is_ok(s)) {
    success = true;
  }
  else {
    printf("get object to local file failed\n");
  }

  aos_pool_destroy(p);

  return success;
}

std::string OssService::GetSignImageRootUrl() {
  std::string url = "http://";
  url += kPublishBucket;
  url += ".";
  url += kEndPoint;
  return url;
}

} // namespace oss