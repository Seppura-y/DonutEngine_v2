#ifndef MEDIA_PLAYER_GUI_CLASS_H
#define MEDIA_PLAYER_GUI_CLASS_H

#include <QObject>

class ControlBar;
class MainWidget;
class CameraMenu;
class DisplayWidget;
class CameraPreviewWidget;

class MediaPlayerGuiClass : public QObject
{
	Q_OBJECT

public:
	static MediaPlayerGuiClass& getInstance();

	MediaPlayerGuiClass(const MediaPlayerGuiClass& other) = delete;
	MediaPlayerGuiClass operator=(const MediaPlayerGuiClass& other) = delete;
	MediaPlayerGuiClass(QObject* parent = nullptr);
private:
public:
	void setMainWidget(MainWidget* wid) { this->main_widget_ = wid; }
	void setControlBar(ControlBar* ctrl_bar) { this->control_bar_ = ctrl_bar; }
	void setCameraMenu(CameraMenu* cmr_menu) { this->camera_menu_ = cmr_menu; }
	void setDisplayWidget(DisplayWidget* display_wid) { this->display_widget_ = display_wid; }
	void setCameraPreviewWidget(CameraPreviewWidget* wid) { this->camera_preview_widget_ = wid; }


	void connectSignalsAndSlots();

private:
	MainWidget* main_widget_ = nullptr;
	ControlBar* control_bar_ = nullptr;
	CameraMenu* camera_menu_ = nullptr;
	DisplayWidget* display_widget_ = nullptr;
	CameraPreviewWidget* camera_preview_widget_ = nullptr;
};

#endif