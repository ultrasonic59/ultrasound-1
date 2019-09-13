#include "profile.h"

#include "core_service/core_service.h"

Profile::Profile(QWidget *parent)
    : QDialog(parent)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    ui.setupUi(this);

    setFixedSize(width(), height());

    ui.sex_combo_box->addItem(QString::fromLocal8Bit("男"));
    ui.sex_combo_box->addItem(QString::fromLocal8Bit("女"));

    ui.uid_value_label->setText(QString::fromStdString(
      core::CoreService::GetInstance()->GetCurrentPrimaryUser().GetUid()));

    QString region = QString::fromStdString(
        core::CoreService::GetInstance()->GetCurrentPrimaryUser().GetProvince()) +
        QString::fromLocal8Bit(" ") +
        QString::fromStdString(
            core::CoreService::GetInstance()->GetCurrentPrimaryUser().GetCity()) +
        QString::fromLocal8Bit(" ") +
        QString::fromStdString(
            core::CoreService::GetInstance()->GetCurrentPrimaryUser().GetCountry());
    ui.region_value_label->setText(region);
    
    ui.hospital_value_label->setText(QString::fromStdString(
        core::CoreService::GetInstance()->GetCurrentPrimaryUser().GetHospital()));

    ui.name_line_edit->setText(QString::fromStdString(
        core::CoreService::GetInstance()->GetCurrentPrimaryUser().GetName()));

    ui.phone_line_edit->setText(QString::fromStdString(
        core::CoreService::GetInstance()->GetCurrentPrimaryUser().GetPhone()));

    ui.sex_combo_box->setCurrentText(QString::fromStdString(
        core::CoreService::GetInstance()->GetCurrentPrimaryUser().GetSex()));

    ui.id_line_edit->setText(QString::fromStdString(
        core::CoreService::GetInstance()->GetCurrentPrimaryUser().GetId()));

    ui.department_line_edit->setText(QString::fromStdString(
        core::CoreService::GetInstance()->GetCurrentPrimaryUser().GetDepartment()));

    ui.title_line_edit->setText(QString::fromStdString(
        core::CoreService::GetInstance()->GetCurrentPrimaryUser().GetTitle()));

    connect(ui.submit_button, SIGNAL(clicked()), this, SLOT(UpdateProfile()));
    connect(ui.return_button, SIGNAL(clicked()), this, SLOT(ReturnSlot()));

    connect(this, SIGNAL(UpdatePrimaryUserInfoSuccessedSignal()),
        this, SLOT(UpdatePrimaryUserInfoSuccessedSlot()));
    connect(this, SIGNAL(UpdatePrimaryUserInfoFailedSignal(const QString&)),
        this, SLOT(UpdatePrimaryUserInfoFailedSlot(const QString&)));
}

Profile::~Profile()
{
}

void Profile::OnGetPrimaryUserInfoSuccessed(const core::PrimaryUser& user) {}
void Profile::OnGetPrimaryUserInfoFailed(int error_code, const std::string& message) {}
void Profile::OnGetSuperiorUserInfoSuccessed(const core::SuperiorUser& user) {}
void Profile::OnGetSuperiorUserInfoFailed(int error_code, const std::string& message) {}
void Profile::OnUpdatePasswordSuccessed(const std::string& uid, const std::string& new_password) {}
void Profile::OnUpdatePasswordFailed(int error_code, const std::string& message) {}
void Profile::OnQuerySubscriberSuperiorUserSuccessed(std::list<core::SuperiorUser> users) {}
void Profile::OnQuerySubscriberSuperiorUserFailed(int error_code, const std::string& message) {}
void Profile::OnQuerySuperiorUserSuccessed(std::list<core::SuperiorUser> users) {}
void Profile::OnQuerySuperiorUserFailed(int error_code, const std::string& message) {}

void Profile::OnUpdatePrimaryUserInfoSuccessed() {
    emit UpdatePrimaryUserInfoSuccessedSignal();
}

void Profile::OnUpdatePrimaryUserInfoFailed(int error_code, const std::string& message) {
    emit UpdatePrimaryUserInfoFailedSignal(QString::fromStdString(message));
}

void Profile::OnUpdateSuperiorUserInfoSuccessed() {}
void Profile::OnUpdateSuperiorUserInfoFailed(int error_code, const std::string& message) {}
void Profile::OnQueryControlPrimaryUserSuccessed(std::list<core::PrimaryUser> users) {}
void Profile::OnQueryControlPrimaryUserFailed(int error_code, const std::string& message) {}

void Profile::UpdateProfile() {
    if (ui.name_line_edit->text().isEmpty()) {
        ui.info_label->setText(QString::fromLocal8Bit("必须填写姓名，远程超声报告中会使用！"));
        ui.return_button->setVisible(true);
        ui.stacked_widget->setCurrentIndex(1);
        return;
    }

    ui.info_label->setText(QString::fromLocal8Bit("正在提交 ..."));
    ui.return_button->setVisible(false);
    ui.stacked_widget->setCurrentIndex(1);

    Json::Value data;
    data["uid"] = core::CoreService::GetInstance()->GetCurrentUid();
    data["name"] = ui.name_line_edit->text().toStdString();
    data["phone"] = ui.phone_line_edit->text().toStdString();
    data["sex"] = ui.sex_combo_box->currentText().toStdString();
    data["id"] = ui.id_line_edit->text().toStdString();
    data["department"] = ui.department_line_edit->text().toStdString();
    data["title"] = ui.title_line_edit->text().toStdString();

    core::CoreService::GetInstance()->UpdateCurrentUserInfo(data);
}

void Profile::ReturnSlot() {
    ui.stacked_widget->setCurrentIndex(0);
}

void Profile::UpdatePrimaryUserInfoSuccessedSlot() {
    ui.info_label->setText(QString::fromLocal8Bit("资料修改成功！"));
    ui.return_button->setVisible(false);

    core::CoreService::GetInstance()->GetCurrentUserInfo();
}

void Profile::UpdatePrimaryUserInfoFailedSlot(const QString& message) {
    ui.info_label->setText(QString::fromLocal8Bit("资料修改失败！") + message);
    ui.return_button->setVisible(true);
}

void Profile::OnQuerySubscriberSuperiorUserOnlineStatusSuccessed(std::list<core::SuperiorUserStatus> users)
{
}

void Profile::OnQuerySubscriberSuperiorUserOnlineStatusFailed(int error_code, const std::string & message)
{
}
