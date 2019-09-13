#include "update_password.h"

#include "core_service/core_service.h"

UpdatePassword::UpdatePassword(QWidget *parent)
    : QDialog(parent)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    ui.setupUi(this);

    setFixedSize(width(), height());

    ui.ok_button->setEnabled(false);

    connect(ui.ok_button, SIGNAL(clicked()), this, SLOT(Submit()));

    connect(ui.old_pass_line_edit, SIGNAL(textChanged(const QString &)), 
        this, SLOT(OnTextChanged(const QString &)));
    connect(ui.new_pass_line_edit, SIGNAL(textChanged(const QString &)),
        this, SLOT(OnTextChanged(const QString &)));
    connect(ui.new_pass_confirm_line_edit, SIGNAL(textChanged(const QString &)),
        this, SLOT(OnTextChanged(const QString &)));

    connect(this, SIGNAL(UpdatePasswordSuccessedSignal()),
        this, SLOT(UpdatePasswordSuccessedSlot()));
    connect(this, SIGNAL(UpdatePasswordFailedSignal()),
        this, SLOT(UpdatePasswordFailedSlot()));

    ui.stacked_widget->setCurrentIndex(0);
}

UpdatePassword::~UpdatePassword()
{
}

void UpdatePassword::OnGetPrimaryUserInfoSuccessed(const core::PrimaryUser& user) {}
void UpdatePassword::OnGetPrimaryUserInfoFailed(int error_code, const std::string& message) {}
void UpdatePassword::OnGetSuperiorUserInfoSuccessed(const core::SuperiorUser& user) {}
void UpdatePassword::OnGetSuperiorUserInfoFailed(int error_code, const std::string& message) {}

void UpdatePassword::OnUpdatePasswordSuccessed(const std::string& uid, const std::string& new_password) {
    emit UpdatePasswordSuccessedSignal();
}

void UpdatePassword::OnUpdatePasswordFailed(int error_code, const std::string& message) {
    emit UpdatePasswordFailedSignal();
}

void UpdatePassword::OnQuerySubscriberSuperiorUserSuccessed(std::list<core::SuperiorUser> users) {}
void UpdatePassword::OnQuerySubscriberSuperiorUserFailed(int error_code, const std::string& message) {}
void UpdatePassword::OnQuerySuperiorUserSuccessed(std::list<core::SuperiorUser> users) {}
void UpdatePassword::OnQuerySuperiorUserFailed(int error_code, const std::string& message) {}
void UpdatePassword::OnUpdatePrimaryUserInfoSuccessed() {}
void UpdatePassword::OnUpdatePrimaryUserInfoFailed(int error_code, const std::string& message) {}
void UpdatePassword::OnUpdateSuperiorUserInfoSuccessed() {}
void UpdatePassword::OnUpdateSuperiorUserInfoFailed(int error_code, const std::string& message) {}

void UpdatePassword::Submit() {
    if (ui.new_pass_line_edit->text() != ui.new_pass_confirm_line_edit->text()) {
        ui.info_label->setText(QString::fromLocal8Bit("密码修改失败，新密码两次输入的内容不一致！"));
        ui.stacked_widget->setCurrentIndex(1);
        return;
    }

    ui.info_label->setText(QString::fromLocal8Bit("正在提交 ..."));
    ui.stacked_widget->setCurrentIndex(1);

    core::CoreService::GetInstance()->UpdatePassword(ui.old_pass_line_edit->text().toStdString(),
        ui.new_pass_line_edit->text().toStdString());
}

void UpdatePassword::OnTextChanged(const QString &text) {
    if (!ui.old_pass_line_edit->text().isEmpty() &&
        !ui.new_pass_line_edit->text().isEmpty() &&
        !ui.new_pass_confirm_line_edit->text().isEmpty())
        ui.ok_button->setEnabled(true);
}

void UpdatePassword::UpdatePasswordSuccessedSlot() {
    ui.info_label->setText(QString::fromLocal8Bit("密码修改成功！"));
}

void UpdatePassword::UpdatePasswordFailedSlot() {
    ui.info_label->setText(QString::fromLocal8Bit("密码修改失败！"));
}

void UpdatePassword::OnQuerySubscriberSuperiorUserOnlineStatusSuccessed(std::list<core::SuperiorUserStatus> users)
{
}

void UpdatePassword::OnQuerySubscriberSuperiorUserOnlineStatusFailed(int error_code, const std::string & message)
{
}

void UpdatePassword::OnQueryControlPrimaryUserSuccessed(std::list<core::PrimaryUser> users) {}
void UpdatePassword::OnQueryControlPrimaryUserFailed(int error_code, const std::string& message) {}
