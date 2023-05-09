#pragma once

#include <QDialog>
#include <QLabel>
#include <QTimer>

#include "ui_screen_capture_dialog.h"

namespace Ui { class ScreenCaptureDialog; };

class ScreenCaptureDialog : public QDialog
{
	Q_OBJECT

public:
    enum SideType
    {
        NO_SIDE = 0,
        TOP_SIDE = 0b0001,
        BOTTOM_SIDE = 0b0010,
        RIGHT_SIDE = 0b0100,
        LEFT_SIDE = 0b1000,
        TOPLEFT_SIDE = TOP_SIDE | LEFT_SIDE,
        BOTTOMLEFT_SIDE = BOTTOM_SIDE | LEFT_SIDE,
        TOPRIGHT_SIDE = TOP_SIDE | RIGHT_SIDE,
        BOTTOMRIGHT_SIDE = BOTTOM_SIDE | RIGHT_SIDE,
        CENTER = 0b10000,
        MOVE_HANDLE = 0b100000
    };

public:
    ScreenCaptureDialog(QWidget *parent = nullptr);
	~ScreenCaptureDialog();

    //explicit ResizeableTopWidget(QColor color, QWidget* parent = nullptr);
    SideType getMouseSide(const QPoint& mousePos) const;
    QVector<QRect> handlerAreas();
    QRect getCaptureArea();

    void setGeometry(const QRect& r);
    QRect geometry() const;
protected:

private:
    void updateAreas();
    void updateCursor();
    QRect rect() const;
    SideType getProperSide(SideType side, const QRect& r);



    void paintEvent(QPaintEvent*) override;
    void resizeEvent(QResizeEvent*) override;
    void moveEvent(QMoveEvent*) override;

    void mousePressEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;

private:
    QColor m_color;
    QPoint m_areaOffset;
    QPoint m_handleOffset;

    QPoint m_dragStartPos;
    SideType m_activeSide;
    QCursor m_idleCentralCursor;
    bool m_ignoreMouse;
    bool m_mouseStartMove;

    // naming convention for handles
    // T top, B bottom, R Right, L left
    // 2 letters: a corner
    // 1 letter: the handle on the middle of the corresponding side
    QRect m_TLHandle, m_TRHandle, m_BLHandle, m_BRHandle;
    QRect m_LHandle, m_THandle, m_RHandle, m_BHandle;

    QRect m_TLArea, m_TRArea, m_BLArea, m_BRArea;
    QRect m_LArea, m_TArea, m_RArea, m_BArea;

    QRect move_handle_area_;

    QImage* image_ = nullptr;

    QLabel* lb_move_handler_ = nullptr;

    QRect global_pos;

    QPushButton* pb_ok_ = nullptr;
    QPushButton* pb_cancel_ = nullptr;
    bool is_resize_stop_ = false;
    QRect previous_geom_;

private:
    Ui::ScreenCaptureDialog ui;
};
