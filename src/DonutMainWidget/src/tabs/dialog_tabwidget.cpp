#include "dialog_tabwidget.h"
#include <QLayout>
#include <QFontDatabase>
#include <QStylePainter>
#include <QStyleOptionTab>

#include "config_helper.h"


DialogTabBar::DialogTabBar(QWidget* a_parent) : QTabBar(a_parent)
{
    setWidth(size().width());

    int id = QFontDatabase::addApplicationFont(":/resources/res/fonts/Font Awesome 6 Pro-Light-300.otf");
    if (id < 0)
    {

    }
    else
    {
        QFont font;
        QString family = QFontDatabase::applicationFontFamilies(id).first();
        font.setFamily(family);

        font.setPixelSize(16);
        font.setPointSize(16);
        this->setFont(font);
    }
}

QSize DialogTabBar::tabSizeHint(int index) const
{
    QSize s = QTabBar::tabSizeHint(index);
    s.transpose();

    s.rwidth() = 90;
    s.rheight() = 50;
    return s;
}

void DialogTabBar::paintEvent(QPaintEvent* ev)
{
    QStylePainter painter(this);
    QStyleOptionTab opt;

    for (int i = 0; i < count(); i++)
    {
        initStyleOption(&opt, i);
        painter.drawControl(QStyle::CE_TabBarTabShape, opt);
        painter.save();

        QSize s = opt.rect.size();
        s.transpose();
        QRect r(QPoint(), s);
        r.moveCenter(opt.rect.center());
        opt.rect = r;

        QPoint c = tabRect(i).center();
        painter.translate(c);
        painter.rotate(90);
        painter.translate(-c);
        painter.drawControl(QStyle::CE_TabBarTabLabel, opt);
        painter.restore();
    }
}

void DialogTabBar::setWidth(int a_width)
{
    size_ = QSize(a_width, size().height());
}

DialogTabWidget::DialogTabWidget(QWidget* a_parent) : QTabWidget(a_parent)
{
    tab_bar_ = new DialogTabBar(this);
    setTabBar(tab_bar_);
    setTabPosition(QTabWidget::West);
    this->setDocumentMode(true);
    this->setStyleSheet(ConfigHelper::getInstance()->getQssString(":resources/res/css/camera_menu.css"));
}

DialogTabWidget::~DialogTabWidget()
{
    if (tab_bar_)
    {
        delete tab_bar_;
    }
}

void DialogTabWidget::resizeEvent(QResizeEvent* e)
{
    tab_bar_->setWidth(size().width());
    QTabWidget::resizeEvent(e);
}