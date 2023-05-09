#include "camera_preview_widget.h"
#include <QHBoxLayout>
#include <QToolButton>
#include <QFontDatabase>
#include <QSpacerItem>
#include <QDebug>
#include <QButtonGroup>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QJsonObject>
#include <QJsonDocument>
#include <QEvent>
#include <QFileDialog>
#include <QPropertyAnimation>

#include <QOpenGLWidget>

#include <QScreen>
#include <QWindow>
#include <QGuiApplication>
#include <QLabel>
#include <QGridLayout>
#include <QMessageBox>


#include "config_helper.h"
#include "volume_button.h"
#include "render_widget.h"
#include "control_bar.h"

#include "camera_menu.h"
#include "splitter_widget.h"
//#include "item_listwidget.h"

#include "media_player_gui_class.h"

CameraPreviewWidget::CameraPreviewWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	this->setAcceptDrops(true);

	initTitle();
	initToolBar();
	initContent();
	//initControlBar();

	timer_control_bar_.setInterval(1500);
	timer_mouse_detect_.setInterval(300);
	//QObject::connect(&timer_control_bar_, &QTimer::timeout, [=] {QApplication::setOverrideCursor(Qt::BlankCursor); });
	QObject::connect(&timer_mouse_detect_, &QTimer::timeout, this, &CameraPreviewWidget::onMouseDetectTimeout);
	this->setStyleSheet(ConfigHelper::getQssString(":/resources/res/css/camera_preview_widget.css"));
	this->setMouseTracking(true);

	setIgnoreKeyPress();

	this->setFocusPolicy(Qt::NoFocus);
	MediaPlayerGuiClass::getInstance().setCameraPreviewWidget(this);
}

CameraPreviewWidget::~CameraPreviewWidget()
{

	// if (canvas_scene_)
	// {
	// 	delete canvas_scene_;
	// 	canvas_scene_ = nullptr;
	// }

	// if (canvas_view_)
	// {
	// 	delete canvas_view_;
	// 	canvas_view_ = nullptr;
	// }

	if (control_bar_)
	{
		delete control_bar_;
		control_bar_ = nullptr;
	}
}

void CameraPreviewWidget::resizeEvent(QResizeEvent* ev)
{
	if (splitter_widget_)
	{
		splitter_widget_->resize(ui.wid_content->width(), ui.wid_content->height());
	}
	auto width = this->width();
	auto height = this->height();
	//ui.wid_content->setGeometry(0, 0, this->width(), this->height());
	QWidget::resizeEvent(ev);
	//canvas_view_->setGeometry(0, 0, this->width(), this->height());
}


void CameraPreviewWidget::initTitle()
{
	int id = QFontDatabase::addApplicationFont(":/resources/res/fonts/Font Awesome 6 Pro-Light-300.otf");
	if (id < 0)
		qWarning() << "FontAwesome cannot be loaded !";

	QFont font;
	QString family = QFontDatabase::applicationFontFamilies(id).first();
	font.setFamily(family);

	font.setPixelSize(16);
	font.setPointSize(16);

	layout_title_ = new QHBoxLayout;
	btn_screen_cap_ = new QPushButton;
	btn_cam_cap_ = new QPushButton;
	btn_web_cam_ = new QPushButton;
	btn_group_title_ = new QButtonGroup(this);

	lb_item_name_ = new QLabel;
	lb_item_name_->setStyleSheet("*{color: white; font-family: Consolas; font-size: 20px;}");

	//std::shared_ptr<QSpacerItem> hor_spacer1 = std::make_shared<QSpacerItem>(20, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);
	QSpacerItem* hor_spacer1 = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);
	QSpacerItem* hor_spacer2 = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	QSpacerItem* hor_spacer3 = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);

	layout_title_->setMargin(0);
	layout_title_->setSpacing(5);

	btn_screen_cap_->setCheckable(true);
	btn_screen_cap_->setFont(font);
	btn_screen_cap_->setText(QChar(0xe163));
	btn_screen_cap_->setFixedSize(28, 28);
	btn_screen_cap_->setCursor(QCursor(Qt::PointingHandCursor));

	font.setPointSize(16);
	btn_cam_cap_->setCheckable(true);
	btn_cam_cap_->setFont(font);
	btn_cam_cap_->setText(QChar(0xf030));
	btn_cam_cap_->setFixedSize(28, 28);
	btn_cam_cap_->setCursor(QCursor(Qt::PointingHandCursor));

	btn_web_cam_->setCheckable(true);
	btn_web_cam_->setFont(font);
	btn_web_cam_->setText(QChar(0xf03d));
	btn_web_cam_->setFixedSize(28, 28);
	btn_web_cam_->setCursor(QCursor(Qt::PointingHandCursor));

	layout_title_->addSpacerItem(hor_spacer1);
	layout_title_->addWidget(lb_item_name_);
	layout_title_->addSpacerItem(hor_spacer2);
	layout_title_->addWidget(btn_screen_cap_);
	layout_title_->addWidget(btn_cam_cap_);
	layout_title_->addWidget(btn_web_cam_);
	layout_title_->addSpacerItem(hor_spacer3);

	btn_group_title_->addButton(btn_screen_cap_);
	btn_group_title_->addButton(btn_cam_cap_);
	btn_group_title_->addButton(btn_web_cam_);

	ui.wid_title->setLayout(layout_title_);

	//connect(btn_group_title_, SIGNAL(buttonToggled(QAbstractButton*, bool)), this, SLOT(onTitleButtonToggled(QAbstractButton*, bool)));

	btn_screen_cap_->setChecked(true);
}

void CameraPreviewWidget::initContent()
{
	QGridLayout* layout = new QGridLayout;
	layout->setContentsMargins(0, 0, 0, 0);
	//QWidget* wid = new QWidget();
	//layout->addWidget(wid);
	//wid->setStyleSheet("background-color: black;");
	splitter_widget_ = new SplitterWidget(ui.wid_content);
	QObject::connect(this, &CameraPreviewWidget::sigResetSplitterContent, splitter_widget_, &SplitterWidget::onResetSplitterContent);
	layout->addWidget(splitter_widget_);

	ui.wid_content->setLayout(layout);

	btn_group_toolbar_->idToggled(1,true);
}

void CameraPreviewWidget::initToolBar()
{
	int id = QFontDatabase::addApplicationFont(":/resources/res/fonts/Font Awesome 6 Pro-Light-300.otf");
	if (id < 0)
		qWarning() << "FontAwesome cannot be loaded !";

	QFont font;
	QString family = QFontDatabase::applicationFontFamilies(id).first();
	font.setFamily(family);

	font.setPixelSize(16);
	font.setPointSize(16);

	layout_toolbar_ = new QHBoxLayout;
	btn_split_1_ = new QPushButton;
	btn_split_4_ = new QPushButton;
	btn_split_9_ = new QPushButton;
	btn_split_16_ = new QPushButton;
	btn_split_25_ = new QPushButton;
	btn_split_custom_ = new QPushButton;

	QPushButton* seperator = new QPushButton;
	btn_group_toolbar_ = new QButtonGroup(this);
	QSpacerItem* hor_spacer1 = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);
	QSpacerItem* hor_spacer2 = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

	layout_toolbar_->setContentsMargins(0, 0, 10, 0);
	layout_toolbar_->setMargin(0);
	layout_toolbar_->setSpacing(0);

	btn_split_1_->setCheckable(true);
	btn_split_1_->setFont(font);
	btn_split_1_->setText(QChar(0x23f9));
	btn_split_1_->setFixedSize(28, 28);
	btn_split_1_->setCursor(QCursor(Qt::PointingHandCursor));
	btn_split_1_->setToolTip(QString::fromLocal8Bit("1x1视图"));

	btn_split_4_->setCheckable(true);
	btn_split_4_->setFont(font);
	btn_split_4_->setText(QChar(0xe196));
	btn_split_4_->setFixedSize(28, 28);
	btn_split_4_->setCursor(QCursor(Qt::PointingHandCursor));
	btn_split_4_->setToolTip(QString::fromLocal8Bit("4x4视图"));

	btn_split_9_->setCheckable(true);
	btn_split_9_->setFont(font);
	btn_split_9_->setText(QChar(0xe195));
	btn_split_9_->setFixedSize(28, 28);
	btn_split_9_->setCursor(QCursor(Qt::PointingHandCursor));
	btn_split_9_->setToolTip(QString::fromLocal8Bit("9x9视图"));

	btn_split_16_->setCheckable(true);
	btn_split_16_->setFont(font);
	btn_split_16_->setText(QChar(0xe198));
	btn_split_16_->setFixedSize(28, 28);
	btn_split_16_->setCursor(QCursor(Qt::PointingHandCursor));
	btn_split_16_->setToolTip(QString::fromLocal8Bit("16x16视图"));

	btn_split_25_->setCheckable(true);
	btn_split_25_->setFont(font);
	btn_split_25_->setText(QChar(0xe199));
	btn_split_25_->setFixedSize(28, 28);
	btn_split_25_->setCursor(QCursor(Qt::PointingHandCursor));
	btn_split_25_->setToolTip(QString::fromLocal8Bit("25x25视图"));

	btn_split_custom_->setCheckable(true);
	btn_split_custom_->setFont(font);
	btn_split_custom_->setText(QChar(0xe3c1));
	btn_split_custom_->setFixedSize(28, 28);
	btn_split_custom_->setCursor(QCursor(Qt::PointingHandCursor));
	btn_split_custom_->setToolTip(QString::fromLocal8Bit("自定义视图"));

	seperator->setEnabled(false);
	seperator->setMinimumWidth(20);
	seperator->setMaximumWidth(20);
	seperator->setFont(font);
	seperator ->setText(QChar(0x007c));
	seperator->setStyleSheet("color:dimgray");


	layout_toolbar_->addSpacerItem(hor_spacer1);
	layout_toolbar_->addWidget(btn_split_1_);
	layout_toolbar_->addWidget(btn_split_4_);
	layout_toolbar_->addWidget(btn_split_9_);
	layout_toolbar_->addWidget(btn_split_16_);
	layout_toolbar_->addWidget(btn_split_25_);
	layout_toolbar_->addWidget(btn_split_custom_);
	layout_toolbar_->addWidget(seperator);
	layout_toolbar_->addSpacerItem(hor_spacer2);

	btn_group_toolbar_->addButton(btn_split_1_);
	btn_group_toolbar_->addButton(btn_split_4_);
	btn_group_toolbar_->addButton(btn_split_9_);
	btn_group_toolbar_->addButton(btn_split_16_);
	btn_group_toolbar_->addButton(btn_split_25_);
	btn_group_toolbar_->addButton(btn_split_custom_);
	
	btn_group_toolbar_->setId(btn_split_1_, 1);
	btn_group_toolbar_->setId(btn_split_4_, 4);
	btn_group_toolbar_->setId(btn_split_9_, 9);
	btn_group_toolbar_->setId(btn_split_16_, 16);
	btn_group_toolbar_->setId(btn_split_25_, 25);
	btn_group_toolbar_->setId(btn_split_custom_, 0);

	ui.wid_toolbar->setLayout(layout_toolbar_);
	ui.wid_toolbar->setFixedHeight(35);

	QObject::connect(btn_group_toolbar_, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(onToolBarButtonToggled(QAbstractButton*)));

	btn_split_1_->setChecked(true);
	//last_split_id_ = btn_group_toolbar_->checkedId();

}

void CameraPreviewWidget::initControlBar()
{
	control_bar_ = new ControlBar();
	layout_toolbar_ = new QHBoxLayout;
	layout_toolbar_->setContentsMargins(0, 0, 0, 0);
	layout_toolbar_->setMargin(0);
	layout_toolbar_->setSpacing(0);
	layout_toolbar_->addWidget(control_bar_, 1);
	ui.wid_toolbar->setLayout(layout_toolbar_);
	ui.wid_toolbar->setFixedHeight(60);
	ui.wid_toolbar->setWindowFlags(Qt::WindowStaysOnTopHint);

	ani_control_bar_show_ = new QPropertyAnimation(ui.wid_toolbar, "geometry");
	ani_control_bar_hide_ = new QPropertyAnimation(ui.wid_toolbar, "geometry");

	QObject::connect(control_bar_, &ControlBar::sigOpenMediaFile, [=](QJsonObject& info)
		{
			auto name = info.find("name").value().toString();
			auto url = info.find("url").value().toString();
			openning_filename_ = name;
			render_widget_->openMediaFile(url);
			emit sigAddLocalFileItem(info);
		}
	);

	QObject::connect(control_bar_, &ControlBar::sigPlaying, [=](bool status)
		{
			is_playing_ = !status;
			render_widget_->setMediaPause(is_playing_);
		}
	);

	QObject::connect(control_bar_, &ControlBar::sigStopPlaying, [=]()
		{
			is_playing_ = false;
			render_widget_->setMediaStop();
			control_bar_->reset();
		}
	);

	QObject::connect(control_bar_, &ControlBar::sigControlSliderChanged, [=](double value)
		{
			//if (!is_playing_)
			//{
			//	return;
			//}
			render_widget_->setSeekPos(value);
		}
	);

	QObject::connect(control_bar_, &ControlBar::sigSoundVolumeChanged, [=](int value)
		{
			//if (!is_playing_)
			//{
			//	return;
			//}
			render_widget_->setSoundVolume(value);
		}
	);

	QObject::connect(control_bar_, &ControlBar::sigNextClip, [=]()
		{
			QString cur_name = lb_item_name_->text();
			auto it = playlist_.find(cur_name);
			if (++it == playlist_.end())
			{
				it = playlist_.begin();
			}
			if ((*it).isEmpty())
			{
				return;
			}
			auto name = it.key();
			auto url = it.value();
			openning_filename_ = name;
			render_widget_->openMediaFile(url);
			emit sigPlayNextFile();
		}
	);

	QObject::connect(control_bar_, &ControlBar::sigPreviousClip, [=]()
		{
			QString cur_name = lb_item_name_->text();
			auto it = playlist_.find(cur_name);
			if (it-- == playlist_.begin())
			{
				it = playlist_.end();
				it--;
			}
			if ((*it).isEmpty())
			{
				return;
			}
			auto name = it.key();
			auto url = it.value();
			openning_filename_ = name;
			render_widget_->openMediaFile(url);
			emit sigPlayPrevFile();
		}
	);

	QObject::connect(control_bar_, &ControlBar::sigFullscreen, this, &CameraPreviewWidget::onShowFullScreen);
}

void CameraPreviewWidget::onPanelChanged(bool is_checked)
{

}

void CameraPreviewWidget::onTitleButtonToggled(QAbstractButton* bt, bool checked)
{

	if (dynamic_cast<QPushButton*>(bt) == btn_screen_cap_ || dynamic_cast<QPushButton*>(bt) == btn_cam_cap_)
	{
		if (checked)
		{
			btn_split_1_->setEnabled(false);
			btn_split_4_->setEnabled(false);
			btn_split_9_->setEnabled(false);
			btn_split_16_->setEnabled(false);
			btn_split_25_->setEnabled(false);
			btn_split_custom_->setEnabled(false);

			btn_group_toolbar_->button(last_split_id_)->setChecked(false);
			btn_group_toolbar_->button(last_split_id_)->repaint();

			btn_start_push_->setEnabled(true);
			btn_push_setting_->setEnabled(true);
			btn_push_info_->setEnabled(true);
		}
	}
	else if (dynamic_cast<QPushButton*>(bt) == btn_web_cam_)
	{
		if (checked)
		{
			btn_split_1_->setEnabled(true);
			btn_split_4_->setEnabled(true);
			btn_split_9_->setEnabled(true);
			btn_split_16_->setEnabled(true);
			btn_split_25_->setEnabled(true);
			btn_split_custom_->setEnabled(true);

			btn_group_toolbar_->button(last_split_id_)->setChecked(true);

			btn_start_push_->setEnabled(false);
			btn_push_setting_->setEnabled(false);
			btn_push_info_->setEnabled(false);
		}
	}
	else
	{

	}
}

void CameraPreviewWidget::onToolBarButtonToggled(QAbstractButton* bt)
{
	bool reset = false;
	int cur_id = btn_group_toolbar_->checkedId();

	int id1 = btn_group_toolbar_->id(btn_split_1_);
	int id4 = btn_group_toolbar_->id(btn_split_4_);
	int id9 = btn_group_toolbar_->id(btn_split_9_);
	int id16 = btn_group_toolbar_->id(btn_split_16_);
	int id25 = btn_group_toolbar_->id(btn_split_25_);
	int cus_id = btn_group_toolbar_->id(btn_split_custom_);
	if (is_custom_layout_ && cur_id != btn_group_toolbar_->id(btn_split_custom_))
	{
		is_custom_layout_ = false;
		reset = true;
	}
	else if (!is_custom_layout_ && cur_id == btn_group_toolbar_->id(btn_split_custom_))
	{
		is_custom_layout_ = true;
		reset = true;
	}

	if (reset)
	{
		auto reply = QMessageBox::warning(this, "warning", QString::fromLocal8Bit("需要重置视图"), QMessageBox::Cancel | QMessageBox::Ok);
		if (reply == QMessageBox::Ok)
		{
			emit sigResetSplitterContent(cur_id);
		}
		else
		{
			is_custom_layout_ = !is_custom_layout_;
		}
	}
	else if (!is_custom_layout_)
	{
		emit sigResetSplitterContent(cur_id);
	}

	last_split_id_ = cur_id;
}

void CameraPreviewWidget::onSoundVolumeValueChanged(int value)
{
	QString str = QString::fromLocal8Bit("音量 ");
	QString str2;
	str2.sprintf("%d %%", value);

	btn_volume_->setToolTip(str + str2);
}

bool CameraPreviewWidget::eventFilter(QObject* watched, QEvent* ev)
{
	if (ev->type() == QEvent::KeyPress)
	{
		ev->ignore();
		return true;
	}
	// CanvasView* window = qobject_cast<CanvasView*>(watched);
	// if (window == nullptr)
	// 	return false;
	//qDebug() << event->type();
	//switch (event->type())
	//{
	//	case QEvent::Drop:
	//	{
	//		this->dropEvent((QDropEvent*)event);
	//		return true;
	//		break;
	//	}
	//	case QEvent::DragEnter:
	//	{
	//		this->dragEnterEvent((QDragEnterEvent*)event);
	//		return true;
	//	}
	//	case QEvent::MouseButtonPress:
	//	{
	//		qDebug() << "mouse button press";
	//		return true;
	//	}
	//	case QEvent::MouseMove:
	//	{
	//		qDebug() << "mouse move";
	//		return true;
	//	}
	//	default:
	//	{
	//		return false;
	//	}
	//}

	return QWidget::eventFilter(watched, ev);
}

void CameraPreviewWidget::dragEnterEvent(QDragEnterEvent* ev)
{
	ev->acceptProposedAction();
}

void CameraPreviewWidget::dropEvent(QDropEvent* ev)
{
	int itemIndex = ((ItemListWidget*)ev->source())->currentIndex().row();
	CameraMenu::ItemListType item_type = (CameraMenu::ItemListType)((ItemListWidget*)ev->source())->item_type_;

	QByteArray arr = ev->mimeData()->data("application/json");
	qDebug() << arr;
	QJsonParseError err;
	QJsonDocument doc(QJsonDocument::fromJson(arr, &err));
	QJsonObject obj = doc.object();

	//QString name = obj.find("name").value().toString();
	//qDebug() << "get name : " << name;

	//this->name_ = obj.find("name").value().toString();
	//this->url_ = obj.find("url").value().toString();
	//this->sub_url_ = obj.find("sub_url").value().toString();
	//this->server_url_ = obj.find("server_url").value().toString();
	//this->item_type_ = itemType;

	auto name = obj.find("name").value().toString();
	auto str_name = name.toStdString();
	auto url = obj.find("url").value().toString();
	auto sub_url = obj.find("sub_url").value().toString().toStdString();
	auto server_url = obj.find("server_url").value().toString().toStdString();

	openning_filename_ = name;
	//emit SigConfigAndStartHandler();
	render_widget_->openMediaFile(url);

	ev->setDropAction(Qt::CopyAction);
	//ev->setDropAction(Qt::MoveAction);
	ev->accept();
}

void CameraPreviewWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
	//if (this->isFullScreen())
	//{
	//	this->showNormal();
	//	emit sigDisplayShowFullscreen(false);
	//	qDebug() << "display show normal";
	//}
	//else
	//{
	//	//this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
	//	//auto screen = QGuiApplication::primaryScreen();
	//	//QRect screen_rect = screen->geometry();
	//	//this->setGeometry(0, 0, screen_rect.width(), screen_rect.height());
	//	this->showFullScreen();
	//	emit sigDisplayShowFullscreen(true);
	//	qDebug() << "display show full";
	//}
	return QWidget::mouseDoubleClickEvent(event);
}

void CameraPreviewWidget::keyPressEvent(QKeyEvent* ev)
{
	if (ev->key() == Qt::Key_Q)
	{
		qDebug() << "q key pressed";
	}
	return QWidget::keyPressEvent(ev);
}



void CameraPreviewWidget::onMouseDetectTimeout()
{
	if (is_fullscreen_)
	{
		if (rect_control_bar_show_.contains(cursor().pos()))
		{
			//this->setCursor(QCursor(Qt::ArrowCursor));

			if (ani_control_bar_hide_->state() == QAbstractAnimation::Running)
			{
				ui.wid_toolbar->setGeometry(rect_control_bar_show_);
				ani_control_bar_hide_->stop();
			}

			if (ui.wid_toolbar->geometry().contains(cursor().pos()))
			{
				is_control_bar_show_ = true;
			}
			else
			{
				ui.wid_toolbar->raise();
				ani_control_bar_show_->start();
				ani_control_bar_hide_->stop();
				is_control_bar_show_ = true;
			}
		}
		else
		{
			if (is_control_bar_show_)
			{
				is_control_bar_show_ = false;
				
				timer_control_bar_.singleShot(2000, this, [=] {ani_control_bar_hide_->start(); });

			}
		}
	}
}

void CameraPreviewWidget::fullscreenDisplay(bool status)
{
	if (status)
	{
		QScreen* current_screen = screen();

		is_fullscreen_ = true;
		this->showFullScreen();
		ui.wid_title->hide();

		//render_widget_->setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
		//render_widget_->showFullScreen();
		//render_widget_->windowHandle()->setScreen(current_screen);


		QRect screen_rect = current_screen->geometry();

		render_widget_->setGeometry(screen_rect);

		int control_bar_height = control_bar_->height();
		//int x = render_widget_->x();
		//rect_control_bar_show_ = QRect(x, screen_rect.height() - control_bar_height, screen_rect.width(), control_bar_height);
		//rect_control_bar_hide_ = QRect(x, screen_rect.height(), screen_rect.width(), control_bar_height);

		int x = screen_rect.x();
		rect_control_bar_show_ = QRect(x, screen_rect.height() - control_bar_height, screen_rect.width(), control_bar_height);
		rect_control_bar_hide_ = QRect(x, screen_rect.height(), screen_rect.width(), control_bar_height);

		ani_control_bar_show_->setStartValue(rect_control_bar_hide_);
		ani_control_bar_show_->setEndValue(rect_control_bar_show_);
		ani_control_bar_show_->setDuration(500);

		ani_control_bar_hide_->setStartValue(rect_control_bar_show_);
		ani_control_bar_hide_->setEndValue(rect_control_bar_hide_);
		ani_control_bar_hide_->setDuration(1000);

		ui.wid_toolbar->setGeometry(rect_control_bar_show_);
		//ui.wid_toolbar->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
		//ui.wid_toolbar->setWindowFlags(Qt::FramelessWindowHint | Qt::Widget | Qt::WindowStaysOnTopHint);
		//ui.wid_toolbar->setWindowFlags(Qt::FramelessWindowHint | Qt::Window | Qt::WindowStaysOnTopHint);
		//ui.wid_toolbar->setWindowFlags(Qt::FramelessWindowHint | Qt::Window | Qt::Tool);
		ui.wid_toolbar->setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
		ui.wid_toolbar->raise();
		//ui.wid_toolbar->setFocus();
		ui.wid_toolbar->setWindowOpacity(0.5);
		ui.wid_toolbar->showNormal();
		//render_widget_->showNormal();
		//ui.wid_toolbar->windowHandle()->setScreen(current_screen);

		timer_control_bar_.start();
		timer_mouse_detect_.start();
		//this->setFocus();
		emit sigDisplayShowFullscreen(true);
		
	}
	else
	{
		is_fullscreen_ = false;
		this->showNormal();
		ani_control_bar_show_->stop();
		ani_control_bar_hide_->stop();

		ui.wid_title->show();

		ui.wid_toolbar->show();
		ui.wid_toolbar->setWindowOpacity(1);
		ui.wid_toolbar->setWindowFlag(Qt::Window, false);
		//render_widget_->setWindowFlag(Qt::Window, false);
		ui.wid_toolbar->setWindowFlag(Qt::SubWindow, true);
		//render_widget_->setWindowFlag(Qt::SubWindow, true);

		ui.wid_toolbar->showNormal();
		//render_widget_->showNormal();
		emit sigDisplayShowFullscreen(false);
		timer_control_bar_.stop();
		timer_mouse_detect_.stop();

		//this->setFocus();
	}

}


void CameraPreviewWidget::onShowFullScreen(bool status)
{
	fullscreenDisplay(status);
}

void CameraPreviewWidget::onListItemDoubleClicked(QJsonObject& info)
{
	openning_filename_ = info.find("name").value().toString();
	auto url = info.find("url").value().toString();
	render_widget_->openMediaFile(url);
}

void CameraPreviewWidget::onItemListUpdate(int type, QJsonObject& obj)
{
	if (type != 1) return;
	playlist_obj_ = QJsonObject(obj);
	playlist_.clear();
	file_list_.clear();

	QStringList filenames = obj.keys();
	for (int i = 0; i < filenames.size(); i++)
	{
		if (obj.contains(filenames[i]))
		{
			auto value = obj.value(filenames[i]);
			auto info = value.toObject();
			auto url = info.find("url").value().toString();
			file_list_.push_back(filenames[i]);
			playlist_.insert(filenames[i], url);
		}

	}
}

void CameraPreviewWidget::setIgnoreKeyPress()
{
	ui.wid_title->installEventFilter(this);
	ui.wid_content->installEventFilter(this);
	ui.wid_toolbar->installEventFilter(this);

	btn_screen_cap_->installEventFilter(this);
	btn_cam_cap_->installEventFilter(this);
	btn_web_cam_->installEventFilter(this);
	btn_group_title_->installEventFilter(this);
}
