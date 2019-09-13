#pragma once
#include <QPrinter>
#include <QSound>
#include <QSqlDatabase>
#include <QTime>
#include <QtWidgets/QMainWindow>
#include <QFileInfoList>
#include "ui_ultrasound.h"

#include "app/ClickableLabel.h"
#include "app/image_viewer.h"
#include "app/IOThreadWorker.h"
#include "app/SystemMessageViewer.h"
#include "call.h"
#include "core_service/user_event_handler.h"
#include "core_service/report_event_handler.h"
#include "dshow_input/dshow_input.h"
#include "dshow_input/dshow_input_event_handler.h"
#include "nim_service/client_event_handler.h"
#include "nim_service/vchat_event_handler.h"
#include "nim_service/sysmsg_event_handler.h"
#include "report_thread_worker.h"

class ultrasound : public QMainWindow, 
                   public DShow::DShowInputEventHandler,
                   public nim::ClientEventHandler,
                   public nim::VChatEventHandler,
	               public nim::SysMsgEventHandler,
                   public core::UserEventHandler,
                   public core::ReportEventHandler,
                   public IOThreadWorkerEventHandler
{
    Q_OBJECT

public:
    ultrasound(QWidget *parent = Q_NULLPTR);
    ~ultrasound();

    void Initialize();
    DShow::DShowInput* GetDShowInput() { return m_dshow; }

    // QMainWindow
    void closeEvent(QCloseEvent *event) override;

    // DShow::DShowInputEventHandler
    void OnCapturedVideoData(unsigned char *data, size_t size,
      uint32_t width, uint32_t height,
      DShow::VideoFormat format, uint64_t timestamp,
      YUVColorSpace color_space) override;
    void OnActivateDeviceError(DShow::VideoConfig& config) override;

    // nim::ClientEventHandler
    void OnLogin(const std::list<OtherClientInfo>& clients) override {}
    void OnLogout() override {}
    void OnLoginInfo(Info info) override {}
    void OnLoginError(Error error, int code, const std::string& message) override {}
    void OnKickOut(ClientType client_type, KickReason kick_reason) override;

    // nim::VChatEventHandler
    void OnVChatChannelCreated(const int64_t channel_id) override;
    void OnVChatStartError(int code) override;
    void OnVChatConnected(const int64_t channel_id) override;
    void OnVChatConnectLogout() override;
    void OnVChatConnectError(const int code) override;
    void OnCalleeRejected() override;
    void OnControlNotify(const std::string& uid, ControlType type) override;
    void OnReceivedVChatInvite(const int64_t channel_id, const std::string& uid,
      const std::string& custom_info) override;
    void OnHangup() override;
    void OnCapturedVideoData(unsigned __int64 time, const char* data,
        unsigned int size, unsigned int width, unsigned int height) override;
    void OnReceivedVideoData(unsigned __int64 time, const char* data,
      unsigned int size, unsigned int width, unsigned int height) override;
    void OnRealtimeStatsInfo(const StatsInfo& info) override;
    void OnSessionNetStatus(const std::string& uid, 
      const SessionNetStatus status) override;
    void OnVChatOptError(const OptType type, const int code) override;
    void OnAudioVolumeNotify(const int32_t self_volume,
      const int32_t other_volume) override;
    void OnVchatSyncNotify(bool accept) override;

    // core::UserEventHandler
    void OnGetPrimaryUserInfoSuccessed(const core::PrimaryUser& user) override;
    void OnGetPrimaryUserInfoFailed(int error_code, const std::string& message) override;
    void OnGetSuperiorUserInfoSuccessed(const core::SuperiorUser& user) override;
    void OnGetSuperiorUserInfoFailed(int error_code, const std::string& message) override;
    void OnUpdatePasswordSuccessed(const std::string& uid, const std::string& new_password) override;
    void OnUpdatePasswordFailed(int error_code, const std::string& message) override;
    void OnQuerySubscriberSuperiorUserSuccessed(std::list<core::SuperiorUser> users) override;
    void OnQuerySubscriberSuperiorUserFailed(int error_code, const std::string& message) override;
    void OnQuerySuperiorUserSuccessed(std::list<core::SuperiorUser> users) override;
    void OnQuerySuperiorUserFailed(int error_code, const std::string& message) override;
    void OnUpdatePrimaryUserInfoSuccessed() override;
    void OnUpdatePrimaryUserInfoFailed(int error_code, const std::string& message) override;
    void OnUpdateSuperiorUserInfoSuccessed() override;
    void OnUpdateSuperiorUserInfoFailed(int error_code, const std::string& message) override;
    void OnQuerySubscriberSuperiorUserOnlineStatusSuccessed(std::list<core::SuperiorUserStatus> users) override;
    void OnQuerySubscriberSuperiorUserOnlineStatusFailed(int error_code, const std::string & message) override;
    void OnQueryControlPrimaryUserSuccessed(std::list<core::PrimaryUser> users) override;
    void OnQueryControlPrimaryUserFailed(int error_code, const std::string& message) override;

    // core::ReportEventHandler
    void OnCreateReportSuccessed() override;
    void OnCreateReportFailed(int error_code, const std::string& message) override;
    void OnUpdateReportSuccessed() override;
    void OnUpdateReportFailed(int error_code, const std::string& message) override;
	void OnConfirmReportSuccessed(const core::ConfirmReportData& data) override;
	void OnConfirmReportFailed(int error_code, const std::string& message) override;
    void OnQueryReportSuccessed(std::list<core::Report> reports) override;
    void OnQueryReportFailed(int error_code, const std::string& message) override;

    // IOThreadWorkerEventHandler
    void OnDownloadFileSuccessed(const IOThreadFileInfo& file_info) override {}
    void OnDownloadFileFailed(const IOThreadFileInfo& file_info) override {}

	// Í¨¹ý SysMsgEventHandler ¼Ì³Ð
	virtual void OnReceiveMessage(const nim::SystemMessage& sys_msg) override;

signals:
    void UpdateCapturedVideoDataSignal(const QImage& image);
    void ActivateDeviceErrorSignal(const QString& info);
    void UpdateVChatVideoDataSignal(const QImage& image);
    void StartAudioSignal();
    void UpdateRealtimeStatsInfoSignal(const QString& video_info, 
        const QString& audio_info);
    void VChatErrorSignal(const QString& message);
    void VChatCalleeRejectedSignal();
    void VChatReceiveInviteSignal(const QString& uid, const QString& info);
    void VChatHangupSignal();
    void VChatBusyLineSignal();
    void VChatConnectLogoutSignal();
    void UpdateAudioVolumeMeterSignal(const int self_volume,
        const int other_volume);
    void AddCaptureImageSignal(const QImage&);
    void KickOutSignal();
	void receiveSuperiorMessage(const QString message);
	void ReceiveReportConfirmedMessageSignal(const QString& superior_id, const QString& superior_name, 
		const QString& uid, const QString& content, const QString& conclusion, const int confirm_status,
		const QString& confirm_msg);

public slots:
    void OpenVideoDeviceSetting();
    void OpenAudioDeviceSetting();
    void CaptureVideoData();
    void NewReport();
    void SaveReport();
    void PreviewReport();
    void PrintReport();
    void Print(QPrinter *p);
    void OpenReportTemplate();
    void OpenReportQuery();
    void OpenUpdatePassword();
    void OpenProfile();
	void onReceiveSuperiorMessage(const QString message);
	void ReceiveReportConfirmedMessage(const QString& superior_id, const QString& superior_name,
		const QString& uid, const QString& content, const QString& conclusion, const int confirm_status,
		const QString& confirm_msg);
	void OpenSystemMessageWindow();

    void UpdateCapturedVideoData(const QImage& image);
    void ActivateDeviceError(const QString& info);
    
    void UpdateVChatVideoData(const QImage& image);
    void StartAudio();
    void UpdateRealtimeStatsInfo(const QString& video_info, 
        const QString& audio_info);
    void ShowCallWidget();
    void StartVChat();
    void EndVChat();
    void VChatError(const QString& message);
    void VChatCalleeRejected();
    void VChatReceiveInvite(const QString& uid, const QString& info);
    void VChatHangup();
    void VChatBusyLine();
    void VChatConnectLogout();
    void UpdateAudioVolumeMeter(const int self_volume,
      const int other_volume);
    void UpdateTime();

    void AddCaptureImage(const QImage&);
    void DeleteCaptureImage();
    void CaptureImageClicked(QListWidgetItem *item);
    void CaptureImageDoubleClicked(QListWidgetItem *item);

    void OnCheckPointIndexChanged(const QString& text);
    void OnCheckPointEditTextChanged(const QString &text);

    void KickOut();
    void LoadReport(const core::Report& report);

    std::string GenNewReportId();

private:
    enum VChatStatus {
        Idle = 0,
        Waiting,
        Speaking
    };

    enum ReportStatus {
        New = 0,
        Loaded,
        Saved,
    };

    struct VChatChannelInfo {
        bool is_valid = false;
        std::string uid;
        std::string name;
		std::string ruds_report_title;
		std::string ruds_report_bedside_title;
		std::string hospital;
		int hospital_id;
		bool ruds_title_request = false;
		bool ruds_confirm_request = false;
        int64_t channel = 0;
        int channel_time = 0;

        void Clear() {
            is_valid = false;
            uid.clear();
            name.clear();
			ruds_report_title.clear();
			ruds_report_bedside_title.clear();
			hospital.clear();
			hospital_id = 0;
			ruds_title_request = false;
			ruds_confirm_request = false;
            channel = 0;
            channel_time = 0;
        }
    };

    void SaveCapturedVideoData(const QImage& image);
    void UpdateReportUI(ReportStatus status);
    void UpdateVChatUI(VChatStatus status, const QString& remote_user_name);

    void LoadDepartment(bool reload);
    void LoadDoctor(bool reload);
    void LoadCheckPoint(bool reload);

    void OpenTemplateDB();
    void CloseTemplateDB();

    void BuildReportPath();
    void SaveReportToServer(bool auto_save);

    QString GetCurrentReportId();

    QFileInfoList GetReportCapturedImages();

	void ShowSystemMessageInStatusBar(const QString& message, bool add_ts = true);

    friend class ReportThreadWorker;

    Ui::ultrasoundClass ui;
    DShow::DShowInput* m_dshow = nullptr;
    QTimer *m_timer = nullptr;
    QTime m_time;
	ClickableLabel* m_show_system_message_statusbar_label = nullptr;

    QAtomicInt m_do_capture;
    QAtomicInt m_capture_filename_index;

    QString m_user_path;
    QString m_report_path;
    ReportStatus m_report_current_status = New;
    QAtomicInt m_vchat_current_status = Idle;

    QSqlDatabase m_template_db;

    std::map<QString, QImage> m_capture_image_list;
    std::list<QString> m_checked_image_list;

    ImageViewer* m_image_viewer = nullptr;
	SystemMessageViewer* m_system_message_viewer = nullptr;

    QSound m_Sound;

    QString m_check_point_text;
    bool m_is_kick_out = false;

    VChatChannelInfo m_current_channel_info;

    IOThreadWorker m_io_worker;

    static const int MAX_CAPTURE_IMAGE = 50;
    static const int MAX_SELECT_IMAGE = 3;




};
