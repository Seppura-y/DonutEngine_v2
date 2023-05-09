#ifndef CAMERA_PREVIEW_WIDGET_H
#define CAMERA_PREVIEW_WIDGET_H

#include <QWidget>
#include <QTimer>
#include <QJsonObject>
// #include "canvas/canvas_scene.h"
// #include "canvas/canvas_view.h"
// #include "canvas/canvas_widget.h"
#include "ui_display_widget.h"

class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;
class QToolButton;
class QPushButton;
class QLabel;
class QButtonGroup;
class QAbstractButton;
class QPropertyAnimation;
class VolumeButton;
class RenderWidget;
class ControlBar;
class SplitterWidget;

class CameraPreviewWidget : public QWidget
{
	Q_OBJECT

public:
	CameraPreviewWidget(QWidget *parent = Q_NULLPTR);
	~CameraPreviewWidget();


protected:
	void resizeEvent(QResizeEvent* ev) override;
	void dragEnterEvent(QDragEnterEvent* ev)override;
	void dropEvent(QDropEvent* ev) override;
	bool eventFilter(QObject* watched, QEvent* event) override;
	void mouseDoubleClickEvent(QMouseEvent* ev) override;
	void keyPressEvent(QKeyEvent* ev) override;
private:
	void initTitle();
	void initContent();
	void initToolBar();
	void initControlBar();

	void fullscreenDisplay(bool status);

signals:
	void sigDisplayShowFullscreen(bool);
	void sigAddLocalFileItem(QJsonObject&);

	void sigPlayNextFile();
	void sigPlayPrevFile();
	void sigPlayingFile(int,QString);

	void sigResetSplitterContent(int);
public slots:
	void onPanelChanged(bool is_checked);
	void onTitleButtonToggled(QAbstractButton* bt, bool checked);
	void onToolBarButtonToggled(QAbstractButton* bt);
	void onSoundVolumeValueChanged(int value);
	void onShowFullScreen(bool status);

	void onListItemDoubleClicked(QJsonObject& url);
	void onItemListUpdate(int type, QJsonObject& obj);
private slots:
	void onMouseDetectTimeout();

private:
	void setIgnoreKeyPress();
private:
	Ui::DisplayWidget ui;

	////////////////////////Title Bar/////////////////////////////
	QPushButton*	btn_screen_cap_ = nullptr;
	QPushButton*	btn_cam_cap_ = nullptr;
	QPushButton*	btn_web_cam_ = nullptr;
	QButtonGroup*	btn_group_title_ = nullptr;
	QLabel*			lb_item_name_ = nullptr;
	QHBoxLayout*	layout_title_ = nullptr;

	////////////////////////Tool Bar/////////////////////////////
	QPushButton* btn_split_1_ = nullptr;
	QPushButton* btn_split_4_ = nullptr;
	QPushButton* btn_split_9_ = nullptr;
	QPushButton* btn_split_16_ = nullptr;
	QPushButton* btn_split_25_ = nullptr;
	QPushButton* btn_split_custom_ = nullptr;
	QButtonGroup* btn_group_toolbar_ = nullptr;

	QPushButton* btn_start_push_ = nullptr;
	QPushButton* btn_push_setting_ = nullptr;
	QPushButton* btn_push_info_ = nullptr;
	QHBoxLayout* layout_toolbar_ = nullptr;

	QPushButton* btn_add_source_ = nullptr;

	VolumeButton* btn_volume_ = nullptr;

	int last_split_id_ = 0;
	bool is_custom_layout_ = false;
	//////////////////////Render Widget//////////////////////
	RenderWidget* render_widget_ = nullptr;

	/////////////////////Splitter Widget////////////////////
	SplitterWidget* splitter_widget_ = nullptr;

	//////////////////////Control Bar//////////////////////
	ControlBar* control_bar_ = nullptr;
	QPropertyAnimation* ani_control_bar_show_ = nullptr;
	QPropertyAnimation* ani_control_bar_hide_ = nullptr;
	QRect rect_control_bar_show_;
	QRect rect_control_bar_hide_;
	QTimer timer_control_bar_;
	QTimer timer_mouse_detect_;
	bool is_fullscreen_ = false;
	bool is_control_bar_show_ = true;

	QTimer timer_check_fullscreen_;


	//////////////////////Display Info//////////////////////
	bool is_playing_ = false;
	bool is_open_success_ = false;
	QString openning_filename_;
	QList<QString> file_list_;
	QList<QString> path_list_;
	QMap<QString, QString> playlist_;
	QJsonObject playlist_obj_;

	////////////////////////canvas/////////////////////////////
	//CanvasWidget* canvas_wid_ = nullptr;
	// CanvasView* canvas_view_ = nullptr;
	// CanvasScene* canvas_scene_ = nullptr;

	///////////////////////audio recorder///////////////////


	///////////get merge picture from canvas////////////


	///////////////////////publisher//////////////////////



};

#endif