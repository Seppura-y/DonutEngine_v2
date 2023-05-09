#include "splitter_scroll_area.h"
#include <QAction>
#include <QDebug>
#include <QScrollBar>

#include "config_helper.h"
SplitterScrollArea::SplitterScrollArea(QWidget* parent) : QScrollArea(parent)
{
    QAction* act = this->menu_.addAction(QString::fromLocal8Bit("merge"));
    QObject::connect(act, SIGNAL(triggered()), this, SLOT(onMerge()));

    act = this->menu_.addAction(QString::fromLocal8Bit("split"));
    QObject::connect(act, SIGNAL(triggered()), this, SLOT(onSplit()));

    act = this->menu_.addAction(QString::fromLocal8Bit("clear"));
    QObject::connect(act, SIGNAL(triggered()), this, SLOT(onClear()));

    //act = this->menu_.addAction(QString::fromLocal8Bit("save"));
    //QObject::connect(act, SIGNAL(triggered()), this, SLOT(onSave()));

    //act = this->menu_.addAction(QString::fromLocal8Bit("load"));
    //QObject::connect(act, SIGNAL(triggered()), this, SLOT(onLoad()));

    //act = this->menu_.addAction(QString::fromLocal8Bit("send"));
    //QObject::connect(act, SIGNAL(triggered()), this, SLOT(onSendfile()));

    //act = this->menu_.addAction(QString::fromLocal8Bit("set ip"));
    //QObject::connect(act, SIGNAL(triggered()), this, SLOT(onSetaddr()));

    //act = this->menu_.addAction(QString::fromLocal8Bit("reset svr"));
    //QObject::connect(act, SIGNAL(triggered()), this, SLOT(onRstsvr()));

    this->setStyleSheet(ConfigHelper::getQssString(":/resources/res/css/my_scrollbar.css"));
}

void SplitterScrollArea::contextMenuEvent(QContextMenuEvent* ev)
{
    this->menu_.exec(QCursor::pos());
    ev->accept();
}

void SplitterScrollArea::onMerge()
{
    emit sigMerge();
}

void SplitterScrollArea::onSplit()
{
    emit sigSplit();
}

void SplitterScrollArea::onClear()
{
    emit sigClear();
}

void SplitterScrollArea::onSave()
{
    emit sigSave();
}
void SplitterScrollArea::onLoad()
{
    emit sigLoad();
}
void SplitterScrollArea::onRstsvr()
{
    emit sigRstsvr();
}

void SplitterScrollArea::onSendfile()
{
    emit sigSendfile();
}

void SplitterScrollArea::onSetaddr()
{
    emit sigSetaddr();
}

void SplitterScrollArea::onSetControlerMode(bool status)
{
    if (status)
    {
        menu_.setEnabled(true);
    }
    else
    {
        menu_.setEnabled(false);
    }
}

void SplitterScrollArea::setMergeEnable(bool en)
{
    menu_.actions().at(0)->setEnabled(en);
}

void SplitterScrollArea::setSplitEnable(bool en)
{
    menu_.actions().at(1)->setEnabled(en);
}

int SplitterScrollArea::getHorizontalScrollbarHeight()
{
    return this->horizontalScrollBar()->height();
}
