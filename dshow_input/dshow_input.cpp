#include "dshow_input/dshow_input.h"

#include <assert.h>
#include <codecvt>
#include <locale>

#include "base/strings/string_util.h"

namespace DShow {

DShowInput::DShowInput() {
    memset(&m_current_video_config, 0, sizeof(DShow::VideoConfig));
}
    
DShowInput::~DShowInput() {
	Shutdown();
}

bool DShowInput::Initialize() {

	m_semaphore = CreateSemaphore(nullptr, 0, 0x7FFFFFFF, nullptr);
	if (!m_semaphore)
		return false;

    m_thread = CreateThread(nullptr, 0, DShowThread, this, 0, nullptr);
    if (!m_thread)
        return false;

	return true;
}

void DShowInput::Activate() {
	QueueAction(Action::Activate);
}

void DShowInput::Deactivate() {
	QueueAction(Action::Deactivate);
}

void DShowInput::Shutdown() {
    RemoveAllEventHandlers();

	if (m_semaphore && m_thread)
	{
		{
			CriticalScope scope(mutex);
			m_actions.resize(1);
			m_actions[0] = Action::Shutdown;
		}

		ReleaseSemaphore(m_semaphore, 1, nullptr);

		WaitForSingleObject(m_thread, INFINITE);

		CloseHandle(m_semaphore);
		m_semaphore = NULL;

		CloseHandle(m_thread);
		m_thread = NULL;
	}
}

void DShowInput::AddDShowInputEventHandler(DShowInputEventHandler* handler) {
    std::lock_guard<std::mutex> guard(m_handlers_mutex);
    m_handlers.push_back(handler);
}

void DShowInput::RemoveDShowInputEventHandler(DShowInputEventHandler* handler) {
    std::lock_guard<std::mutex> guard(m_handlers_mutex);
    m_handlers.remove(handler);
}

void DShowInput::RemoveAllEventHandlers() {
    std::lock_guard<std::mutex> guard(m_handlers_mutex);
    m_handlers.clear();
}

void DShowInput::OnCapturedVideoData(unsigned char *data, size_t size,
    uint32_t width, uint32_t height,
    DShow::VideoFormat format, uint64_t timestamp, 
    YUVColorSpace color_space) {

    std::lock_guard<std::mutex> guard(m_handlers_mutex);
    for (DShowInputEventHandler* handler : m_handlers) {
        handler->OnCapturedVideoData(data, size, width, height, 
            format, timestamp, color_space);
    }
}

void DShowInput::OnActivateDeviceError(DShow::VideoConfig& config) {
    std::lock_guard<std::mutex> guard(m_handlers_mutex);
    for (DShowInputEventHandler* handler : m_handlers) {
        handler->OnActivateDeviceError(config);
    }
}

void DShowInput::EnumVideoDevices(std::vector<DShow::VideoDevice>& devices) {
	DShow::Device::EnumVideoDevices(devices);
}

DWORD DShowInput::DShowThread(void* param) {
    DShowInput *dshowInput = (DShowInput*)param;

    CoInitialize(nullptr);
    dshowInput->DShowLoop();
    CoUninitialize();
    return 0;
}

void DShowInput::ProcessMessages()
{
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void DShowInput::DShowLoop() {

	while (true) {
		DWORD ret = MsgWaitForMultipleObjects(1, &m_semaphore, false,
			INFINITE, QS_ALLINPUT);
		if (ret == (WAIT_OBJECT_0 + 1)) {
			ProcessMessages();
			continue;
		}
		else if (ret != WAIT_OBJECT_0) {
			break;
		}

		Action action = Action::None;
		{
			CriticalScope scope(mutex);
			if (m_actions.size()) {
				action = m_actions.front();
				m_actions.erase(m_actions.begin());
			}
		}

		switch (action) {
		case Action::Activate:
            if (!ActivateInternal()) {
                OnActivateDeviceError(m_video_config);
            }
			break;

		case Action::Deactivate:
			DeactivateInternal();
			break;

		case Action::Shutdown:
			m_device.ShutdownGraph();
			return;

		case Action::ConfigVideo:
			m_device.OpenDialog(nullptr, DShow::DialogType::ConfigVideo);
			break;

		case Action::ConfigAudio:
			m_device.OpenDialog(nullptr, DShow::DialogType::ConfigAudio);
			break;

		case Action::ConfigCrossbar1:
			m_device.OpenDialog(nullptr, DShow::DialogType::ConfigCrossbar);
			break;

		case Action::ConfigCrossbar2:
			m_device.OpenDialog(nullptr, DShow::DialogType::ConfigCrossbar2);
			break;

		case Action::None:;
		}
	}
}

void DShowInput::QueueAction(Action action)
{
	CriticalScope scope(mutex);
	m_actions.push_back(action);
	ReleaseSemaphore(m_semaphore, 1, nullptr);
}

bool DShowInput::ActivateInternal() {
	if (!m_device.ResetGraph())
		return false;

	if (!UpdateVideoConfig()) {
		return false;
	}

	UpdateAudioConfig();

	if (!m_device.ConnectFilters())
		return false;

	if (m_device.Start() != DShow::Result::Success)
		return false;

	return true;
}

void DShowInput::DeactivateInternal() {
	m_device.ResetGraph();
}

bool DShowInput::UpdateVideoConfig() {
	int cx = 0, cy = 0;
	long long interval = 0;
	DShow::VideoFormat format = DShow::VideoFormat::Any;

	m_video_config.name = m_current_video_config.name;
    m_video_config.path = m_current_video_config.path;
	m_video_config.useDefaultConfig = m_current_video_config.useDefaultConfig;
    if (!m_video_config.useDefaultConfig)
    {
        cx = m_current_video_config.cx;
        cy = m_current_video_config.cy;
        interval = m_current_video_config.frameInterval;
        format = m_current_video_config.format;
    }
	m_video_config.cx = cx;
	m_video_config.cy = cy;
	m_video_config.frameInterval = interval;
	m_video_config.internalFormat = format;

	m_video_config.callback = std::bind(&DShowInput::OnVideoData, this,
		std::placeholders::_1, std::placeholders::_2,
		std::placeholders::_3, std::placeholders::_4,
		std::placeholders::_5);

	if (m_video_config.internalFormat != DShow::VideoFormat::MJPEG)
		m_video_config.format = m_video_config.internalFormat;

	if (!m_device.SetVideoConfig(&m_video_config)) 
		return false;

	if (m_video_config.internalFormat == DShow::VideoFormat::MJPEG) {
		m_video_config.format = DShow::VideoFormat::XRGB;
		m_video_config.useDefaultConfig = false;

		if (!m_device.SetVideoConfig(&m_video_config))
			return false;
	}

    // Save the real working config.
    m_current_video_config.cx = m_video_config.cx;
    m_current_video_config.cy = m_video_config.cy;
    m_current_video_config.format = m_video_config.format;
    m_current_video_config.frameInterval = m_video_config.frameInterval;
    m_current_video_config.internalFormat = m_video_config.internalFormat;

	return true;
}

bool DShowInput::UpdateAudioConfig() {
	return true;
}

void DShowInput::OnVideoData(const DShow::VideoConfig &config,
	unsigned char *data, size_t size,
	long long startTime, long long endTime) {

	if (m_video_config.format == DShow::VideoFormat::H264) {
		//OnEncodedVideoData(AV_CODEC_ID_H264, data, size, startTime);
		return;
	}

    uint32_t width = config.cx;
    uint32_t height = config.cy;
    uint64_t timestamp = startTime * 100;
    DShow::VideoFormat format = config.format;

    OnCapturedVideoData(data, size, width, height, format, timestamp, 
      m_color_space);

    /*
	const int cx = config.cx;
	const int cy = config.cy;
    m_frame.data_size = size;
    m_frame.timestamp = (uint64_t)startTime * 100;;
	m_frame.width = config.cx;
	m_frame.height = config.cy;
	m_frame.format = ConvertVideoFormat(config.format);
	m_frame.flip = (config.format == DShow::VideoFormat::XRGB ||
		config.format == DShow::VideoFormat::ARGB);

	if (m_video_config.format == DShow::VideoFormat::XRGB ||
		m_video_config.format == DShow::VideoFormat::ARGB) {
		m_frame.data[0] = data;
		m_frame.linesize[0] = cx * 4;

	}
	else if (m_video_config.format == DShow::VideoFormat::YVYU ||
		m_video_config.format == DShow::VideoFormat::YUY2 ||
		m_video_config.format == DShow::VideoFormat::HDYC ||
		m_video_config.format == DShow::VideoFormat::UYVY) {
		m_frame.data[0] = data;
		m_frame.linesize[0] = cx * 2;

	}
	else if (m_video_config.format == DShow::VideoFormat::I420) {
		m_frame.data[0] = data;
		m_frame.data[1] = m_frame.data[0] + (cx * cy);
		m_frame.data[2] = m_frame.data[1] + (cx * cy / 4);
		m_frame.linesize[0] = cx;
		m_frame.linesize[1] = cx / 2;
		m_frame.linesize[2] = cx / 2;

	}
	else if (m_video_config.format == DShow::VideoFormat::YV12) {
		m_frame.data[0] = data;
		m_frame.data[2] = m_frame.data[0] + (cx * cy);
		m_frame.data[1] = m_frame.data[2] + (cx * cy / 4);
		m_frame.linesize[0] = cx;
		m_frame.linesize[1] = cx / 2;
		m_frame.linesize[2] = cx / 2;

	}
	else if (m_video_config.format == DShow::VideoFormat::NV12) {
		m_frame.data[0] = data;
		m_frame.data[1] = m_frame.data[0] + (cx * cy);
		m_frame.linesize[0] = cx;
		m_frame.linesize[1] = cx;

	}
	else if (m_video_config.format == DShow::VideoFormat::Y800) {
		m_frame.data[0] = data;
		m_frame.linesize[0] = cx;
	}
	else {
		// TODO: other formats 
		return;
	}
    */
}

void DShowInput::OnAudioData(const DShow::AudioConfig &config,
	unsigned char *data, size_t size,
	long long startTime, long long endTime) {

}

//void DShowInput::OnEncodedVideoData(enum AVCodecID id,
//	unsigned char *data, size_t size, long long ts) {
//
//}
//
//void DShowInput::OnEncodedAudioData(enum AVCodecID id,
//	unsigned char *data, size_t size, long long ts) {
//
//}

} // namespace media_input