#pragma once

#include <QDialog>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>


#include "ui_item_set_dialog.h"

class ItemSetDialog : public QDialog
{
	Q_OBJECT

public:
	ItemSetDialog(QWidget *parent = Q_NULLPTR);
	ItemSetDialog(int list_index, QWidget* parent = Q_NULLPTR);
	~ItemSetDialog();

	QString getUrl();
	QString getServerUrl();
	QString getSubUrl();
	QString getName();

	void setUrl(QString url);
	void setSubUrl(QString url);
	void setServerUrl(QString url);
	void setName(QString name);
protected slots:
	void getLocalFilename();
protected:
	void initUi(int type);
private:
	Ui::ItemSetDialog ui;
	QPushButton* pb_ok_ = nullptr;
	QPushButton* pb_cancel_ = nullptr;
	QPushButton* pb_open_ = nullptr;
	QLineEdit* le_url_ = nullptr;
	QLineEdit* le_sub_url_ = nullptr;
	QLineEdit* le_server_ = nullptr;
	QLineEdit* le_name_ = nullptr;

	QString filename_;
};
