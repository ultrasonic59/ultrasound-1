#include "net_service/http/http_request.h"

extern "C" {
#include "curl.h"
}

#include "net_service/http/http_request_event_handler.h"

namespace net {

HttpRequest::ResponseData::ResponseData() {
}

HttpRequest::ResponseData::~ResponseData() {
    Clear();
}

size_t HttpRequest::ResponseData::Append(void *buffer, size_t size, size_t nmemb) {
    size_t realsize = size * nmemb;
    m_memory = (char*)realloc(m_memory, m_size + realsize + 1);
    if (m_memory == nullptr) {
        /* out of memory! */
        return 0;
    }

    memcpy(&(m_memory[m_size]), buffer, realsize);
    m_size += realsize;
    m_memory[m_size] = 0;
    return realsize;
}

void HttpRequest::ResponseData::Clear() {
    if (m_memory) {
        free(m_memory);
        m_memory = nullptr;
    }
    m_size = 0;
}


HttpRequest::HttpRequest() {
    m_handle = curl_easy_init();
    m_error = new char[CURL_ERROR_SIZE];
    memset(m_error, 0, CURL_ERROR_SIZE);

    curl_easy_setopt(m_handle, CURLOPT_WRITEFUNCTION, WriteData);
    curl_easy_setopt(m_handle, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(m_handle, CURLOPT_ERRORBUFFER, m_error);
    curl_easy_setopt(m_handle, CURLOPT_HTTPGET, 1L);
}

HttpRequest::~HttpRequest() {
    Reset();

    delete[] m_error;
    curl_easy_cleanup(m_handle);
}

void HttpRequest::Reset() {
    m_event_handler = nullptr;
    m_request_Id = -1;
    m_url.clear();
    m_method = GET;
    m_post_data.clear();

    m_error_code = 0;
    m_response_data.Clear();
    m_response_code = 0;
    memset(m_error, 0, CURL_ERROR_SIZE);

    if (m_list) {
        curl_slist_free_all(m_list);
        m_list = nullptr;
    }
    m_upload_data.clear();
	m_peer_uid.clear();

    curl_easy_reset(m_handle);

    curl_easy_setopt(m_handle, CURLOPT_WRITEFUNCTION, WriteData);
    curl_easy_setopt(m_handle, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(m_handle, CURLOPT_ERRORBUFFER, m_error);
    curl_easy_setopt(m_handle, CURLOPT_HTTPGET, 1L);
}

void HttpRequest::SetMethod(Method method) { 
    m_method = method; 

    switch (m_method)
    {
    case net::HttpRequest::GET:
        curl_easy_setopt(m_handle, CURLOPT_HTTPGET, 1L);
        break;
    case net::HttpRequest::POST:
        curl_easy_setopt(m_handle, CURLOPT_POSTFIELDS, "");
        break;
    case net::HttpRequest::HEAD:
        curl_easy_setopt(m_handle, CURLOPT_NOBODY, 1L);
        break;
    case net::HttpRequest::DELETE_METHOD:
        curl_easy_setopt(m_handle, CURLOPT_CUSTOMREQUEST, "DELETE");
        break;
    case net::HttpRequest::PUT:
        curl_easy_setopt(m_handle, CURLOPT_READFUNCTION, ReadUploadData);
        curl_easy_setopt(m_handle, CURLOPT_READDATA, this);
        curl_easy_setopt(m_handle, CURLOPT_UPLOAD, 1L);
        break;
    case net::HttpRequest::PATCH:
        break;
    default:
        break;
    }
}

void HttpRequest::SetUrl(const std::string& url) {
    m_url = url;
}

void HttpRequest::AddHeader(const std::string& name, 
  const std::string& value) {
    std::string header = name;
    header += ": ";
    header += value;
    m_list = curl_slist_append(m_list, header.c_str());
}

void HttpRequest::SetUrlParam(const std::string& name,
  const std::string& value) {

}

void HttpRequest::SetPostData(const std::string& data) {
    m_post_data = data;
    curl_easy_setopt(m_handle, CURLOPT_POSTFIELDSIZE, m_post_data.size());
    curl_easy_setopt(m_handle, CURLOPT_COPYPOSTFIELDS, m_post_data.c_str());
}

void HttpRequest::SetPostFile(const std::string& filePath) {

}

void HttpRequest::SetTimeout(long timeout) {

}

void HttpRequest::SetUpdateData(const std::string& data) {
    m_upload_data = data;
    curl_easy_setopt(m_handle, CURLOPT_INFILESIZE, (curl_off_t)m_upload_data.size());
}

void HttpRequest::SetPeerUid(const std::string& peer_uid) {
	m_peer_uid = peer_uid;
}

void HttpRequest::Perform() {
    curl_easy_setopt(m_handle, CURLOPT_URL, m_url.c_str());
    
    curl_easy_setopt(m_handle, CURLOPT_HTTPHEADER, m_list);

    // Do not strict certificate check
    curl_easy_setopt(m_handle, CURLOPT_SSL_VERIFYPEER, 0L);

    m_error_code = curl_easy_perform(m_handle);
    
    curl_easy_getinfo(m_handle, CURLINFO_RESPONSE_CODE, &m_response_code);

    if(m_event_handler)
        m_event_handler->OnResponse(this);
}

bool HttpRequest::GetResponseJson(Json::Value& value) {
    if (m_response_data.GetContents() == nullptr || m_response_data.GetContentsSize() <= 0)
        return false;

    Json::Reader reader;
    return reader.parse(m_response_data.GetContents(), value);
}

void HttpRequest::SetProxy(const std::string & host, const int port, const std::string & username, const std::string & password)
{
	if (host.length() <= 0 || port == 0) {
		return;
	}

	curl_easy_setopt(m_handle, CURLOPT_PROXY, host.c_str());
	curl_easy_setopt(m_handle, CURLOPT_PROXYPORT, port);
	if (username.length() > 0) {
		curl_easy_setopt(m_handle, CURLOPT_PROXYUSERNAME, username.c_str());
	}
	if (password.length() > 0)
	{
		curl_easy_setopt(m_handle, CURLOPT_PROXYUSERPWD, password.c_str());
	}
	
	curl_easy_setopt(m_handle, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS5);
}

size_t HttpRequest::WriteData(void *buffer, size_t size, size_t nmemb, 
  void *userp) {
    HttpRequest* request = static_cast<HttpRequest*>(userp);
    return request->m_response_data.Append(buffer, size, nmemb);
}

size_t HttpRequest::ReadUploadData(char *buffer, size_t size, size_t nitems, void *userdata) {
    HttpRequest* request = static_cast<HttpRequest*>(userdata);
    if (request->m_upload_data.empty())
        return 0;

    // Todo: handle large data
    if (request->m_upload_data.size() > (size* nitems))
        return 0;

    memcpy(buffer, request->m_upload_data.c_str(), request->m_upload_data.size());
    return request->m_upload_data.size();
}

} // namespace net