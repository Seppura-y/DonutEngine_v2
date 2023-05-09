#include "volume_slider_dialog.h"
#include <QVBoxLayout>
#include <QEvent>
#include <windows.h>

#include "config_helper.h"
#include "custom_slider.h"

//#pragma comment(lib, "user32.lib")

VolumeSliderDialog::VolumeSliderDialog(QWidget* parent)
	: QDialog(parent)
{
	this->setFixedSize(40, 200);
    this->setAttribute(Qt::WA_TranslucentBackground);

	QVBoxLayout* v_layout = new QVBoxLayout(this);
	slider_ = new CustomSlider(this);
	slider_->setOrientation(Qt::Vertical);
    slider_->setMaximum(100);
    slider_->setStyleSheet(ConfigHelper::getInstance()->getQssString(":resources/res/css/control_bar.css"));
    v_layout->addWidget(slider_);

	setFixedSize(40, 120);
    setWindowFlags(Qt::FramelessWindowHint | Qt::ToolTip); // Qt::ToolTip 悬浮显示效果
    setStyleSheet("QDialog{background-color: rgba(54, 54, 54, 0.5);}");

	connect(slider_, &CustomSlider::sigCustomSliderValueChanged,
        [=](int value)
        {
            if (flip_)
            {
                return;
            }
            volume_ = value;
		    emit sigSliderValueChanged(value);
		}
    );

    connect(slider_, &CustomSlider::sliderPressed,
        [=]()
        {
            //flip_ = false;
            volume_ = slider_->value();
            emit sigSliderValueChanged(volume_);
        }
    );

    connect(slider_, &CustomSlider::valueChanged,
        [=]()
        {
            if (flip_)
            {
                emit sigSliderValueChanged(0);
                return;
            }
            //flip_ = false;
            volume_ = slider_->value();
            emit sigSliderValueChanged(volume_);
        }
    );
}

VolumeSliderDialog::~VolumeSliderDialog()
{
}

void VolumeSliderDialog::setSliderValue(int value)
{
    //volume_ = value;
    slider_->setValue(value);
}

void VolumeSliderDialog::setMute(bool status)
{
    if (status)
    {
        flip_ = true;
        setSliderValue(0);
    }
    else
    {
        flip_ = false;
        setSliderValue(volume_);
    }
}

bool VolumeSliderDialog::event(QEvent* event)
{
    //static bool class_amended = false;

    //if (event->type() == QEvent::WinIdChange)
    //{
    //    HWND hwnd = (HWND)winId();

    //    if (class_amended == false)
    //    {
    //        class_amended = true;
    //        DWORD class_style = ::GetClassLong(hwnd, GCL_STYLE);
    //        class_style &= ~CS_DROPSHADOW;
    //        ::SetClassLong(hwnd, GCL_STYLE, class_style);
    //    }
    //}

	return QWidget::event(event);
}

void VolumeSliderDialog::onVolumeMute(bool status)
{
    //if (status)
    //{
    //    flip_ = true;
    //    setSliderValue(0);
    //    //slider_->setValue(0);
    //}
    //else
    //{
    //    flip_ = false;
    //    setSliderValue(volume_);
    //    //slider_->setValue(volume_);
    //}
}