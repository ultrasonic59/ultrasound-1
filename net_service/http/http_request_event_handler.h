#ifndef NET_SERVICE_HTTP_HTTP_REQUEST_EVENT_HANDLER_H_
#define NET_SERVICE_HTTP_HTTP_REQUEST_EVENT_HANDLER_H_

#include "net_service/http/http_request.h"

namespace net {

class HttpRequestEventHandler {
public:
    virtual ~HttpRequestEventHandler() {}

    virtual void OnResponse(HttpRequest* request) = 0;
};

} // namespace net

#endif // NET_SERVICE_HTTP_HTTP_REQUEST_EVENT_HANDLER_H_