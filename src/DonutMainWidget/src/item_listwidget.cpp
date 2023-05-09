#include "item_listwidget.h"
#include "config_helper.h"

#include <QMouseEvent>
#include <QApplication>
#include <QDrag>
#include <QMimeData>
#include <QJsonDocument>
#include <QJsonObject>
#include <QByteArray>
#include <QScrollbar>
#include <QDebug>


ItemListWidget::ItemListWidget(int item_type,QWidget* parent) : QListWidget(parent)
{
	item_type_ = item_type;
    this->verticalScrollBar()->setStyleSheet(ConfigHelper::getQssString(":/resources/res/css/my_scrollbar.css"));
    this->horizontalScrollBar()->setStyleSheet(ConfigHelper::getQssString(":/resources/res/css/my_scrollbar.css"));
    this->setSelectionMode(SelectionMode::SingleSelection);
    this->setDefaultDropAction(Qt::CopyAction);
    this->setViewMode(QListView::ListMode);
    this->setDragDropMode(QAbstractItemView::InternalMove);
    this->setDragEnabled(true);
    //QObject::connect(this, &QListWidget::currentRowChanged, this, [this](int row) {qDebug() << "currentRowChanged : " << row; });
    //QObject::connect(this->model(), SIGNAL(this->model()->rowsMoved()), this, [] {qDebug() << "rows moved"; });
    QObject::connect(this->model(), &QAbstractItemModel::rowsMoved, this, 
        [this](const QModelIndex& parent, int start, int end, const QModelIndex& destination, int row) 
        {
            qDebug() << "rows moved " << " start : " << start << " end : " << end << " row : " << row << "destination row : " << parent.row();

        }
    );
}


ItemListWidget::ItemListWidget(QWidget* parent) : QListWidget(parent)
{

}

void ItemListWidget::mousePressEvent(QMouseEvent* ev)
{
    //Make sure to drag with the left button.
    if (ev->button() == Qt::LeftButton)
    {
        //Save the starting point of the drag first.
        drag_point_ = ev->pos();
        //Keep the dragged item.
        //drag_item_ = new QListWidgetItem(*this->itemAt(ev->pos()));
        drag_item_ = this->itemAt(ev->pos());
        //is_left_pressed_ = true;
    }
    //else
    //{
    //    is_left_pressed_ = false;
    //}
    //Retain the mouse click operation of the original QListWidget widget.
    QListWidget::mousePressEvent(ev);
    //is_left_pressed_ = false;
}

void ItemListWidget::mouseMoveEvent(QMouseEvent* ev)
{
    //Make sure to hold down the left button to move.
    if (ev->buttons() & Qt::LeftButton)
    {
        QPoint temp = ev->pos() - drag_point_;
        //Only this length is greater than the default distance, it will be considered by the system as a drag operation.
        if (temp.manhattanLength() > QApplication::startDragDistance())
        {
            QJsonObject send;
            ConfigHelper* conf = ConfigHelper::getInstance();
            QJsonObject* obj = conf->getObject((ConfigHelper::JsonObjType)this->item_type_);
            if (drag_item_ == nullptr || obj == nullptr)
            {
                return;
            }
            QString name = drag_item_->text();
            if (obj->contains(name))
            {
                send = obj->find(name).value().toObject();
            }

            QJsonDocument doc(send);
            QByteArray arr = doc.toJson();

            QDrag* drag = new QDrag(this);
            QMimeData* mimeData = new QMimeData;
            mimeData->setData("application/json", arr);

            drag->setMimeData(mimeData);

            //QListWidget::mouseMoveEvent(ev);

            auto action = drag->exec(Qt::CopyAction | Qt::MoveAction);

            if (action == (Qt::CopyAction) || (action == Qt::MoveAction))
            {
                //After successfully dragging, delete the dragged item.
                //auto i = this->takeItem(this->row(m_dragItem));
                //delete i;
            }
        }
        else
        {
            QListWidget::mouseMoveEvent(ev);
        }
    }
}

void ItemListWidget::dropEvent(QDropEvent* ev)
{
    if (ev->source() == this)
    {
        ev->setDropAction(Qt::DropAction::MoveAction);
        this->setDragDropMode(QAbstractItemView::InternalMove);
    }
    else
    {
        ev->setDropAction(Qt::DropAction::CopyAction);
        this->setDragDropMode(QAbstractItemView::DragDrop);
    }
    //ev->acceptProposedAction();
    return QListWidget::dropEvent(ev);
}

void ItemListWidget::dragEnterEvent(QDragEnterEvent* ev)
{
    //ev->acceptProposedAction();
    QListWidget::dragEnterEvent(ev);
}

void ItemListWidget::dragMoveEvent(QDragMoveEvent* ev)
{
    //ev->acceptProposedAction();
    QListWidget::dragMoveEvent(ev);
}

//void ItemListWidget::keyPressEvent(QKeyEvent* ev)
//{
//    ev->ignore();
//    return;
//}
