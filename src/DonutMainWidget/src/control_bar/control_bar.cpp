#include "control_bar.h"

#include "media_player_gui_class.h"
#include "custom_slider.h"
#include "volume_button.h"
#include "config_helper.h"
#include "dialog_base.h"
#include "main_widget.h"


#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFontDatabase>
#include <QMessageBox>
#include <QCursor>
#include <QTimeEdit>
#include <QStyleOption>
#include <QPainter>
#include <QLabel>
#include <QJsonObject>
#include <QJsonDocument>

#define MAX_SLIDER_VALUE 65536

ControlBar::ControlBar(QWidget* parent) : QWidget(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinMaxButtonsHint);
    initUI();

    setIgnoreKeyPress();

    MediaPlayerGuiClass::getInstance().setControlBar(this);
}

ControlBar::~ControlBar()
{
    if (play_slider_)
    {
        delete play_slider_;
        play_slider_ = nullptr;
    }

    if (volume_button_)
    {
        delete volume_button_;
        volume_button_ = nullptr;
    }

    if (btn_play_or_pause_)
    {
        delete btn_play_or_pause_;
        btn_play_or_pause_ = nullptr;
    }

    if (btn_stop_)
    {
        delete btn_stop_;
        btn_stop_ = nullptr;
    }

    if (btn_next_clip_)
    {
        delete btn_next_clip_;
        btn_next_clip_ = nullptr;
    }

    if (btn_previous_clip_)
    {
        delete btn_previous_clip_;
        btn_previous_clip_ = nullptr;
    }

    if (btn_fullscreen_)
    {
        delete btn_fullscreen_;
        btn_fullscreen_ = nullptr;
    }

    if (btn_open_file_)
    {
        delete btn_open_file_;
        btn_open_file_ = nullptr;
    }

    if (t_edit_total_)
    {
        delete t_edit_total_;
        t_edit_total_ = nullptr;
    }

    if (t_edit_current_)
    {
        delete t_edit_current_;
        t_edit_current_ = nullptr;
    }
}

void ControlBar::initUI()
{
    setStyleSheet(ConfigHelper::getQssString(":/resources/res/css/control_bar.css"));
    int id = QFontDatabase::addApplicationFont(":/resources/res/fonts/Font Awesome 6 Pro-Light-300.otf");
    if (id < 0)
    {
        qWarning() << "ControlBar : FontAwesome cannot be loaded !";
    }
    else
    {
        QFont font;
        QString family = QFontDatabase::applicationFontFamilies(id).first();
        font.setFamily(family);
        font.setPixelSize(16);
        font.setPointSize(16);

        play_slider_ = new CustomSlider;
        volume_button_ = new VolumeButton;
        btn_next_clip_ = new QPushButton;
        btn_play_or_pause_ = new QPushButton;
        btn_stop_ = new QPushButton;
        btn_previous_clip_ = new QPushButton;
        btn_fullscreen_ = new QPushButton;
        btn_open_file_ = new QPushButton;
        t_edit_current_ = new QTimeEdit();
        t_edit_total_ = new QTimeEdit();
        QLabel* lab = new QLabel();

        QHBoxLayout* h_layout = new QHBoxLayout();
        QVBoxLayout* v_layout = new QVBoxLayout();

        play_slider_->setStyleSheet(ConfigHelper::getQssString(":/resources/res/css/control_bar.css"));
        play_slider_->setOrientation(Qt::Horizontal);
        play_slider_->setFixedHeight(16);
        QObject::connect(play_slider_, &CustomSlider::sigCustomSliderValueChanged, 
            [=] 
            {
                //double percent_value = play_slider_->value() * 1.0 / play_slider_->maximum();
                //emit sigControlSliderChanged(percent_value);
                int max = play_slider_->maximum();
                int value = play_slider_->value();
                emit sigControlSliderChanged(value);
            }
        );


        QObject::connect(volume_button_, &VolumeButton::sigVolumeValueChanged, [=](int value)
            {
                volume_ = value;
                emit sigSoundVolumeChanged(value);
            }
        );

        QObject::connect(volume_button_, &VolumeButton::sigVolumeMute, [=](bool mute)
            {
                if (mute)
                {
                    emit sigSoundVolumeChanged(0);
                }
                else
                {
                    emit sigSoundVolumeChanged(volume_);
                }
            }
        );


        btn_play_or_pause_->setCheckable(true);
        btn_play_or_pause_->setFont(font);
        btn_play_or_pause_->setText(QChar(0x25b6));
        btn_play_or_pause_->setFixedSize(28, 28);
        btn_play_or_pause_->setCursor(QCursor(Qt::PointingHandCursor));
        btn_play_or_pause_->setToolTip(QString::fromLocal8Bit("play"));
        connect(btn_play_or_pause_, &QPushButton::clicked, this,
            [=]
            {
                if (is_stoped_)
                {
                    btn_play_or_pause_->setChecked(false);
                    return;
                }
                if (btn_play_or_pause_->isChecked())
                {
                    btn_play_or_pause_->setText(QChar(0x23f8));
                    btn_play_or_pause_->setToolTip(QString::fromLocal8Bit("pause"));
                    is_playing_ = true;
                    emit sigPlaying(true);
                }
                else
                {
                    btn_play_or_pause_->setText(QChar(0x25b6));
                    btn_play_or_pause_->setToolTip(QString::fromLocal8Bit("play"));
                    is_playing_ = false;
                    emit sigPlaying(false);
                }
            }
        );

        btn_stop_->setFont(font);
        btn_stop_->setText(QChar(0x23f9));
        btn_stop_->setFixedSize(28, 28);
        btn_stop_->setCursor(QCursor(Qt::PointingHandCursor));
        btn_stop_->setToolTip(QString::fromLocal8Bit("stop"));
        connect(btn_stop_, &QPushButton::clicked, this,
            [=]
            {
                emit sigStopPlaying();
                is_playing_ = false;
                is_stoped_ = true;
            }
        );


        btn_next_clip_->setFont(font);
        btn_next_clip_->setText(QChar(0xf051));
        btn_next_clip_->setFixedSize(28, 28);
        btn_next_clip_->setCursor(QCursor(Qt::PointingHandCursor));
        btn_next_clip_->setToolTip(QString::fromLocal8Bit("next"));
        connect(btn_next_clip_, &QPushButton::clicked, this,
            [=]
            {
                emit sigNextClip();
            }
        );


        btn_previous_clip_->setFont(font);
        btn_previous_clip_->setText(QChar(0xf048));
        btn_previous_clip_->setFixedSize(28, 28);
        btn_previous_clip_->setCursor(QCursor(Qt::PointingHandCursor));
        btn_previous_clip_->setToolTip(QString::fromLocal8Bit("previous"));
        connect(btn_previous_clip_, &QPushButton::clicked, this,
            [=]
            {
                emit sigPreviousClip();
            }
        );


        btn_fullscreen_->setCheckable(true);
        btn_fullscreen_->setFont(font);
        btn_fullscreen_->setText(QChar(0xf065));
        btn_fullscreen_->setFixedSize(28, 28);
        btn_fullscreen_->setCursor(QCursor(Qt::PointingHandCursor));
        btn_fullscreen_->setToolTip(QString::fromLocal8Bit("fullscreen"));
        connect(btn_fullscreen_, &QPushButton::clicked, this,
            [=]
            {
                if (is_stoped_)
                {
                    btn_fullscreen_->setChecked(false);
                    btn_fullscreen_->setText(QChar(0xf065));
                    is_fullscreen_ = false;
                    emit sigFullscreen(false);
                    return;
                }
                if (btn_fullscreen_->isChecked())
                {
                    btn_fullscreen_->setText(QChar(0xf066));
                    btn_fullscreen_->setToolTip(QString::fromLocal8Bit("fullscreen"));
                    is_fullscreen_ = true;
                    emit sigFullscreen(true);
                }
                else
                {
                    btn_fullscreen_->setText(QChar(0xf065));
                    btn_fullscreen_->setToolTip(QString::fromLocal8Bit("normal"));
                    is_fullscreen_ = false;
                    emit sigFullscreen(false);
                }
            }
        );


        btn_open_file_->setFont(font);
        btn_open_file_->setText(QChar(0xf07c));
        btn_open_file_->setFixedSize(28, 28);
        btn_open_file_->setCursor(QCursor(Qt::PointingHandCursor));
        btn_open_file_->setToolTip(QString::fromLocal8Bit("open"));
        connect(btn_open_file_, &QPushButton::clicked, this,
            [=]
            {
                MediaFileDialog* dialog = new MediaFileDialog();
                for (;;)
                {
                    if (dialog->exec() == QDialog::Accepted)
                    {
                        QJsonObject info;
                        auto name = dialog->getFileName();
                        if (name.isEmpty())
                        {
                            QMessageBox::warning(this, "warning", "please set a filename");
                            continue;
                        }
                        info.insert("name", name);
                        info.insert("url", dialog->getFilePath());
                        info.insert("item_type",1);
                        emit sigOpenMediaFile(info);
                        break;
                    }
                    else if (dialog->close())
                    {
                        break;
                    }
                }
            }
        );


        t_edit_current_->setReadOnly(true);
        t_edit_current_->setDisplayFormat("HH:mm:ss");
        t_edit_current_->setEnabled(false); // disable text selection
        t_edit_current_->setButtonSymbols(QAbstractSpinBox::ButtonSymbols::NoButtons);


        t_edit_total_->setReadOnly(true);
        t_edit_total_->setDisplayFormat("HH:mm:ss");
        t_edit_total_->setEnabled(false);
        t_edit_total_->setButtonSymbols(QAbstractSpinBox::ButtonSymbols::NoButtons);


        lab->setText("/");
        lab->setFixedWidth(8);

        h_layout->addWidget(btn_previous_clip_);
        h_layout->addWidget(btn_play_or_pause_);
        h_layout->addWidget(btn_next_clip_);
        h_layout->addWidget(btn_stop_);

        h_layout->addStretch();
        h_layout->addWidget(t_edit_current_);
        h_layout->addWidget(lab);
        h_layout->addWidget(t_edit_total_);
        h_layout->addStretch();

        h_layout->addWidget(btn_open_file_);
        h_layout->addWidget(btn_fullscreen_);
        h_layout->addWidget(volume_button_);
                
        v_layout->addWidget(play_slider_,1);
        //v_layout->addLayout(h_layout2);
        v_layout->addLayout(h_layout);

        v_layout->setContentsMargins(QMargins(2, 0, 2, 2));
        this->setLayout(v_layout);
    }

}

void ControlBar::setTotalDuration(QTime time)
{
    //total_duration_ = sec;

    //int hour, minues, second;
    //hour = sec / 3600;
    //minues = (sec % 3600) / 60;
    //second = (sec % 60);
    //QTime time(hour, minues, second);

    total_duration_ = time.hour() * 3600 + time.minute() * 60 + time.second();
    t_edit_total_->setTime(time);
    play_slider_->setMaximum(total_duration_);
}

void ControlBar::setCurrentDuration(QTime time)
{
    //int hour, minues, second;
    //hour = sec / 3600;
    //minues = (sec % 3600) / 60;
    //second = (sec % 60);
    //QTime time(hour, minues, second);
    //t_edit_current_->setTime(time);

    t_edit_current_->setTime(time);
    int seconds = time.hour() * 3600 + time.minute() * 60 + time.second();

    play_slider_->setValue(seconds * 1.0 / total_duration_ * play_slider_->maximum());
}

void ControlBar::setPlaying(bool status)
{
    if (is_playing_ == status)
    {
        return;
    }
    is_playing_ = status;
    is_stoped_ = false;
    btn_play_or_pause_->click();
}

void ControlBar::setFullscreen(bool status)
{
    is_fullscreen_ = true;
    btn_fullscreen_->click();
}

void ControlBar::setStop()
{
    is_playing_ = false;
    is_stoped_ = true;
    btn_stop_->click();
}

void ControlBar::setSoundSliderValue(int value)
{
    volume_button_->setVolume(value);
}

void ControlBar::reset()
{
    is_playing_ = false;
    is_stoped_ = true;
    is_fullscreen_ = false;
    btn_play_or_pause_->setChecked(false);
    btn_play_or_pause_->setText(QChar(0x25b6));
    btn_fullscreen_->setChecked(false);
    t_edit_total_->setTime(QTime(0, 0, 0, 0));
    t_edit_current_->setTime(QTime(0, 0, 0, 0));
    play_slider_->setValue(0);
}


void ControlBar::paintEvent(QPaintEvent* ev)
{

    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QWidget::paintEvent(ev);
}

bool ControlBar::eventFilter(QObject* watched, QEvent* ev)
{
    if (ev->type() == QEvent::KeyPress)
    {
        ev->ignore();
        return true;
    }
    return false;
}

void ControlBar::setIgnoreKeyPress()
{
    play_slider_->installEventFilter(this);
    volume_button_->installEventFilter(this);
    btn_play_or_pause_->installEventFilter(this);
    btn_stop_->installEventFilter(this);
    btn_next_clip_->installEventFilter(this);
    btn_previous_clip_->installEventFilter(this);
    btn_fullscreen_->installEventFilter(this);
    btn_open_file_->installEventFilter(this);
    t_edit_total_->installEventFilter(this);
    t_edit_current_->installEventFilter(this);
}

void ControlBar::onSeekForwardByKeyboard()
{
    int value = play_slider_->value();
    int seek = value + 5;
    play_slider_->setValue(seek);
    emit sigControlSliderChanged(seek);
}

void ControlBar::onSeekBackwardByKeyboard()
{
    //play_slider_->setValue(play_slider_->value() - play_slider_->maximum() / 30);
    int value = play_slider_->value();
    int seek = value - 5;
    play_slider_->setValue(seek);
    emit sigControlSliderChanged(seek);
}

void ControlBar::onVolumeAddByKeyboard()
{
    volume_button_->setVolume(volume_ + 5);
}

void ControlBar::onVolumeSubByKeyboard()
{
    volume_button_->setVolume(volume_ - 5);
}

void ControlBar::onPauseByKeyboard()
{
    btn_play_or_pause_->click();
}

void ControlBar::onMuteByKeyboard()
{
    
    if (is_mute_)
    {
        is_mute_ = false;
        volume_button_->setMute(false);
    }
    else
    {
        is_mute_ = true;
        volume_button_->setMute(true);
    }
}
