#ifndef DShow_DShow_H_
#define DShow_DShow_H_

#include <list>
#include <mutex>
#include <windows.h>

#include "base/macros.h"
#include "dshow_input/dshow_input_event_handler.h"
#include "third_party/libdshowcapture/dshowcapture.hpp"

namespace DShow {

class DShowInput : public DShowInputEventHandler {
public:
    DShowInput();
    ~DShowInput();

    bool Initialize();
	void Activate();
	void Deactivate();
	void Shutdown();

    DShow::VideoConfig GetCurrentVideoConfig() { return m_current_video_config; }
    void SetCurrentVideoConfig(const DShow::VideoConfig& config) {
        m_current_video_config = config;
    }

    YUVColorSpace GetColorSpace() { return m_color_space; }
    void SetColorSpace(YUVColorSpace value) { m_color_space = value; }

    void AddDShowInputEventHandler(DShowInputEventHandler* handler);
    void RemoveDShowInputEventHandler(DShowInputEventHandler* handler);

    void OnCapturedVideoData(unsigned char *data, size_t size,
        uint32_t width, uint32_t height,
        DShow::VideoFormat format, uint64_t timestamp, 
        YUVColorSpace color_space) override;
    void OnActivateDeviceError(DShow::VideoConfig& config) override;

    static void EnumVideoDevices(std::vector<DShow::VideoDevice>& devices);

private:
	enum class Action {
		None,
		Activate,
		Deactivate,
		Shutdown,
		ConfigVideo,
		ConfigAudio,
		ConfigCrossbar1,
		ConfigCrossbar2,
	};

	class CriticalSection {
		CRITICAL_SECTION mutex;

	public:
		inline CriticalSection() { InitializeCriticalSection(&mutex); }
		inline ~CriticalSection() { DeleteCriticalSection(&mutex); }

		inline operator CRITICAL_SECTION*() { return &mutex; }
	};

	class CriticalScope {
		CriticalSection &mutex;

		CriticalScope() = delete;
		CriticalScope& operator=(CriticalScope &cs) = delete;

	public:
		inline CriticalScope(CriticalSection &mutex_) : mutex(mutex_)
		{
			EnterCriticalSection(mutex);
		}

		inline ~CriticalScope()
		{
			LeaveCriticalSection(mutex);
		}
	};

    static DWORD WINAPI DShowThread(void* param);
	static void ProcessMessages();
   
    void RemoveAllEventHandlers();

	void DShowLoop();
	void QueueAction(Action action);
	bool ActivateInternal();
	void DeactivateInternal();
	bool UpdateVideoConfig();
	bool UpdateAudioConfig();
	void OnVideoData(const DShow::VideoConfig &config,
		unsigned char *data, size_t size,
		long long startTime, long long endTime);
	void OnAudioData(const DShow::AudioConfig &config,
		unsigned char *data, size_t size,
		long long startTime, long long endTime);
	//void OnEncodedVideoData(enum AVCodecID id,
	//	unsigned char *data, size_t size, long long ts);
	//void OnEncodedAudioData(enum AVCodecID id,
	//	unsigned char *data, size_t size, long long ts);

    DShow::Device m_device;
	DShow::VideoConfig  m_video_config;
	DShow::AudioConfig  m_audio_config;

	HANDLE m_semaphore = NULL;
	HANDLE m_thread = NULL;

	CriticalSection mutex;
	std::vector<Action> m_actions;

    std::mutex m_handlers_mutex;
    std::list<DShowInputEventHandler*> m_handlers;

    DShow::VideoConfig m_current_video_config;

    YUVColorSpace m_color_space = BT_601;

	DISALLOW_COPY_AND_ASSIGN(DShowInput);
};

} // namespace media_input

#endif // DShow_DShow_H_
