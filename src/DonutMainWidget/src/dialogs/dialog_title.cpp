#include "dialog_title.h"
#include "ui_dialog_title.h"
#include "config_helper.h"
#include "font_icon.h"


#include <QDebug>
#include <QFontDatabase>
#include <QStyleOption>
#include <QPainter>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QString>

#include <iostream>


DialogTitle::DialogTitle(QWidget* parent, const QString& title) : QWidget(parent)
{
	ui = new Ui::DialogTitle();
	ui->setupUi(this);
	this->setFixedHeight(30);

	ui->pb_close->setCursor(QCursor(Qt::PointingHandCursor));

	ui->pb_close->raise();

	ui->lb_title->setText(title);

	initUi();

	QObject::connect(ui->pb_close, &QPushButton::clicked, this, &DialogTitle::sigCloseBtnClicked);
}

DialogTitle::~DialogTitle()
{
	delete ui;
}

void DialogTitle::initUi()
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

	setStyleSheet(ConfigHelper::getQssString(":/resources/res/css/dialog_base.css"));
}

void DialogTitle::setTitleText(const QString& str)
{
	ui->lb_title->setText(str);
}




void DialogTitle::paintEvent(QPaintEvent* ev)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}