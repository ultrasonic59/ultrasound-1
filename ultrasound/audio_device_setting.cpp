#include "audio_device_setting.h"

#include <QStyledItemDelegate>

#include "nim_service/nim_service.h"

AudioDeviceSetting::AudioDeviceSetting(QWidget *parent)
    : QDialog(parent)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    ui.setupUi(this);

    setFixedSize(width(), height());

    connect(ui.mic_auto_checkbox, SIGNAL(clicked(bool)), 
      this, SLOT(MicAutoVolumeChecked(bool)));
    connect(ui.ok_button, SIGNAL(clicked()), this, SLOT(UpdateDeviceSettings()));
    connect(ui.cancel_button, SIGNAL(clicked()), this, SLOT(reject()));

    RefreshDevices();
}

AudioDeviceSetting::~AudioDeviceSetting()
{
}

void AudioDeviceSetting::RefreshDevices() {
    m_output_devices.clear();
    m_output_devices = nim::NimService::GetInstance()->EnumDevice(
      nim::DeviceEventHandler::DeviceTypeAudioOut);

	int current_index = -1;
	nim::DeviceEventHandler::DeviceInfo default_output_device =
		nim::NimService::GetInstance()->GetDefaultDeviceInfo(nim::DeviceEventHandler::DeviceTypeAudioOut);

    for (nim::DeviceEventHandler::DeviceInfo& device : m_output_devices) {
        ui.output_combobox->addItem(
          QString::fromStdString(device.friendly_name));

		if (device.device_path == default_output_device.device_path) {
			current_index = ui.output_combobox->count() - 1;
		}
    }

	if (current_index >= 0) {
		ui.output_combobox->setCurrentIndex(current_index);
	}


	current_index = -1;
    m_input_devices.clear();
    m_input_devices = nim::NimService::GetInstance()->EnumDevice(
      nim::DeviceEventHandler::DeviceTypeAudioIn);

	nim::DeviceEventHandler::DeviceInfo default_input_device =
		nim::NimService::GetInstance()->GetDefaultDeviceInfo(nim::DeviceEventHandler::DeviceTypeAudioIn);

    for (nim::DeviceEventHandler::DeviceInfo& device : m_input_devices) {
        ui.input_combobox->addItem(
          QString::fromStdString(device.friendly_name));

		if (device.device_path == default_input_device.device_path) {
			current_index = ui.input_combobox->count() - 1;
		}
    }

	if (current_index >= 0) {
		ui.input_combobox->setCurrentIndex(current_index);
	}

    int output_volume = nim::NimService::GetInstance()->GetSpeakerAudioVolumn();
    ui.output_horizontal_slider->setValue(output_volume);

    int input_volume = nim::NimService::GetInstance()->GetMicAudioVolumn();
    ui.input_horizontal_slider->setValue(input_volume);

    bool checked = nim::NimService::GetInstance()->GetMicAutoVolumn();
    ui.mic_auto_checkbox->setChecked(checked);
    ui.input_horizontal_slider->setDisabled(checked);
}

void AudioDeviceSetting::MicAutoVolumeChecked(bool checked) {
    ui.input_horizontal_slider->setDisabled(checked);

    int input_volume = nim::NimService::GetInstance()->GetMicAudioVolumn();
    ui.input_horizontal_slider->setValue(input_volume);
}


void AudioDeviceSetting::UpdateDeviceSettings() {
    int current_output_index = ui.output_combobox->currentIndex();
    int current_input_index = ui.input_combobox->currentIndex();

    if (current_output_index >= 0
        && current_output_index < m_output_devices.size()) {
        nim::DeviceEventHandler::DeviceInfo device = 
          m_output_devices[current_output_index];
        nim::NimService::GetInstance()->SetDefaultDeviceInfo(
          nim::DeviceEventHandler::DeviceTypeAudioOut, device);
    }

    if (current_input_index >= 0
        && current_input_index < m_input_devices.size()) {
        nim::DeviceEventHandler::DeviceInfo device =
            m_input_devices[current_input_index];
        nim::NimService::GetInstance()->SetDefaultDeviceInfo(
            nim::DeviceEventHandler::DeviceTypeAudioIn, device);
    }

    nim::NimService::GetInstance()->SetSpeakerAudioVolumn(
        ui.output_horizontal_slider->value());

    nim::NimService::GetInstance()->SetMicAutoVolumn(
        ui.mic_auto_checkbox->isChecked());

    if (!ui.mic_auto_checkbox->isChecked()) {
        nim::NimService::GetInstance()->SetMicAudioVolumn(
            ui.input_horizontal_slider->value());
    }

    return accept();
}