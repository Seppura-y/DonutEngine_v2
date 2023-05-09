#include "main_widget.h"

#include "media_player_gui_class.h"

#include <iostream>
#include <QPushButton>
#include <QFont>
#include <QPropertyAnimation>
#include <QOpenglExtensions>
#include <QOpenglWidget>
#include <QSlider>
#include <QLabel>
#include <QScrollArea>
#include <QDebug>
#include <QPainter>
#include <QHBoxLayout>
#include <QFontDatabase>
#include <QtWinExtras/QtWin>
//#include <dwmapi.h>
#include <QScreen>


#include "config_helper.h"
#include "font_icon.h"
#include "title.h"
// #include "media_manager_gui_class.h"
#include "menu_bar.h"
#include "camera_menu.h"
#include "display_widget.h"
#include "render_widget.h"
// #include "mybutton.h"
// #include "panel_widget.h"

#include "camera_preview_widget.h"

#include <Windows.h>
#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lParam)    ((int)(short)LOWORD(lParam))
#endif
#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lParam)    ((int)(short)HIWORD(lParam))
#endif

using namespace std;

MainWidget* MainWidget::instance_ = nullptr;

//
//BOOL CALLBACK EnumVLC(HWND hwnd, LPARAM lParam)
//{
//    TCHAR szTitle[1024];
//    int len = GetWindowText(hwnd, szTitle, 1024);
//
//    if (len > 0)
//    {
//        EnableWindow(hwnd, FALSE);
//        KillTimer(NULL, 1);
//    }
//    return TRUE;
//}
//
//void CALLBACK TimeProc(HWND hwnd, UINT msg, UINT_PTR id, DWORD time)
//{
//    HWND wnd = FindWindowEx(NULL, NULL, NULL, "MediaManager");
//    if (wnd)
//    {
//        EnumChildWindows(wnd, EnumVLC, NULL);
//    }
//}


MainWidget::MainWidget(QWidget *parent) : QMainWindow(parent)
{
    ui.setupUi(this);

    this->setFocus();
    ui.statusBar->close();
    this->setWindowFlag(Qt::WindowType::FramelessWindowHint);
    //this->setWindowFlag(Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    //setAttribute(Qt::WA_Hover);
    setStyleSheet(ConfigHelper::getQssString(":resources/res/css/main_widget.css"));

    HWND hwnd = reinterpret_cast<HWND>(this->winId());
    DWORD style = GetWindowLong(hwnd, GWL_STYLE);
    SetWindowLongPtr(hwnd, GWL_STYLE, style/* | WS_MAXIMIZEBOX*/ | WS_THICKFRAME | WS_CAPTION);
    //SetWindowLongPtr(hwnd, GWL_STYLE, style | WS_THICKFRAME);

    //bool enabled = QtWin::isCompositionEnabled();
    //if (enabled) {
    //    HWND hwnd = (HWND)this->winId();
    //    DWORD style = ::GetWindowLong(hwnd, GWL_STYLE);
    //    //::SetWindowLong(hwnd, GWL_STYLE, style | WS_THICKFRAME | WS_CAPTION | WS_BORDER);
    //    SetWindowLong(hwnd, GWL_STYLE, style | WS_THICKFRAME);
    //    QtWin::extendFrameIntoClientArea(this, -1, -1, -1, -1);
    //}
    this->resize(1280, 720);
    normal_rect_ = this->geometry();
    main_wid_layout_ = new QHBoxLayout;
    initTitle();
    initMainWidget();
    //initMainWidgetForCamPreview();

    this->setMouseTracking(true);

    //installEventFilter(title_);

    MediaPlayerGuiClass::getInstance().setMainWidget(this);
}

void MainWidget::mousePressEvent(QMouseEvent* ev)
{
    if (ev->button() == Qt::LeftButton)
    {
        left_bt_pressed_ = true;
        mouse_drag_point_ = ev->pos();
    }
    //else
    //{
    //    return QMainWindow::mousePressEvent(ev);
    //}
    return QMainWindow::mousePressEvent(ev);
}

void MainWidget::mouseMoveEvent(QMouseEvent* ev)
{
    if (ui.title_wid->geometry().contains(mouse_drag_point_) && left_bt_pressed_)
    {
        this->move(ev->globalPos() - mouse_drag_point_);
    }
    else
    {
        return QMainWindow::mouseMoveEvent(ev);
    }
    return QMainWindow::mouseMoveEvent(ev);
}

void MainWidget::mouseReleaseEvent(QMouseEvent* ev)
{
    if (left_bt_pressed_)
    {
        left_bt_pressed_ = false;
        if (!is_maximized_init_)
        {
            return;
        }
        if (this->geometry().topLeft() != maximized_rect_.topLeft() && this->geometry().size() == maximized_rect_.size())
        {
            maximized_flag_ = true;
            //emit sigWindowNormal();
            title_->setWindowNormal();
        }
        else
        {
            maximized_flag_ = false;
            title_->setWindowMaximized();
        }
    }
    return QMainWindow::mouseReleaseEvent(ev);
}

void MainWidget::mouseDoubleClickEvent(QMouseEvent* ev)
{
    //if (this->isFullScreen())
    //{
    //    is_fullscreen_ = false;
    //    this->showNormal();
    //    this->setGeometry(normal_rect_);
    //    cam_menu_->show();
    //    menu_extend_bt_->show();
    //    ui.title_wid->show();
    //    //this->setWindowFlag(Qt::WindowStaysOnTopHint, false);
    //    emit sigShowFullscreen(false);
    //    qDebug() << "show normal";
    //}
    //else
    //{
    //    is_fullscreen_ = true;
    //    cam_menu_->hide();
    //    menu_extend_bt_->hide();
    //    ui.title_wid->hide();
    //    //this->setWindowFlag(Qt::WindowStaysOnTopHint,true);

    //    auto screen = QGuiApplication::primaryScreen();
    //    QRect screen_rect = screen->geometry();
    //    this->setGeometry(0, 0, screen_rect.width(), screen_rect.height());
    //    this->showFullScreen();
    //    emit sigShowFullscreen(true);
    //    qDebug() << "show full";
    //}
}

bool MainWidget::nativeEvent(const QByteArray& eventType, void* message, long* result)
{
#ifdef Q_OS_WIN
    if (eventType != "windows_generic_MSG")
        return false;

    MSG* msg = static_cast<MSG*>(message);

    QWidget* widget = QWidget::find(reinterpret_cast<WId>(msg->hwnd));
    if (!widget)
        return false;

    switch (msg->message) 
    {
        case WM_NCCALCSIZE: 
        {
            *result = 0;
            return true;
        }

        case WM_NCHITTEST: 
        {
            int x = GET_X_LPARAM(msg->lParam);
            int y = GET_Y_LPARAM(msg->lParam);
            QPoint pt = mapFromGlobal(QPoint(x, y));
            *result = calculateBorder(pt);

            if (*result == HTCLIENT) 
            {
                if (title_->getPlainTitleRect().contains(pt))
                {
                    *result = HTCAPTION;
                }
            }
            return true;
        }

        case WM_GETMINMAXINFO: 
        {
            LPMINMAXINFO lpMMI = (LPMINMAXINFO)(msg->lParam);
            lpMMI->ptMinTrackSize.x = 1280;
            lpMMI->ptMinTrackSize.y = 720;

            if (::IsZoomed(msg->hwnd)) 
            {
                is_maximized_ = true;
                title_->setWindowMaximized();
                RECT frame = { 0, 0, 0, 0 };
                AdjustWindowRectEx(&frame, WS_OVERLAPPEDWINDOW, FALSE, 0);
                frame.left = abs(frame.left);
                frame.top = abs(frame.bottom);
                widget->setContentsMargins(frame.left, frame.top, frame.right, frame.bottom);
            }
            else 
            {
                widget->setContentsMargins(0, 0, 0, 0);
                title_->setWindowNormal();
                is_maximized_ = false;
            }

            *result = ::DefWindowProc(msg->hwnd, msg->message, msg->wParam, msg->lParam);
            return true;
            break;
        }

        default:
            //*result = ::DefWindowProc(msg->hwnd, msg->message, msg->wParam, msg->lParam);
            break;
    }

#endif
    return QWidget::nativeEvent(eventType, message, result);
}

void MainWidget::resizeEvent(QResizeEvent* ev)
{
    if (!is_fullscreen_)
    {
        normal_rect_ = this->geometry();
    }
    else
    {
        return QWidget::resizeEvent(ev);
    }
}


LRESULT MainWidget::calculateBorder(const QPoint& pt)
{
    if (::IsZoomed((HWND)this->winId())) {
        return HTCLIENT;
    }
    int borderSize = 4;
    int cx = this->size().width();
    int cy = this->size().height();

    QRect rectTopLeft(0, 0, borderSize, borderSize);
    if (rectTopLeft.contains(pt)) {
        return HTTOPLEFT;
    }

    QRect rectLeft(0, borderSize, borderSize, cy - borderSize * 2);
    if (rectLeft.contains(pt)) {
        return HTLEFT;
    }

    QRect rectTopRight(cx - 2 * borderSize, 0, 2 * borderSize, 2 * borderSize);
    if (rectTopRight.contains(pt)) {
        return HTTOPRIGHT;
    }

    QRect rectRight(cx - borderSize, borderSize, borderSize, cy - borderSize * 2);
    if (rectRight.contains(pt)) {
        return HTRIGHT;
    }

    QRect rectTop(borderSize, 0, cx - borderSize * 2, borderSize);
    if (rectTop.contains(pt)) {
        return HTTOP;
    }

    QRect rectBottomLeft(0, cy - 2 * borderSize, 2 * borderSize, 2 * borderSize);
    if (rectBottomLeft.contains(pt)) {
        return HTBOTTOMLEFT;
    }

    QRect rectBottomRight(cx - 2 * borderSize, cy - 2 * borderSize, 2 * borderSize, 2 * borderSize);
    if (rectBottomRight.contains(pt)) {
        return HTBOTTOMRIGHT;
    }

    QRect rectBottom(borderSize, cy - borderSize, cx - borderSize * 2, borderSize);
    if (rectBottom.contains(pt)) {
        return HTBOTTOM;
    }

    return HTCLIENT;
}

void MainWidget::initTitle()
{
    //title_ = MediaGUIClass.getTitle();

    title_ = new Title();

    QHBoxLayout* layout = new QHBoxLayout(ui.title_wid);
    layout->addWidget(title_);
    ui.title_wid->setLayout(layout);
    layout->setContentsMargins(0, 0, 0, 0);
    ui.title_wid->setGeometry(0, 0, this->width(), title_->height());

    connect(title_, &Title::sigCloseBtnClicked, this, [this] {
            this->close();
        });

    connect(title_, &Title::sigMaxBtnClicked, this, [this] {
            if (this->isMaximized())
            {
                this->showNormal();
                title_->setWindowNormal();
                //emit sigWindowNormal();
            }
            else
            {
                if (maximized_flag_ && is_maximized_init_)
                {
                    this->setGeometry(maximized_rect_);
                    this->setWindowState(Qt::WindowMaximized);
                    maximized_flag_ = false;
                }
                else
                {
                    this->showMaximized();
                }

                //emit sigWindowMaximized();
                title_->setWindowMaximized();

                if (!is_maximized_init_)
                {
                    is_maximized_init_ = true;
                    maximized_rect_ = this->geometry();
                }
            }
        });

    connect(title_, &Title::sigMiniBtnClicked, this, [this] {
            this->showMinimized();
        });

    connect(title_, &Title::sigTitleDoubleClicked, this, [this] {
            if (this->isMaximized())
            {
                this->showNormal();
                //emit sigWindowNormal();
                title_->setWindowNormal();
            }
            else
            {
                if (maximized_flag_ && is_maximized_init_)
                {
                    this->setGeometry(maximized_rect_);
                    this->setWindowState(Qt::WindowMaximized);
                    maximized_flag_ = false;
                }
                else
                {
                    this->showMaximized();
                }

                //emit sigWindowMaximized();
                title_->setWindowMaximized();

                if (!is_maximized_init_)
                {
                    is_maximized_init_ = true;
                    maximized_rect_ = this->geometry();
                }
            }
        });

    //this->installEventFilter(title_);
}

void MainWidget::createMenuBar()
{
    menu_bar_ = new MenuBar(title_);
    menu_bar_->addMenu("asdf");
    menu_bar_->addMenu("sdfsdf");
    menu_bar_->addMenu("sdfw");
    menu_bar_->addMenu("gn");
}

void MainWidget::initMainWidget()
{
    display_widget_ = new DisplayWidget();
    cam_menu_ = new CameraMenu();
    cam_menu_->setMaximumWidth(230);
    QObject::connect(cam_menu_, &CameraMenu::sigProcessScreenCapture, this, [this] { this->hide(); });
    QObject::connect(cam_menu_, &CameraMenu::sigProcessScreenCaptureFinished, this, [this] {qDebug() << "man fin"; this->show(); });

    menu_extend_bt_ = new QPushButton;
    menu_extend_bt_->setMaximumSize(8, 90);
    menu_extend_bt_->setFixedSize(8, 90);
    menu_extend_bt_->setStyleSheet(ConfigHelper::getQssString(":/resources/res/css/narrow_button.css"));

    int id = QFontDatabase::addApplicationFont(":/resources/res/fonts/Font Awesome 6 Duotone-Solid-900.otf");

    if (id < 0)
        qWarning() << "FontAwesome cannot be loaded !";

    QFont font;
    QString family = QFontDatabase::applicationFontFamilies(id).first();
    font.setFamily(family);

    font.setPixelSize(12);
    font.setPointSize(12);
    menu_extend_bt_->setFont(font);
    menu_extend_bt_->setText(QChar(0xf32e));

    QHBoxLayout* left_extent_bt_layout = new QHBoxLayout;
    left_extent_bt_layout->setContentsMargins(0, 0, 0, 0);
    left_extent_bt_layout->setMargin(0);
    left_extent_bt_layout->addWidget(menu_extend_bt_);
    left_extent_bt_layout->setSpacing(0);

    QPalette palette;
    palette.setColor(QPalette::Background, QColor(0, 255, 0, 0));


    //display_widget_->installEventFilter(this);

    QObject::connect(display_widget_, &DisplayWidget::sigDisplayShowFullscreen, [=](bool status)
        {
            if(status)
            {
                is_fullscreen_ = true;

                cam_menu_->hide();
                menu_extend_bt_->hide();
                ui.title_wid->hide();
                //this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
                auto screen = QGuiApplication::primaryScreen();
                QRect screen_rect = screen->geometry();
                this->setGeometry(0, 0, screen_rect.width(), screen_rect.height());
                this->showFullScreen();
                //emit sigShowFullscreen(true);
            }
            else
            {
                is_fullscreen_ = false;
                cam_menu_->show();
                menu_extend_bt_->show();
                ui.title_wid->show();
                this->showNormal();
                this->setGeometry(normal_rect_);
                //emit sigShowFullscreen(false);
            }
        });
    //panel_widget_->setAutoFillBackground(true);
    //panel_widget_->setPalette(palette);

    main_wid_layout_->setContentsMargins(0, 0, 0, 5);
    main_wid_layout_->setMargin(0);
    main_wid_layout_->setSpacing(0);
    main_wid_layout_->addWidget(cam_menu_);
    //main_wid_layout_->addWidget(wid);
    main_wid_layout_->addLayout(left_extent_bt_layout);
    //main_wid_layout_->addWidget(w);
    main_wid_layout_->addWidget(display_widget_);

    ui.main_wid->setLayout(main_wid_layout_);

    connect(menu_extend_bt_, &QPushButton::clicked, this,
        [this]
        {
            if (is_cam_menu_extend_)
            {
                is_cam_menu_extend_ = false;
                this->cam_menu_->hide();
                menu_extend_bt_->setText(QChar(0xf330));
            }
            else
            {
                is_cam_menu_extend_ = true;
                this->cam_menu_->show();
                menu_extend_bt_->setText(QChar(0xf32e));
            }
        }
        );


    QObject::connect(this, &MainWidget::sigShowFullscreen, display_widget_, &DisplayWidget::onShowFullScreen);
}

void MainWidget::initMainWidgetForCamPreview()
{
    int id = QFontDatabase::addApplicationFont(":/resources/res/fonts/Font Awesome 6 Duotone-Solid-900.otf");

    if (id < 0)
        qWarning() << "FontAwesome cannot be loaded !";

    QFont font;
    QString family = QFontDatabase::applicationFontFamilies(id).first();
    font.setFamily(family);

    font.setPixelSize(12);
    font.setPointSize(12);

    cam_menu_ = new CameraMenu();
    cam_menu_->setMaximumWidth(230);
    QObject::connect(cam_menu_, &CameraMenu::sigProcessScreenCapture, this, [this] { this->hide(); });
    QObject::connect(cam_menu_, &CameraMenu::sigProcessScreenCaptureFinished, this, [this] {qDebug() << "man fin"; this->show(); });

    menu_extend_bt_ = new QPushButton;
    menu_extend_bt_->setMaximumSize(8, 90);
    menu_extend_bt_->setFixedSize(8, 90);
    menu_extend_bt_->setStyleSheet(ConfigHelper::getQssString(":/resources/res/css/narrow_button.css"));
    menu_extend_bt_->setFont(font);
    menu_extend_bt_->setText(QChar(0xf32e));
    connect(menu_extend_bt_, &QPushButton::clicked, this,
        [this]
        {
            if (is_cam_menu_extend_)
            {
                is_cam_menu_extend_ = false;
                this->cam_menu_->hide();
                menu_extend_bt_->setText(QChar(0xf330));
            }
            else
            {
                is_cam_menu_extend_ = true;
                this->cam_menu_->show();
                menu_extend_bt_->setText(QChar(0xf32e));
            }
        }
    );

    QHBoxLayout* left_extent_bt_layout = new QHBoxLayout;
    left_extent_bt_layout->setContentsMargins(0, 0, 0, 0);
    left_extent_bt_layout->setMargin(0);
    left_extent_bt_layout->addWidget(menu_extend_bt_);
    left_extent_bt_layout->setSpacing(0);

    camera_preview_widget_ = new CameraPreviewWidget();

    main_wid_layout_->setContentsMargins(0, 0, 0, 5);
    main_wid_layout_->setMargin(0);
    main_wid_layout_->setSpacing(0);
    main_wid_layout_->addWidget(cam_menu_);
    main_wid_layout_->addLayout(left_extent_bt_layout);
    main_wid_layout_->addWidget(camera_preview_widget_);
    ui.main_wid->setLayout(main_wid_layout_);
}


void MainWidget::keyPressEvent(QKeyEvent* ev)
{
    this->setFocus();
    if (ev->key() == Qt::Key_Space)
    {
        qDebug() << "key space pressed";
        emit sigSpaceKeyPressed();
    }

    if (ev->key() == Qt::Key_M)
    {
        qDebug() << "key m pressed";
        emit sigMKeyPressed();
    }

    if (ev->key() == Qt::Key_Up)
    {
        //qDebug() << "key up pressed";
        emit sigUpArrowKeyPressed();
    }

    if (ev->key() == Qt::Key_Down)
    {
        //qDebug() << "key down pressed";
        emit sigDownArrowKeyPressed();
    }

    if (ev->key() == Qt::Key_Left)
    {
        //qDebug() << "key left pressed";
        emit sigLeftArrowKeyPressed();
    }

    if (ev->key() == Qt::Key_Right)
    {
        //qDebug() << "key right pressed";
        emit sigRightArrowKeyPressed();

    }



    return QWidget::keyPressEvent(ev);
}