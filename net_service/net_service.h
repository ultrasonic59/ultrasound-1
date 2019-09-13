#ifndef NET_SERVICE_NET_SERVICE_H_
#define NET_SERVICE_NET_SERVICE_H_

#include <atomic>
#include <list>
#include <mutex>
#include <windows.h>

#include "base/macros.h"
#include "net_service/http/http_request.h"

namespace net {

struct NetProxyInfo {
	bool enabled;
	std::string host;
	int port;
	std::string username;
	std::string password;
};

class NetService {
public:
    SINGLETON_DEFINE(NetService);

    bool Initialize();
    void Shutdown();

    HttpRequest* CreateRequest();
    void ReleaseRequest(HttpRequest* request);

    void StartRequest(HttpRequest* request);

	void SetProxyInfo(const NetProxyInfo& proxyInfo);
private:
    NetService() = default;
    ~NetService() = default;

    static DWORD WINAPI WorkerThread(void* param);

    int64_t GenId() { return m_id++; }

    HttpRequest* PopIdleRequest();
    void PushIdleRequest(HttpRequest* request);
    
    HttpRequest* PopActiveRequest();
    void PushActiveRequest(HttpRequest* request);
    
    void CleatRequestList();

    std::atomic_int64_t m_id;

    HANDLE m_worker_thread;
    HANDLE m_shutdown_event;

    std::mutex m_idle_list_mutex;
    std::list<HttpRequest*> m_idle_list;

    std::mutex m_active_list_mutex;
    std::list<HttpRequest*> m_active_list;

	NetProxyInfo m_net_proxy_info;
};

} // namespace net

#endif // NET_SERVICE_NET_SERVICE_H_
