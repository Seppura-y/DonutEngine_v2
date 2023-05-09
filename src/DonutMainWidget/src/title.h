#pragma once

#include <QWidget>
#include <QPropertyAnimation>
#include <QRect>
#include <QPushButton>
#include <QTimer>
#include <QMouseEvent>

class MenuBar;
class QHBoxLayout;
class QSpacerItem;

namespace Ui { class Title; };

class Title : public QWidget
{
	Q_OBJECT

public:
	explicit Title(QWidget* parent = Q_NULLPTR);
	~Title();

	void initUi();
	void initIcons();
	void initAnimation();
	void initExtendMenu();

	void setWindowMaximized();
	void setWindowNormal();

	void setMenu(MenuBar* bar);
	QWidget* getMenuWidget() { return menu_wid_; }
	//ÅÅ³ý°´¼üµÄTitleBar¿Õ°×ÇøÓò
	QRect getPlainTitleRect();

	class HintButton : public QPushButton
	{
	public:
		HintButton(QWidget* wid) : QPushButton(wid){}
		void paintEvent(QPaintEvent* ev) override;

		void setNeedDraw(bool status);
	public:
		bool is_need_draw_ = false;
	};

protected:
	void keyPressEvent(QKeyEvent* ev) override;
private:
	void paintEvent(QPaintEvent* ev) override;
	void mouseDoubleClickEvent(QMouseEvent* ev) override;
	bool eventFilter(QObject* watched, QEvent* event) override;
	void resizeEvent(QResizeEvent* ev) override;
signals:
	void sigCloseBtnClicked();
	void sigMiniBtnClicked();
	void sigInitAniDistance();

	void sigMaxBtnClicked();
	void sigTitleDoubleClicked();

public slots:
	void onDrawTimeout();
	void onExtendMenu();
	void onInitAniDistance();
	void onAniFinished();
private:
	Ui::Title* ui;

	MenuBar* menu_bar_ = nullptr;
	int icon_num_ = 0xe1a4;

	QHBoxLayout* ext_menu_layout_ = nullptr;
	QSpacerItem* ext_menu_spacer_ = nullptr;
	int ani_start_x_ = 0;
	int ani_start_y_ = 0;
	int ani_end_x_ = 0;
	int ani_end_y_ = 0;
	int ani_distance_ = 0;
	QPropertyAnimation* ani_menu_show_ = nullptr;
	QRect ani_menu_start_rect_;
	QRect ani_menu_end_rect_;

	QWidget* menu_wid_ = nullptr;

	QPushButton* pb_mini_ = nullptr;
	QPushButton* pb_max_ = nullptr;
	HintButton* pb_extend_ = nullptr;

	QPushButton* pb_doc_ = nullptr;
	QPushButton* pb_option_ = nullptr;
	QPushButton* pb_about_ = nullptr;

	bool is_menu_extend_ = true;
	bool is_distance_init_ = false;
	bool is_maximized_ = false;
};
