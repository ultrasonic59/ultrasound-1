#pragma once

#include <QSound>
#include <QtWidgets/QMainWindow>
#include <QMediaPlayer>
#include <QMutex>
#include "ui_ultrasound_consultant.h"

#include "app/image_viewer.h"
#include "app/IOThreadWorker.h"
#include "app/SystemMessageViewer.h"
#include "core_service/user_event_handler.h"
#include "core_service/report_event_handler.h"
#include "nim_service/client_event_handler.h"
#include "nim_service/sysmsg_event_handler.h"
#include "nim_service/vchat_event_handler.h"

#include "core_service/primary_user.h"

class UltrasoundConsultant : public QMainWindow,
                             public nim::ClientEventHandler,
                             public nim::VChatEventHandler,
                             public nim::SysMsgEventHandler,
                             public core::UserEventHandler,
                             public core::ReportEventHandler,
                             public IOThreadWorkerEventHandler
{
    Q_OBJECT

public:
    UltrasoundConsultant(QWidget *parent = Q_NULLPTR);
	~UltrasoundConsultant();

    void Initialize();

    // QMainWindow
    void closeEvent(QCloseEvent *event) override;

    // nim::ClientEventHandler
    void OnLogin(const std::list<OtherClientInfo>& clients) override;
    void OnLogout() override;
    void OnLoginInfo(Info info) override;
    void OnLoginError(Error error, int code, const std::string& message) override;
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
    void OnDownloadFileSuccessed(const IOThreadFileInfo& file_info) override;
    void OnDownloadFileFailed(const IOThreadFileInfo& file_info) override;

	// Í¨¹ý SysMsgEventHandler ¼Ì³Ð
	virtual void OnReceiveMessage(const nim::SystemMessage& sys_msg) override;

signals:
    void QueryControlPrimaryUserSuccessedSignal();
    void QueryControlPrimaryUserFailedSignal();
    void KickOutSignal();
    void VChatConnectedSignal();
    void VChatConnectLogoutSignal();
    void VChatConnectErrorSignal(const QString& message);
    void ReceivedVChatInviteSignal(const QString& uid, const QString& custom_info);
    void HangupSignal();
    void CapturedVideoDataSignal(const QImage& data);
    void ReceivedVideoDataSignal(const QImage& data);
    void RealtimeStatsInfoSignal(const QString& videoInfo, const QString& audioInfo);
    void AudioVolumeNotifySignal(const int self_volume, const int other_volume);
    void VchatSyncNotifySignal(bool accept);
    void GetSuperiorUserInfoSuccessedSignal();
    void UpdatePasswordSuccessedSignal();
    void UpdatePasswordFailedSignal(const QString& message);
    void UpdateSuperiorUserInfoSuccessedSignal();
    void UpdateSuperiorUserInfoFailedSignal(const QString& message);
    void QueryReportSuccessedSignal();
    void QueryReportFailedSignal();
    void DownloadFileSuccessedSignal(const QString& report_uid, const QString& file_path, const QImage& image);
    void DownloadFileFailedSignal(const QString& report_uid, const QString& file_path);
	void ConfirmReportSuccessedSignal();
	void ConfirmReportFailedSignal(const QString& message);
	void receiveSuperiorMessage(const QString message);

public slots:
    void SetDisturb();
    void OnRightStackedWidgetCurrentChanged(int index);

    void OnConsultantControlItemClicked(QTableWidgetItem *item);
    void QueryControlPrimaryUserSuccessedSlot();
    void QueryControlPrimaryUserFailedSlot();

    void QueryControlPrimaryUser();
    void VChatStart();
    void VChatAnswer();
    void VChatWait();
    void VChatReject();
    void VChatEnd();
    void UpdateTime();

    void OpenReport();
    void QueryReport();
	void QueryWaitReport();
    void PrePage();
    void NextPage();
    void OnHistoryTableWidgetItemClicked(QTableWidgetItem *item);
	void OnRecentCallTableWidgetItemClicked(QTableWidgetItem *item);
    void PlayVideo();
	void ConfirmCurrentReport();

    void UploadSignImage();
    void UpdateProfile();
    void UpdatePassword();
    void OnMyPassTextChanged(const QString &text);

    void KickOutSlot();
    void VChatConnectedSlot();
    void VChatConnectLogoutSlot();
    void VChatConnectErrorSlot(const QString& message);
    void ReceivedVChatInviteSlot(const QString& uid, const QString& custom_info);
    void HangupSlot();
    void CapturedVideoDataSlot(const QImage& data);
    void ReceivedVideoDataSlot(const QImage& data);
    void RealtimeStatsInfoSlot(const QString& videoInfo, const QString& audioInfo);
    void AudioVolumeNotifySlot(const int self_volume, const int other_volume);
    void VchatSyncNotifySlot(bool accept);
    void GetSuperiorUserInfoSuccessedSlot();
    void UpdatePasswordSuccessedSlot();
    void UpdatePasswordFailedSlot(const QString& message);
    void UpdateSuperiorUserInfoSuccessedSlot();
    void UpdateSuperiorUserInfoFailedSlot(const QString& message);
    void QueryReportSuccessedSlot();
    void QueryReportFailedSlot();

    void DownloadFileSuccessedSlot(const QString& report_uid, const QString& file_path, const QImage& image);
    void DownloadFileFailedSlot(const QString& report_uid, const QString& file_path);

	void ConfirmReportSuccessedSlot();
	void ConfirmReportFailedSlot(const QString& message);

	void OnSendMessageClicked(void);

    void ReportImageDoubleClicked(QImage& image);

	void onReceiveSuperiorMessage(const QString message);
	void OpenSystemMessageWindow();

private:
    Ui::UltrasoundConsultantClass ui;

    enum VChatStatus {
        Idle = 0,
        Waiting,
        Speaking,
		Reject,
		End
    };

	struct VChatSession
	{
		VChatStatus status;
		QTime talk_time;
	};

    void UpdateVChatUI(VChatStatus status);

    void UpdateConsultantPageUI();
    void UpdateRecentPageUI();
    void UpdateHistoryPageUI();
    void UpdateMyPageUI();
    void UpdateSettingsPageUI();
	void UpdateRecentCallPageUI();

	void MakeRecentCallRecord(VChatSession session);
	void TryToMakeRecentCallRecord(VChatStatus status);

    void QueryReportImpl();
	void ShowSystemMessageInStatusBar(const QString& message, bool add_ts = true);

    QSound m_Sound;
    QSound m_Control_Sound;
    bool m_is_control = false;
    QString m_current_vchat_name;
	QString m_current_vchat_uid;
	QMutex vchat_end_session_mutex;
	QMap<QString,bool> m_current_vchat_userinfo_completed;
	QMap<QString, VChatSession> vchat_end_sessions;
    QTimer *m_timer = nullptr;
    QTime m_time;
	QDateTime m_beginVChatTime;
	core::PrimaryUser m_currentVChatUser;

    QAtomicInt m_do_not_disturb = 0; // 1 is enable, 0 is not.
    VChatStatus m_current_vchat_status = Idle;
    bool m_is_kick_out = false;
    QString m_user_path;
    QString m_report_path;

	bool m_query_wait = false;
    int m_skip = 0;
    int m_limit = 20;
    std::list<core::Report> m_reports;
    QString m_current_report_uid;
    int m_current_image_label = 1;

    IOThreadWorker m_io_worker;

    HANDLE m_player_process_handle = NULL;

    std::list<core::PrimaryUser> m_control_primary_users;

    ImageViewer* m_image_viewer = nullptr;

	ClickableLabel* m_show_system_message_statusbar_label = nullptr;
	SystemMessageViewer* m_system_message_viewer = nullptr;
};
