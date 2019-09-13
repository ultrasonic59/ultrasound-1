#pragma once

#include <QDialog>
#include "ui_device_setting.h"

#include "dshow_input/dshow_input.h"

class DeviceSetting : public QDialog
{
    Q_OBJECT

public:
    DeviceSetting(QWidget *parent, DShow::DShowInput* dshow);
    ~DeviceSetting();

    static bool IsVideoInfoEqual(DShow::VideoInfo& info, 
        DShow::VideoConfig& config);
    static const char* GetFormatName(DShow::VideoFormat format);

private slots:
    void UpdateCaps(int index);
    void UpdateDeviceSettings();

private:
    void RefreshDevices();

    Ui::DeviceSetting ui;
    
    DShow::DShowInput* m_dshow;
    std::vector<DShow::VideoDevice> m_devices;

    const long long DEFAULT_FPS = 333333;
};
