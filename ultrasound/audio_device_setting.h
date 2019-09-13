#pragma once

#include <QDialog>
#include "ui_audio_device_setting.h"
#include "nim_service/device_event_handler.h"

class AudioDeviceSetting : public QDialog
{
    Q_OBJECT

public:
    AudioDeviceSetting(QWidget *parent = Q_NULLPTR);
    ~AudioDeviceSetting();

private slots:
    void MicAutoVolumeChecked(bool checked);
    void UpdateDeviceSettings();

private:
    void RefreshDevices();

    Ui::AudioDeviceSetting ui;

    std::vector<nim::DeviceEventHandler::DeviceInfo> m_output_devices;
    std::vector<nim::DeviceEventHandler::DeviceInfo> m_input_devices;
};
