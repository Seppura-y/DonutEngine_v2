#include "item_set_dialog.h"
#include <QFileDialog>

ItemSetDialog::ItemSetDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
}

ItemSetDialog::ItemSetDialog(int list_index, QWidget* parent) : QDialog(parent)
{
	ui.setupUi(this);
	initUi(list_index);
}

ItemSetDialog::~ItemSetDialog()
{

}

void ItemSetDialog::initUi(int type)
{
	pb_ok_ = ui.pb_ok;
	pb_cancel_ = ui.pb_cancel;
	pb_open_ = ui.pb_open;
	le_name_ = ui.le_name;
	le_url_ = ui.le_url;
	le_server_ = ui.le_server;
	le_sub_url_ = ui.le_sub_url;

	if (type == 1)
	{
		le_sub_url_->hide();
		ui.label_2->hide();
	}
	else
	{
		pb_open_->hide();
	}

	QObject::connect(pb_ok_, &QPushButton::clicked, this, &ItemSetDialog::accept);
	QObject::connect(pb_cancel_, &QPushButton::clicked, this, &ItemSetDialog::reject);
	QObject::connect(pb_open_, &QPushButton::clicked, this, &ItemSetDialog::getLocalFilename);
}

QString ItemSetDialog::getUrl()
{
	return le_url_->text();
}

QString ItemSetDialog::getServerUrl()
{
	return le_server_->text();
}

QString ItemSetDialog::getSubUrl()
{
	return le_sub_url_->text();
}

QString ItemSetDialog::getName()
{
	return le_name_->text();
}

void ItemSetDialog::getLocalFilename()
{
     filename_ = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("select file"), QString("./"), QString("Files (*.mp4)"));
	 ui.le_url->setText(filename_);
}

void ItemSetDialog::setUrl(QString url)
{
	ui.le_url->setText(url);
}

void ItemSetDialog::setSubUrl(QString url)
{
	ui.le_sub_url->setText(url);
}

void ItemSetDialog::setServerUrl(QString url)
{
	ui.le_server->setText(url);
}

void ItemSetDialog::setName(QString name)
{
	ui.le_name->setText(name);
}