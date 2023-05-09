#include "volume_button.h"
#include <QMouseEvent>
#include <QStylePainter>
#include <QStyleOptionButton>
#include <iostream>
#include <QThread>
#include <QFontDatabase>


#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QParallelAnimationGroup>

#include <QDebug>

#include "config_helper.h"

VolumeButton::VolumeButton(QWidget* parent) : QPushButton(parent)
{
	this->setAttribute(Qt::WA_StyledBackground);
	this->setFixedSize(28, 28);
	int id = QFontDatabase::addApplicationFont(":/resources/res/fonts/Font Awesome 6 Pro-Light-300.otf");
	if (id < 0)
	{
		std::cout << "VolumeButton -> load font failed" << std::endl;
	}
	else
	{
		QFont font;
		QString family = QFontDatabase::applicationFontFamilies(id).first();
		font.setFamily(family);

		font.setPixelSize(16);
		font.setPointSize(16);
		this->setFont(font);
		this->setText(QChar(0xf028));
		this->setCursor(QCursor(Qt::PointingHandCursor));
	}

	this->setStyleSheet(ConfigHelper::getInstance()->getQssString(":resources/res/css/control_bar.css"));

	graphics_effect_ = new QGraphicsOpacityEffect(this);
	graphics_effect_->setOpacity(0);

	animation_ = new QPropertyAnimation(graphics_effect_, "opacity");
	animation_->setDuration(1000);
	animation_->setStartValue(1);
	animation_->setEndValue(0);
	//animation_->setEasingCurve(QEasingCurve::BezierSpline);

	sequential_animation_group_ = new QSequentialAnimationGroup(this);
	sequential_animation_group_->addAnimation(animation_);

	parallel_animation_group_ = new QParallelAnimationGroup(this);
	parallel_animation_group_->addAnimation(sequential_animation_group_);
	parallel_animation_group_->setDirection(QAbstractAnimation::Forward);
	parallel_animation_group_->setLoopCount(1);

	QObject::connect(parallel_animation_group_, &QParallelAnimationGroup::finished,
		[=]
		{
			volume_slider_dialog_->hide();
		}
	);
}

VolumeButton::~VolumeButton()
{
}

void VolumeButton::initUi()
{

}




void VolumeButton::paintEvent(QPaintEvent* ev)
{
	QStylePainter p(this);
	QStyleOptionButton option;
	initStyleOption(&option);
	p.drawControl(QStyle::CE_PushButton, option);
}

void VolumeButton::enterEvent(QEvent* ev)
{
	// when the cursor enter the button rect, stop the animation
	parallel_animation_group_->stop();
	if (!volume_slider_dialog_)
	{
		volume_slider_dialog_ = new VolumeSliderDialog(this);
		volume_slider_dialog_->setSliderValue(volume_);
	}

	QPoint button_pos = this->mapToGlobal(QPoint(0, 0));
	QRect button_rect = this->rect();
	QRect dialog_rect = volume_slider_dialog_->rect();	//rect包含标题栏，去掉标题栏后height不变

	int x = button_pos.x() + (button_rect.width() - dialog_rect.width()) / 2;
	int y = button_pos.y() - dialog_rect.height() - 5;
	volume_slider_dialog_->move(x, y);	//相对于global

	volume_slider_dialog_->show();
	timer_id_ = startTimer(250);

	connect(volume_slider_dialog_, &VolumeSliderDialog::sigSliderValueChanged,
		[=](int value)
		{
			if (value == volume_)
			{
				return;
			}

			if (value != 0)
			{
				this->setText(QChar(0xf028));
				is_mute_ = false;
			}
			else
			{
				this->setText(QChar(0xf2e2));
				is_mute_ = true;
			}
			volume_ = value;
			emit sigVolumeValueChanged(value);
		}
	);

	//connect(this, &VolumeButton::sigVolumeMute, volume_slider_dialog_, &VolumeSliderDialog::onVolumeMute);
}

void VolumeButton::mousePressEvent(QMouseEvent* ev)
{
	if (ev->button() == Qt::LeftButton)
	{
		if (is_mute_)
		{
			is_mute_ = false;
			this->setText(QChar(0xf028));
			if (volume_slider_dialog_)
			{
				volume_slider_dialog_->setMute(false);
				//volume_slider_dialog_->setSliderValue(volume_);
				//emit sigVolumeValueChanged(volume_);
				//emit sigVolumeMute(false);
			}
		}
		else
		{
			is_mute_ = true;
			this->setText(QChar(0xf2e2));
			if (volume_slider_dialog_)
			{
				volume_slider_dialog_->setMute(true);
				//volume_slider_dialog_->setSliderValue(0);
				//emit sigVolumeValueChanged(0);
				//emit sigVolumeMute(true);
			}
		}
	}
	QPushButton::mousePressEvent(ev);
}

void VolumeButton::timerEvent(QTimerEvent* event)
{
	if ((volume_slider_dialog_ != nullptr) && (volume_slider_dialog_->isVisible()))
	{
		if (volume_slider_dialog_)
		{
			QPoint cursor_pos = QCursor::pos();

			QPoint button_pos = this->mapToGlobal(QPoint(0, 0));
			QRect button_rect = this->rect();

			QRect dialog_rect = volume_slider_dialog_->rect();
			QRect dialog_pos = volume_slider_dialog_->geometry();

			QRect area(
				dialog_pos.left(),
				dialog_pos.top(),
				dialog_rect.width(),
				button_pos.y() + button_rect.height() - dialog_pos.top()
			);

			if (!area.contains(cursor_pos))
			{
				if (parallel_animation_group_->state() == QParallelAnimationGroup::Stopped)
				{
					graphics_effect_->setOpacity(0);
					volume_slider_dialog_->setGraphicsEffect(graphics_effect_);
					parallel_animation_group_->start();
				}
			}
			else
			{
				// when the cursor enter the slider space, stop the animation
				parallel_animation_group_->stop();
				graphics_effect_->setOpacity(1);
				volume_slider_dialog_->setGraphicsEffect(graphics_effect_);
			}
		}
	}
	//else
	//{
	//	killTimer(timer_id_);
	//}
}
