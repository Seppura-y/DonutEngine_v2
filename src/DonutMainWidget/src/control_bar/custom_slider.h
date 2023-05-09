#ifndef CUSTOM_SLIDER_H
#define CUSTOM_SLIDER_H
#include <QSlider>

class CustomSlider : public QSlider
{
    Q_OBJECT

public:
    CustomSlider(QWidget* parent = nullptr);
    ~CustomSlider();

protected:
    void mousePressEvent(QMouseEvent* ev) override;
    void mouseReleaseEvent(QMouseEvent* ev) override;
    void mouseMoveEvent(QMouseEvent* ev) override;
    void paintEvent(QPaintEvent* ev) override;
signals:
    void sigCustomSliderValueChanged(int value);

private:
    bool is_pressed_ = false;
};

#endif