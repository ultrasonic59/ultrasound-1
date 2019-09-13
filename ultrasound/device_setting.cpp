#include "device_setting.h"

#include <assert.h>

DeviceSetting::DeviceSetting(QWidget *parent, DShow::DShowInput* dshow)
    : QDialog(parent), m_dshow(dshow)
{
    assert(m_dshow);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    ui.setupUi(this);

    setFixedSize(width(), height());
  
    connect(ui.deviceComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(UpdateCaps(int)));
    
    connect(ui.okButton, SIGNAL(clicked()), this, SLOT(UpdateDeviceSettings()));
    connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

    ui.colorspaceComboBox->addItem("BT.601");
    ui.colorspaceComboBox->addItem("BT.709");

    RefreshDevices();
}

DeviceSetting::~DeviceSetting() {

}

void DeviceSetting::RefreshDevices() {
    m_devices.clear();
    DShow::DShowInput::EnumVideoDevices(m_devices);

    int current_index = -1;
    DShow::VideoConfig current_config =
        m_dshow->GetCurrentVideoConfig();

    for (DShow::VideoDevice& device : m_devices) {
        ui.deviceComboBox->addItem(QString::fromWCharArray(device.name.c_str()));

        if (device.name == current_config.name
            && device.path == current_config.path) {
            current_index = ui.deviceComboBox->count() - 1;
        }
    }

    if (current_index >= 0) {
        ui.deviceComboBox->setCurrentIndex(current_index);
    }

    ui.colorspaceComboBox->setCurrentIndex(m_dshow->GetColorSpace());
}

bool DeviceSetting::IsVideoInfoEqual(DShow::VideoInfo& info, 
  DShow::VideoConfig& config) {
    return info.format == config.format
        && info.maxCX == config.cx
        && info.maxCY == config.cy
        && info.maxInterval >= config.frameInterval
        && info.minInterval <= config.frameInterval;
}

const char* DeviceSetting::GetFormatName(DShow::VideoFormat format) {
    switch (format) {
    case DShow::VideoFormat::ARGB:
        return "ARGB";
    case DShow::VideoFormat::XRGB:
        return "XRGB";
    case DShow::VideoFormat::I420:
        return "I420";
    case DShow::VideoFormat::NV12:
        return "NV12";
    case DShow::VideoFormat::YV12:
        return "YV12";
    case DShow::VideoFormat::Y800:
        return "Y800";
    case DShow::VideoFormat::YVYU:
        return "YVYU";
    case DShow::VideoFormat::YUY2:
        return "YUY2";
    case DShow::VideoFormat::UYVY:
        return "UYVY";
    case DShow::VideoFormat::HDYC:
        return "HDYC";
    case DShow::VideoFormat::MJPEG:
        return "MJPEG";
    case DShow::VideoFormat::H264:
        return "H264";
    case DShow::VideoFormat::Any:
    case DShow::VideoFormat::Unknown:
    default:
        break;
    }

    return "";
}

void DeviceSetting::UpdateCaps(int index) {
    ui.capComboBox->clear();

    if (index >= 0 && index < m_devices.size()) {
        
        int current_index = -1;
        DShow::VideoConfig current_config =
            m_dshow->GetCurrentVideoConfig();

        for (DShow::VideoInfo& info : m_devices[index].caps) {
            QString item;
            item.append(GetFormatName(info.format));
            item.append(",");
            item.append(QString::number(info.maxCX));
            item.append("x");
            item.append(QString::number(info.maxCY));
            item.append(",");
            item.append(QString::number((float)info.minInterval / 10000 )); //ms
            item.append("-");
            item.append(QString::number((float)info.maxInterval / 10000)); //ms

            if (IsVideoInfoEqual(info, current_config)) {
                item.append(" @ ");
                item.append(QString::number(
                  (float)current_config.frameInterval / 10000));
            }

            ui.capComboBox->addItem(item);

            if (IsVideoInfoEqual(info, current_config)) {
                current_index = ui.capComboBox->count() - 1;
            }
        }

        if (current_index >= 0) {
            ui.capComboBox->setCurrentIndex(current_index);
        }
    }
}

void DeviceSetting::UpdateDeviceSettings() {
    m_dshow->SetColorSpace(
        (DShow::DShowInputEventHandler::YUVColorSpace)
        ui.colorspaceComboBox->currentIndex());

    int current_device_index = ui.deviceComboBox->currentIndex();
    int current_cap_index = ui.capComboBox->currentIndex();
    
    if (current_device_index >= 0 
        && current_device_index < m_devices.size()) {
        
        DShow::VideoDevice& device = m_devices[current_device_index];
        DShow::VideoConfig current_config =
            m_dshow->GetCurrentVideoConfig();

        if (current_cap_index >= 0
            && current_cap_index < device.caps.size()) {
            
            DShow::VideoInfo& cap = device.caps[current_cap_index];

            if (device.name != current_config.name
                || device.path != current_config.path
                || !IsVideoInfoEqual(cap, current_config)) {

                DShow::VideoConfig config;
                config.name = device.name;
                config.path = device.path;
                config.useDefaultConfig = false;
                config.cx = cap.maxCX;
                config.cy = cap.maxCY;
                config.format = cap.format;

                if (cap.minInterval == cap.maxInterval) {
                    config.frameInterval = cap.minInterval;
                }
                else {
                    if (DEFAULT_FPS >= cap.minInterval
                        && DEFAULT_FPS <= cap.maxInterval) {
                        config.frameInterval = DEFAULT_FPS;
                    }
                    else {
                        if (DEFAULT_FPS < cap.minInterval) {
                            config.frameInterval = cap.minInterval;
                        }
                        else {
                            config.frameInterval = cap.maxInterval;
                        }
                    }
                }
                
                m_dshow->SetCurrentVideoConfig(config);

                return accept();
            }
        }
    }
    
    return reject();
}