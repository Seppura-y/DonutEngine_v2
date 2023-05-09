#pragma once

#include <QWidget>
#include <QRect>

class MenuBar;
class QHBoxLayout;
class QSpacerItem;

namespace Ui { class DialogTitle; };

class DialogTitle : public QWidget
{
	Q_OBJECT

public:
	explicit DialogTitle(QWidget* parent = nullptr, const QString& title = QString(""));
	~DialogTitle();

	void initUi();
	void setTitleText(const QString& str);
private:
	void paintEvent(QPaintEvent* ev) override;
signals:
	void sigCloseBtnClicked();
private:
	Ui::DialogTitle* ui;

	int icon_num_ = 0xe1a4;

	QHBoxLayout* ext_menu_layout_ = nullptr;
	QSpacerItem* ext_menu_spacer_ = nullptr;

};
