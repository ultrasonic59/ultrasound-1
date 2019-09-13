#ifndef NET_SERVICE_HTTP_HTTP_REQUEST_H_
#define NET_SERVICE_HTTP_HTTP_REQUEST_H_

#include <stdint.h>
#include <string>

#include "json.h"

struct curl_slist;

namespace net {

class HttpRequestEventHandler;

class HttpRequest {
public:
    enum Method {
        GET,
        POST,
        HEAD,
        DELETE_METHOD, // DELETE is already taken on Windows.
                       // <winnt.h> defines a DELETE macro.
        PUT,
        PATCH,
    };

    HttpRequest();
    ~HttpRequest();

    int64_t GetRequestId() { return m_request_Id; }
    void SetRequestId(int64_t id) { m_request_Id = id; }
    
    void SetEventHandler(HttpRequestEventHandler* handler) { 
        m_event_handler = handler;
    }

    void Reset();

    void SetMethod(Method method);

    void SetUrl(const std::string& url);

    void AddHeader(const std::string& name, const std::string& value);

    void SetUrlParam(const std::string& name, const std::string& value);

    void SetPostData(const std::string& data);

    void SetPostFile(const std::string& filePath);

    void SetTimeout(long timeout);

    void SetUpdateData(const std::string& data);

	void SetPeerUid(const std::string& peer_uid);

    void Perform();

    int GetErrorCode() { return m_error_code; }
    char* GetErrorMessage() { return m_error; }
    long GetResponseCode() { return m_response_code; }
    bool GetResponseJson(Json::Value& value);
	std::string GetPostData() { return m_post_data; }
	std::string GetPeerUid() { return m_peer_uid; }

	void SetProxy(const std::string& host, const int port, const std::string& username, const std::string& password);
private:
    class ResponseData {
    public:
        ResponseData();
        ~ResponseData();

        char* GetContents() { return m_memory; }
        size_t GetContentsSize() { return m_size; }

        size_t Append(void *buffer, size_t size, size_t nmemb);
        void Clear();
    private:
        char* m_memory = nullptr;
        size_t m_size = 0;
    };

    static size_t WriteData(void *buffer, size_t size, size_t nmemb, void *userp);
    static size_t ReadUploadData(char *buffer, size_t size, size_t nitems, void *userdata);

    void* m_handle = nullptr;

    HttpRequestEventHandler* m_event_handler = nullptr;
    int64_t m_request_Id = -1;
    std::string m_url;
    std::string m_post_data;
    Method m_method = GET;
    
    int m_error_code = 0;
    long m_response_code = 0;
    ResponseData m_response_data;
    char* m_error = nullptr;
    struct curl_slist* m_list = nullptr;
    std::string m_upload_data;
	std::string m_peer_uid;
};

} // namespace net

#endif // NET_SERVICE_HTTP_HTTP_REQUEST_H_
