#include "custom_slider.h"
#include "config_helper.h"
#include <QMouseEvent>
#include <QStyleOption>
#include <QPainter>

#define MAX_SLIDER_VALUE 65536

CustomSlider::CustomSlider(QWidget* parent)
    : QSlider(parent)
{
    this->setMaximum(MAX_SLIDER_VALUE);
}

CustomSlider::~CustomSlider()
{

}

void CustomSlider::mousePressEvent(QMouseEvent* ev)
{
    QSlider::mousePressEvent(ev);

    is_pressed_ = true;
    double pos = 0;

    if (this->orientation() == Qt::Horizontal)
    {
        pos = ev->pos().x() / (double)width();
    }
    else if (this->orientation() == Qt::Vertical)
    {
        pos = ((double)height() - ev->pos().y()) / (double)height();
    }

    this->setValue(pos * (this->maximum() - this->minimum()) + this->minimum());

    emit sigCustomSliderValueChanged(this->value());
}

void CustomSlider::mouseReleaseEvent(QMouseEvent* ev)
{
    QSlider::mouseReleaseEvent(ev);

    is_pressed_ = false;
    emit sigCustomSliderValueChanged(this->value());
}

void CustomSlider::mouseMoveEvent(QMouseEvent* ev)
{
    QSlider::mouseMoveEvent(ev);
    if (is_pressed_)
    {
        double pos = 0;

        if (this->orientation() == Qt::Horizontal)
        {
            pos = ev->pos().x() / (double)width();
        }
        else if (this->orientation() == Qt::Vertical)
        {
            pos = ((double)height() - ev->pos().y()) / (double)height();
        }

        this->setValue(pos * (this->maximum() - this->minimum()) + this->minimum());

        emit sigCustomSliderValueChanged(this->value());
    }
}

void CustomSlider::paintEvent(QPaintEvent* ev)
{

    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QSlider::paintEvent(ev);
}
