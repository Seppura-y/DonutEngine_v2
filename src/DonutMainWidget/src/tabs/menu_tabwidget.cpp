#include "menu_tabwidget.h"
#include <QLayout>
#include <QFontDatabase>
#include <QStylePainter>
#include <QStyleOptionTab>

#include "config_helper.h"

MenuTabBar::MenuTabBar(QWidget* a_parent) : QTabBar(a_parent)
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

QSize MenuTabBar::tabSizeHint(int index) const
{
    return QSize(size_.width() / (count() ? count() : 1), size().height());
}

void MenuTabBar::paintEvent(QPaintEvent* ev)
{
    return QTabBar::paintEvent(ev);
}

void MenuTabBar::setWidth(int a_width)
{
    size_ = QSize(a_width, size().height());
}

MenuTabWidget::MenuTabWidget(QWidget* a_parent) : QTabWidget(a_parent)
{
    tab_bar_ = new MenuTabBar(this);
    setTabBar(tab_bar_);
    setTabPosition(QTabWidget::West);
    this->setDocumentMode(true);
    this->setStyleSheet(ConfigHelper::getInstance()->getQssString(":resources/res/css/camera_menu.css"));
}

MenuTabWidget::~MenuTabWidget()
{
    if (tab_bar_)
    {
        delete tab_bar_;
    }
}

void MenuTabWidget::resizeEvent(QResizeEvent* e)
{
    tab_bar_->setWidth(size().width());
    QTabWidget::resizeEvent(e);
}
