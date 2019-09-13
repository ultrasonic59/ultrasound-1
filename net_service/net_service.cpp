#include "net_service/net_service.h"

#include <assert.h>

#include "curl.h"

namespace net {

bool NetService::Initialize() {
    CURLcode ret = curl_global_init(CURL_GLOBAL_WIN32 | CURL_GLOBAL_SSL);
    if(ret != CURLE_OK)
        return false;

    m_shutdown_event = CreateEvent(NULL, TRUE, FALSE, NULL);

    m_worker_thread = CreateThread(NULL, 0, WorkerThread, this, 0, NULL);

	m_net_proxy_info.enabled = false;

    return true;
}

void NetService::Shutdown() {

    SetEvent(m_shutdown_event);

    WaitForSingleObject(m_worker_thread, INFINITE);
    CloseHandle(m_shutdown_event);
    CloseHandle(m_worker_thread);

    CleatRequestList();

    curl_global_cleanup();
}

HttpRequest* NetService::CreateRequest() {
    HttpRequest* request = PopIdleRequest();
    assert(request);
    request->SetRequestId(GenId());
	if (m_net_proxy_info.enabled) {
		request->SetProxy(m_net_proxy_info.host, m_net_proxy_info.port , m_net_proxy_info.username, m_net_proxy_info.password);
	}
    return request;
}

void NetService::ReleaseRequest(HttpRequest* request) {
    if (request) {
        PushIdleRequest(request);
    }
}

void NetService::StartRequest(HttpRequest* request) {
    assert(request && request->GetRequestId() >= 0);
    PushActiveRequest(request);
}

void NetService::SetProxyInfo(const NetProxyInfo & proxyInfo)
{
	m_net_proxy_info = proxyInfo;
}

DWORD NetService::WorkerThread(void* param) {
    NetService* service = static_cast<NetService*>(param);
    while (true) {
        DWORD  ret = WaitForSingleObject(service->m_shutdown_event, 10);
        if (ret == WAIT_OBJECT_0)
            return 0;

        HttpRequest* request = service->PopActiveRequest();
        if (request) {

            request->Perform();

            service->PushIdleRequest(request);
        }
    }
}

HttpRequest* NetService::PopIdleRequest() {
    HttpRequest* request = nullptr;

    std::lock_guard<std::mutex> guard(m_idle_list_mutex);
    if (m_idle_list.size() > 0) {
        request = m_idle_list.front();
        m_idle_list.pop_front();
    }
    else {
        request = new HttpRequest();
    }

    return request;
}

void NetService::PushIdleRequest(HttpRequest* request) {
    if (request) {
        request->Reset();
        {
            std::lock_guard<std::mutex> guard(m_idle_list_mutex);
            m_idle_list.push_back(request);
        }
    }
}

HttpRequest* NetService::PopActiveRequest() {
    HttpRequest* request = nullptr;

    std::lock_guard<std::mutex> guard(m_active_list_mutex);
    if (m_active_list.size() > 0) {
        request = m_active_list.front();
        m_active_list.pop_front();
    }

    return request;
}

void NetService::PushActiveRequest(HttpRequest* request) {
    std::lock_guard<std::mutex> guard(m_active_list_mutex);
    m_active_list.push_back(request);
}

void NetService::CleatRequestList() {
    {
        std::lock_guard<std::mutex> guard(m_active_list_mutex);
        for (HttpRequest* request : m_active_list) {
            delete request;
        }
        m_active_list.clear();
    }

    {
        std::lock_guard<std::mutex> guard(m_idle_list_mutex);
        for (HttpRequest* request : m_idle_list) {
            delete request;
        }
        m_idle_list.clear();
    }
}

} // namespace net