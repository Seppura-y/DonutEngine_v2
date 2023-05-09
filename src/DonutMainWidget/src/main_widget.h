#pragma once

#include <QtWidgets/QMainWindow>
#include <QMouseEvent>
#include <QRect>
#ifdef Q_OS_WIN32
#include <Windows.h>
#endif


#include "ui_main_widget.h"

class MenuBar;
class Title;
class CameraMenu;
class QHBoxLayout;
class QPushButton;
class ControlBar;
class DisplayWidget;
class CameraPreviewWidget;
class SplitterWidget;

class MainWidget : public QMainWindow
{
    Q_OBJECT

public:
    static MainWidget* getInstance()
    {
        if (!instance_)
        {
            instance_ = new MainWidget();
        }
        return instance_;
    }
    MainWidget(const MainWidget& other) = delete;
    MainWidget operator=(const MainWidget& other) = delete;
private:
    MainWidget(QWidget *parent = Q_NULLPTR);

protected:
    void mouseMoveEvent(QMouseEvent* ev) override;
    void mousePressEvent(QMouseEvent* ev) override;
    void mouseReleaseEvent(QMouseEvent* ev) override;
    void mouseDoubleClickEvent(QMouseEvent* ev) override;
    bool nativeEvent(const QByteArray& eventType,void* message,long* result) override;
    void resizeEvent(QResizeEvent* ev) override;
    void keyPressEvent(QKeyEvent* ev) override;
    //bool eventFilter(QObject* watched, QEvent* event) override;
protected:
    void initTitle();
    void initMainWidget();
    void initMainWidgetForCamPreview();

protected slots:

private:
    void createMenuBar();
    LRESULT calculateBorder(const QPoint& pt);
signals:
    void sigWindowMaximized();
    void sigWindowNormal();
    void sigShowFullscreen(bool);

    void sigUpArrowKeyPressed();
    void sigDownArrowKeyPressed();
    void sigLeftArrowKeyPressed();
    void sigRightArrowKeyPressed();
    void sigSpaceKeyPressed();
    void sigMKeyPressed();
private:
    MenuBar* menu_bar_ = nullptr;
    Title* title_ = nullptr;
    CameraMenu* cam_menu_ = nullptr;
private:
    Ui::MainWidgetClass ui;

    static MainWidget* instance_;

    bool left_bt_pressed_ = false;
    QPoint mouse_drag_point_;

    bool is_maximized_init_ = false;
    bool maximized_flag_ = false;
    QRect maximized_rect_;
    QRect normal_rect_;

    QHBoxLayout* main_wid_layout_ = nullptr;
    QPushButton* menu_extend_bt_ = nullptr;
    bool is_cam_menu_extend_ = true;

    DisplayWidget* display_widget_ = nullptr;

    bool is_fullscreen_ = false;

    ////////////////////////////////////////////////////////////////////
    //nativeEvent
    bool is_maximized_ = false;
    bool is_minimized_ = false;

    ////////////////////////////////////////////////////////////////////
    // Control bar
    ControlBar* control_bar_ = nullptr;

    ////////////////////////////////////////////////////////////////////
    // Control bar
    CameraPreviewWidget* camera_preview_widget_ = nullptr;

    ////////////////////////////////////////////////////////////////////
    // Splitter widget
    SplitterWidget* splitter_widget_ = nullptr;
};

