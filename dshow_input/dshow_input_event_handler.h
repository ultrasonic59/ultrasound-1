#ifndef DShow_DShow_EVENT_HANDLER_H_
#define DShow_DShow_EVENT_HANDLER_H_

#include <stdint.h>

#include "third_party/libdshowcapture/dshowcapture.hpp"

namespace DShow {

class DShowInputEventHandler {
public:
    enum YUVColorSpace {
        BT_601 = 0,
        BT_709,
    };

    virtual ~DShowInputEventHandler() { }

    virtual void OnCapturedVideoData(unsigned char *data, size_t size, 
        uint32_t width, uint32_t height,
        DShow::VideoFormat format, uint64_t timestamp, 
        YUVColorSpace color_space) = 0;

    virtual void OnActivateDeviceError(DShow::VideoConfig& config) = 0;
};

} // namespace DShow

#endif // DShow_DShow_EVENT_HANDLER_H_
