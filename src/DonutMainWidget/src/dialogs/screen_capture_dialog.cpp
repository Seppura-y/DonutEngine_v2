#include "screen_capture_dialog.h"
#include "config_helper.h"

#include <QPainter>
#include <QMouseEvent>
#include <QPen>
#include <QBrush>
#include <QWindow>
#include <QPushButton>
#include <QScreen>
#include <QGuiApplication>
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QDebug>
#include <QFontDatabase>

#define MARGIN (m_THandle.width())

ScreenCaptureDialog::ScreenCaptureDialog(QWidget *parent)
	: QDialog(parent)
    , m_activeSide(NO_SIDE)
    , m_ignoreMouse(false)
{
	ui.setupUi(this);
    this->resize(640, 480);
    this->setMinimumSize(180, 180);
    m_color = QColor("orange");
    
    previous_geom_ = this->geometry();

    int id = QFontDatabase::addApplicationFont(":/resources/res/fonts/Font Awesome 6 Pro-Light-300.otf");
    if (id < 0)
        qWarning() << "FontAwesome cannot be loaded !";

    QFont font;
    QString family = QFontDatabase::applicationFontFamilies(id).first();
    font.setFamily(family);

    font.setPixelSize(16);
    font.setPointSize(16);

    // prevents this widget from consuming CaptureToolButton mouse events
    //setAttribute(Qt::WA_TransparentForMouseEvents);
    //parent->installEventFilter(this);

    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setMouseTracking(true);

    //QScreen* screen = QGuiApplication::primaryScreen();

    lb_move_handler_ = new QLabel(this);
    lb_move_handler_->setFont(font);
    lb_move_handler_->setAlignment(Qt::AlignCenter);
    lb_move_handler_->setText(QChar(0x270b));
    lb_move_handler_->resize(40, 40);
    lb_move_handler_->setMouseTracking(true);
    //lb_move_handler_->setGraphicsEffect(ef);

    lb_move_handler_->show();
    QGraphicsOpacityEffect* ef = new QGraphicsOpacityEffect();
    ef->setOpacity(0.7);
    this->setGraphicsEffect(ef);

    //int sideVal = GlobalValues::buttonBaseSize() * 0.6;
    //int sideVal = QApplication::fontMetrics().lineSpacing() * 2.2 * 0.6;
    int sideVal = 18;
    int handleSide = sideVal / 2;
    const QRect areaRect(0, 0, sideVal, sideVal);

    const QRect handleRect(0, 0, handleSide, handleSide);
    m_TLHandle = m_TRHandle = m_BLHandle = m_BRHandle = m_LHandle = m_THandle = m_RHandle = m_BHandle = handleRect;
    m_TLArea = m_TRArea = m_BLArea = m_BRArea = areaRect;

    move_handle_area_ = QRect(rect().topLeft().x() + rect().width() / 2 - 30, rect().topLeft().y() + rect().height() / 2 - 30, 60, 60);

    m_areaOffset = QPoint(-sideVal / 2, -sideVal / 2);
    m_handleOffset = QPoint(-handleSide / 2, -handleSide / 2);

    font.setPixelSize(16);
    font.setPointSize(16);

    pb_ok_ = new QPushButton(this);
    pb_ok_->setFont(font);
    pb_ok_->setText(QChar(0x2713));
    pb_ok_->resize(40, 40);
    pb_ok_->setAttribute(Qt::WA_TranslucentBackground);
    pb_ok_->show();


    pb_cancel_ = new QPushButton(this);
    pb_cancel_->setFont(font);
    pb_cancel_->setText(QChar(0x00d7));
    pb_cancel_->resize(40, 40);
    pb_cancel_->show();

    QObject::connect(pb_ok_, &QPushButton::clicked, this, &ScreenCaptureDialog::accept);
    QObject::connect(pb_cancel_, &QPushButton::clicked, this, &ScreenCaptureDialog::reject);

    this->setStyleSheet(ConfigHelper::getInstance()->getQssString(":/resouces/res/css/screen_capture_dialog.css"));
}

ScreenCaptureDialog::~ScreenCaptureDialog()
{

}


ScreenCaptureDialog::SideType ScreenCaptureDialog::getMouseSide(const QPoint& mousePos) const
{
    //if (!isVisible()) {
    //    return NO_SIDE;
    //}
    QPoint localPos = mapFromParent(mousePos);

    if (m_TLArea.contains(localPos)) {
        return TOPLEFT_SIDE;
    }
    else if (m_TRArea.contains(localPos)) {
        return TOPRIGHT_SIDE;
    }
    else if (m_BLArea.contains(localPos)) {
        return BOTTOMLEFT_SIDE;
    }
    else if (m_BRArea.contains(localPos)) {
        return BOTTOMRIGHT_SIDE;
    }
    else if (m_LArea.contains(localPos)) {
        return LEFT_SIDE;
    }
    else if (m_TArea.contains(localPos)) {
        return TOP_SIDE;
    }
    else if (m_RArea.contains(localPos)) {
        return RIGHT_SIDE;
    }
    else if (m_BArea.contains(localPos)) {
        return BOTTOM_SIDE;
    }
    else if (move_handle_area_.contains(localPos))
    {
        return CENTER;
    }
    else if (rect().contains(localPos)) 
    {
        //return CENTER;
    }
    else 
    {
        return NO_SIDE;
    }
}

QVector<QRect> ScreenCaptureDialog::handlerAreas()
{
    QVector<QRect> areas;
    areas << m_TLHandle << m_TRHandle << m_BLHandle << m_BRHandle << m_LHandle
        << m_THandle << m_RHandle << m_BHandle;
    return areas;
}

void ScreenCaptureDialog::updateAreas()
{
    QRect r = rect();
    m_TLArea.moveTo(r.topLeft() + m_areaOffset);
    m_TRArea.moveTo(r.topRight() + m_areaOffset);
    m_BLArea.moveTo(r.bottomLeft() + m_areaOffset);
    m_BRArea.moveTo(r.bottomRight() + m_areaOffset);


    m_LArea = QRect(m_TLArea.bottomLeft(), m_BLArea.topRight());
    m_TArea = QRect(m_TLArea.topRight(), m_TRArea.bottomLeft());
    m_RArea = QRect(m_TRArea.bottomLeft(), m_BRArea.topRight());
    m_BArea = QRect(m_BLArea.topRight(), m_BRArea.bottomLeft());

    m_TLHandle.moveTo(m_TLArea.center() + m_handleOffset);
    m_BLHandle.moveTo(m_BLArea.center() + m_handleOffset);
    m_TRHandle.moveTo(m_TRArea.center() + m_handleOffset);
    m_BRHandle.moveTo(m_BRArea.center() + m_handleOffset);
    m_LHandle.moveTo(m_LArea.center() + m_handleOffset);
    m_THandle.moveTo(m_TArea.center() + m_handleOffset);
    m_RHandle.moveTo(m_RArea.center() + m_handleOffset);
    m_BHandle.moveTo(m_BArea.center() + m_handleOffset);
    move_handle_area_ = QRect(rect().topLeft().x() + rect().width() / 2 - 30, 
                              rect().topLeft().y() + rect().height() / 2 - 30, 60, 60);
}

void ScreenCaptureDialog::updateCursor()
{
    SideType mouseSide = m_activeSide;
    if (!m_activeSide) {
        //mouseSide = getMouseSide(parentWidget()->mapFromGlobal(QCursor::pos()));
        mouseSide = getMouseSide(QCursor::pos());
    }

    switch (mouseSide) 
    {
        case TOPLEFT_SIDE:
            setCursor(Qt::SizeFDiagCursor);
            break;
        case BOTTOMRIGHT_SIDE:
            setCursor(Qt::SizeFDiagCursor);
            break;
        case TOPRIGHT_SIDE:
            setCursor(Qt::SizeBDiagCursor);
            break;
        case BOTTOMLEFT_SIDE:
            setCursor(Qt::SizeBDiagCursor);
            break;
        case LEFT_SIDE:
            setCursor(Qt::SizeHorCursor);
            break;
        case RIGHT_SIDE:
            setCursor(Qt::SizeHorCursor);
            break;
        case TOP_SIDE:
            setCursor(Qt::SizeVerCursor);
            break;
        case BOTTOM_SIDE:
            setCursor(Qt::SizeVerCursor);
            break;
        case MOVE_HANDLE:
            setCursor(Qt::SizeAllCursor);
            break;
        default:
            if (m_activeSide == CENTER) 
            {
                setCursor(Qt::ClosedHandCursor);
            }
            else 
            {
                setCursor(m_idleCentralCursor);
                return;
            }
            break;
    }
}

void ScreenCaptureDialog::paintEvent(QPaintEvent*)
{
    QPainter p(this);

    QPen pen = p.pen();
    pen.setStyle(Qt::DashLine);
    pen.setColor(m_color);
    p.setPen(pen);
    p.drawRect(rect() + QMargins(0, 0, -1, -1));
    p.setRenderHint(QPainter::Antialiasing);
    p.setBrush(m_color);

    for (auto rect : handlerAreas()) {

        pen = p.pen();
        pen.setStyle(Qt::SolidLine);
        pen.setColor("white");
        p.setPen(pen);

        p.setRenderHint(QPainter::Antialiasing);
        QBrush brush;
        brush.setStyle(Qt::SolidPattern);
        brush.setColor("white");
        p.setBrush(brush);

        p.drawEllipse(rect.center(), 6, 6);

        pen.setColor("orange");
        brush.setColor("orange");

        p.setPen(pen);
        p.setBrush(brush);

        p.drawEllipse(rect.center(), 4, 4);
    }
}

void ScreenCaptureDialog::resizeEvent(QResizeEvent*)
{
    updateAreas();

    lb_move_handler_->move(this->width() / 2 - 30, this->height() / 2 - 30);
    pb_ok_->move(this->width() - 92, this->height() - 52);
    pb_cancel_->move(this->width() - 50, this->height() - 52);
    //move_handle_area_ = QRect(this->width() / 2 - 30, this->height() / 2 - 30, 60, 60);
}

void ScreenCaptureDialog::moveEvent(QMoveEvent* ev)
{
    //updateAreas();
    QWidget::moveEvent(ev);
}

void ScreenCaptureDialog::mousePressEvent(QMouseEvent* e)
{
    if (e->button() != Qt::LeftButton) {
        return;
    }

    m_dragStartPos = mapToGlobal(e->pos());
    m_activeSide = getMouseSide(parentWidget()->mapFromGlobal(QCursor::pos()));
}

void ScreenCaptureDialog::mouseReleaseEvent(QMouseEvent* e)
{
    // released outside of the selection area

    m_activeSide = NO_SIDE;
    updateCursor();
    is_resize_stop_ = false;
    //emit geometrySettled();
}

void ScreenCaptureDialog::mouseMoveEvent(QMouseEvent* e)
{
    updateCursor();

    if (e->buttons() != Qt::LeftButton) {
        return;
    }

    SideType mouseSide = m_activeSide;
    if (!m_activeSide) {
        mouseSide = getMouseSide(e->pos());
    }


    bool is_minimum_width = width() == minimumWidth();
    bool is_minimum_height = height() == minimumHeight();

    QPoint pos = mapToGlobal(e->pos());
    auto geom = geometry();


    QPoint newTopLeft = geom.topLeft(), newBottomRight = geom.bottomRight();

    int& newLeft = newTopLeft.rx(), & newRight = newBottomRight.rx(),
        & newTop = newTopLeft.ry(), & newBottom = newBottomRight.ry();
    switch (mouseSide) 
    {
    case TOPLEFT_SIDE:
        if (m_activeSide) 
        {
            if ((pos.x() > geom.x() && is_minimum_width))
            {
                return;
            }
            if ((pos.y() > geom.y() && is_minimum_height))
            {
                return;
            }
            newTopLeft = pos;
        }
        break;
    case BOTTOMRIGHT_SIDE:
        if (m_activeSide) 
        {
            if ((pos.x() < geom.x() + minimumWidth() && is_minimum_width))
            {
                return;
            }

            if ((pos.y() > geom.y() && is_minimum_height))
            {
                return;
            }
            newBottomRight = pos;
        }
        break;
    case TOPRIGHT_SIDE:
        if (m_activeSide) 
        {
            if ((pos.x() < geom.x() + minimumWidth() && is_minimum_width))
            {
                return;
            }
            if ((pos.y() > geom.y() && is_minimum_height))
            {
                return;
            }
            newTop = pos.y();
            newRight = pos.x();
        }
        break;
    case BOTTOMLEFT_SIDE:
        if (m_activeSide) 
        {
            if ((pos.x() > geom.x() && is_minimum_width))
            {
                return;
            }

            if ((pos.y() < geom.y() + minimumHeight() && is_minimum_height))
            {
                return;
            }
            newBottom = pos.y();
            newLeft = pos.x();
        }
        break;
    case LEFT_SIDE:
        if (m_activeSide) 
        {
            if ((pos.x() > geom.x() && is_minimum_width))
            {
                return;
            }
            newLeft = pos.x();
        }
        break;
    case RIGHT_SIDE:
        if (m_activeSide) 
        {
            if ((pos.x() < geom.x() + minimumWidth() && is_minimum_width))
            {
                return;
            }
            newRight = pos.x();
        }
        break;
    case TOP_SIDE:
        if (m_activeSide) 
        {
            if ((pos.y() > geom.y() && is_minimum_height))
            {
                return;
            }
            newTop = pos.y();
        }
        break;
    case BOTTOM_SIDE:
        if (m_activeSide) 
        {
            if ((pos.y() < geom.y() + minimumHeight() && is_minimum_height))
            {
                return;
            }
            newBottom = pos.y();
        }
        break;
    default:
        if (m_activeSide == CENTER) 
        {
            move(this->pos() + pos - m_dragStartPos);
            m_dragStartPos = pos;
        }
        return;
    }
    // finalize geometry change
    if (m_activeSide) 
    {
        geom = { newTopLeft, newBottomRight };
        setGeometry(geom);
        //setGeometry(geom.normalized());
    }
    //m_dragStartPos = pos;
}

// helper function
ScreenCaptureDialog::SideType ScreenCaptureDialog::getProperSide(ScreenCaptureDialog::SideType side, const QRect& r)
{
    using SideType = ScreenCaptureDialog::SideType;
    int intSide = side;
    if (r.right() < r.left()) {
        intSide ^= SideType::LEFT_SIDE;
        intSide ^= SideType::RIGHT_SIDE;
    }
    if (r.bottom() < r.top()) {
        intSide ^= SideType::TOP_SIDE;
        intSide ^= SideType::BOTTOM_SIDE;
    }

    return (SideType)intSide;
}

void ScreenCaptureDialog::setGeometry(const QRect& r)
{
    QWidget::setGeometry(r + QMargins(MARGIN, MARGIN, MARGIN, MARGIN));
    updateCursor();
}

QRect ScreenCaptureDialog::geometry() const
{
    return QWidget::geometry() - QMargins(MARGIN, MARGIN, MARGIN, MARGIN);
}

QRect ScreenCaptureDialog::rect() const
{
    return QWidget::rect() - QMargins(MARGIN, MARGIN, MARGIN, MARGIN);
}

QRect ScreenCaptureDialog::getCaptureArea()
{
    auto tl = this->geometry().topLeft();
    tl += m_TLArea.center() + m_handleOffset;

    auto br = this->geometry().bottomRight();
    br += m_BRArea.center() + m_handleOffset;

    return QRect(tl, br);
    //return QRect(m_TLArea.center() + m_handleOffset, m_BRArea.center() + m_handleOffset);
}