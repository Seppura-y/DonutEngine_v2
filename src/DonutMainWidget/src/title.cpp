#include "title.h"
#include "ui_title.h"
#include "config_helper.h"
#include "font_icon.h"
#include "menu_bar.h"
//#include "media_manager_gui_class.h"
//#include "media_manager.h"
#include "main_widget.h"

#include <QDebug>
#include <QFontDatabase>
#include <QStyleOption>
#include <QPainter>
#include <QHBoxLayout>
#include <QSpacerItem>

#include <iostream>

using namespace std;
Title::Title(QWidget* parent) : QWidget(parent)
{
	ui = new Ui::Title();
	ui->setupUi(this);

	ui->pb_close->setCursor(QCursor(Qt::PointingHandCursor));
	ui->pb_max->setCursor(QCursor(Qt::PointingHandCursor));
	ui->pb_mini->setCursor(QCursor(Qt::PointingHandCursor));
	ui->pb_title->setCursor(QCursor(Qt::PointingHandCursor));

	// raise to cover the animation button
	ui->pb_close->raise();
	ui->pb_max->raise();
	ui->pb_sep->raise();
	ui->pb_mini->raise();
	ui->pb_title->raise();

	ui->pb_close->installEventFilter(this);
	ui->pb_max->installEventFilter(this);
	ui->pb_sep->installEventFilter(this);
	ui->pb_mini->installEventFilter(this);
	ui->pb_title->installEventFilter(this);

	//MediaGUIClass.setTitle(this);
	menu_wid_ = ui->wid_menu;

	initUi();
	QObject::connect(this, SIGNAL(sigInitAniDistance()), this, SLOT(onInitAniDistance()));

	QObject::connect(ui->pb_close, &QPushButton::clicked, this, &Title::sigCloseBtnClicked);
	QObject::connect(ui->pb_mini, &QPushButton::clicked, this, &Title::sigMiniBtnClicked);
	QObject::connect(ui->pb_max, &QPushButton::clicked, this, &Title::sigMaxBtnClicked);

	//menu_wid_->setAttribute(Qt::WA_TransparentForMouseEvents);
	//menu_wid_->setAttribute(Qt::WA_TranslucentBackground, true);
}

Title::~Title()
{
	if (ani_menu_show_) delete ani_menu_show_;
	delete ui;
}

void Title::onDrawTimeout()
{
	pb_extend_->is_need_draw_ = true;
	pb_extend_->repaint();
}

void Title::initUi()
{
	initExtendMenu();
	initIcons();

	ani_menu_show_ = new QPropertyAnimation(ui->wid_menu, "geometry");
	ani_menu_show_->setDuration(200);
	QObject::connect(ani_menu_show_, SIGNAL(finished()), this, SLOT(onAniFinished()));

	setStyleSheet(ConfigHelper::getQssString(":/resources/res/css/title.css"));
}

void Title::initExtendMenu()
{
	ext_menu_layout_ = new QHBoxLayout();
	ext_menu_spacer_ = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	ext_menu_layout_->addSpacerItem(ext_menu_spacer_);

	pb_extend_ = new HintButton(ui->wid_menu);
	pb_extend_->setMaximumSize(35, 35);
	pb_extend_->setMinimumSize(35, 35);
	pb_extend_->setCursor(QCursor(Qt::PointingHandCursor));
	ext_menu_layout_->addWidget(pb_extend_);
	connect(pb_extend_, SIGNAL(clicked()), this, SLOT(onExtendMenu()));

	pb_doc_ = new QPushButton();
	pb_doc_->setText(QString::fromLocal8Bit("文件"));
	//pb_doc_->setText(QString("文件"));
	pb_doc_->setMaximumSize(45, 20);
	pb_doc_->setMinimumSize(45, 20);
	pb_doc_->setCursor(QCursor(Qt::PointingHandCursor));
	ext_menu_layout_->addWidget(pb_doc_);

	pb_option_ = new QPushButton();
	pb_option_->setText(QString::fromLocal8Bit("设置"));
	//pb_option_->setText(QString("设置"));
	pb_option_->setMaximumSize(45, 20);
	pb_option_->setMinimumSize(45, 20);
	pb_option_->setCursor(QCursor(Qt::PointingHandCursor));
	ext_menu_layout_->addWidget(pb_option_);

	pb_about_ = new QPushButton();
	pb_about_->setText(QString::fromLocal8Bit("关于"));
	//pb_about_->setText(QString("关于"));
	pb_about_->setMaximumSize(45, 20);
	pb_about_->setMinimumSize(45, 20);
	pb_about_->setCursor(QCursor(Qt::PointingHandCursor));
	ext_menu_layout_->addWidget(pb_about_);

	menu_wid_->setLayout(ext_menu_layout_);
	menu_wid_->setMaximumWidth(35 + 45 + 45 + 45);
}

void Title::initIcons()
{
	int id = QFontDatabase::addApplicationFont(":/resources/res/fonts/Font Awesome 6 Pro-Light-300.otf");
	if (id < 0)
		qWarning() << "FontAwesome cannot be loaded !";

	QFont font;
	QString family = QFontDatabase::applicationFontFamilies(id).first();
	font.setFamily(family);

	font.setPixelSize(18);
	font.setPointSize(18);
	ui->pb_close->setFont(font);
	ui->pb_close->setText(QChar(0x00d7));

	font.setPixelSize(15);
	font.setPointSize(15);
	ui->pb_max->setFont(font);
	ui->pb_max->setText(QChar(0x23f9));

	font.setPixelSize(15);
	font.setPointSize(15);
	ui->pb_mini->setFont(font);
	ui->pb_mini->setText(QChar(0x2013));

	font.setPixelSize(18);
	font.setPointSize(18);
	ui->pb_sep->setEnabled(false);
	ui->pb_sep->setMinimumWidth(10);
	ui->pb_sep->setMaximumWidth(10);
	ui->pb_sep->setFont(font);
	ui->pb_sep->setText(QChar(0x007c));

	font.setPixelSize(15);
	font.setPointSize(15);
	pb_extend_->setFont(font);
	pb_extend_->setText(QChar(0x232a));
}

void Title::initAnimation()
{
	if (!is_distance_init_)
	{
		ani_distance_ = ui->pb_sep->pos().x() - pb_doc_->pos().x()/* - ui->wid_menu->x()*/;
		is_distance_init_ = true;
	}

	if (is_menu_extend_)
	{
		ani_start_x_ = ui->wid_menu->x();
		ani_start_y_ = ui->wid_menu->y();
		ani_end_x_ = ui->wid_menu->x() + ani_distance_;
		ani_end_y_ = ui->wid_menu->y();
	}
	else
	{
		ani_start_x_ = ui->wid_menu->x();
		ani_start_y_ = ui->wid_menu->y();
		ani_end_x_ = ui->wid_menu->x() - ani_distance_;
		ani_end_y_ = ui->wid_menu->y();
	}
	ani_menu_start_rect_ = QRect(ani_start_x_, ani_start_y_, ui->wid_menu->width(), ui->wid_menu->height());
	ani_menu_end_rect_ = QRect(ani_end_x_, ani_end_y_, ui->wid_menu->width(), ui->wid_menu->height());
	ani_menu_show_->setStartValue(ani_menu_start_rect_);
	ani_menu_show_->setEndValue(ani_menu_end_rect_);
}


void Title::setMenu(MenuBar* bar)
{
	ui->wid_menu = bar;
	ui->wid_menu->setVisible(true);
}

void Title::keyPressEvent(QKeyEvent* ev)
{
	ev->ignore();
	return;
}

void Title::paintEvent(QPaintEvent* ev)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

	emit sigInitAniDistance();
}

void Title::mouseDoubleClickEvent(QMouseEvent* ev)
{
	QWidget::mouseDoubleClickEvent(ev);
	emit sigTitleDoubleClicked();
}

void Title::resizeEvent(QResizeEvent* ev)
{
	//QRegion reg(frameGeometry());
	//reg -= QRegion(geometry());
	//reg += childrenRegion();
	//setMask(reg);
	if (is_menu_extend_)
	{

	}
	else
	{
		//menu_wid_->move(menu_wid_->width(), 0);
		menu_wid_->move(ui->pb_sep->x() - pb_extend_->width() - 10, 0);
	}
	return QWidget::resizeEvent(ev);
}

bool Title::eventFilter(QObject* watched, QEvent* event)
{
	//MainWidget* window = qobject_cast<MainWidget*>(watched);
	//if (window == nullptr)
	//	return false;

	//switch (event->type()) {
	//case QEvent::Resize: {
	//	if (window->isMaximized()) {
	//		maxRestoreButton_->setChecked(true);
	//	}
	//	else {
	//		maxRestoreButton_->setChecked(false);
	//	}
	//	return true;
	//}

	//default:
	//	break;
	//}
	if (event->type() == QEvent::KeyPress)
	{
		event->ignore();
		return true;
	}
	return QWidget::eventFilter(watched, event);
}

void Title::HintButton::paintEvent(QPaintEvent* ev)
{
	QPushButton::paintEvent(ev);

	if (is_need_draw_)
	{
		QPainter painter(this);
		QPen linepen(Qt::red);
		linepen.setCapStyle(Qt::RoundCap);
		linepen.setWidth(6);
		painter.setRenderHint(QPainter::Antialiasing, true);
		painter.setPen(linepen);
		painter.drawPoint(this->width() - 6, this->y());
	}
}

void Title::HintButton::setNeedDraw(bool status)
{
	is_need_draw_ = status;
}

void Title::onExtendMenu()
{
	initAnimation();
	if (is_menu_extend_)
	{
		pb_extend_->setText(QChar(0x2329));
		ani_menu_show_->start();
		is_menu_extend_ = false;
	}
	else
	{
		pb_extend_->setNeedDraw(false);
		pb_extend_->setText(QChar(0x232a));
		pb_extend_->repaint();
		ani_menu_show_->start();
		is_menu_extend_ = true;
	}
}

void Title::onInitAniDistance()
{
	if (!is_distance_init_)
	{
		ani_distance_ = ui->pb_sep->pos().x() - pb_doc_->pos().x() - ui->wid_menu->x();
		is_distance_init_ = true;
	}
}

void Title::onAniFinished()
{
	if (!is_menu_extend_)
	{
		pb_extend_->setNeedDraw(true);
		pb_extend_->repaint();
	}
	//else
	//{
	//	pb_extend_->setNeedDraw(false);
	//	pb_extend_->repaint();
	//}
}

void Title::setWindowMaximized()
{
	ui->pb_max->setText(QChar(0xf24d));
}

void Title::setWindowNormal()
{
	ui->pb_max->setText(QChar(0x23f9));
}

QRect Title::getPlainTitleRect()
{
	//return QRect(this->x() + ui->pb_title->width(), 0, this->width() - menu_wid_->width() - ui->pb_title->width(), this->height());
	return QRect(this->x() + ui->pb_title->width(), 0, menu_wid_->x() - (this->x() + ui->pb_title->width()), this->height());
}