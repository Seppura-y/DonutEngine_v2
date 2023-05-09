#pragma once
#include "config_helper.h"
#include <QListWidget.h>
#include <QListWidgetItem>
#include <QPoint>
#include <QDropEvent>
#include <QDrag>

class ItemListWidget : public QListWidget
{
	Q_OBJECT

public:
	explicit ItemListWidget(int itemType,QWidget* parent = nullptr);
	explicit ItemListWidget(QWidget* parent = nullptr);

	void setItemType(int type) { item_type_ = type; }
	int getItemType() { return item_type_; }
	int item_type_ = -1;

signals:
	
protected:
	void mousePressEvent(QMouseEvent* ev) override;
	void mouseMoveEvent(QMouseEvent* ev) override;
	void dropEvent(QDropEvent* ev) override;
	void dragEnterEvent(QDragEnterEvent* ev) override;
	void dragMoveEvent(QDragMoveEvent* ev) override;
	//void keyPressEvent(QKeyEvent* ev) override;
private:

    bool is_drag_outside_ = false;
    bool is_left_pressed_ = false;
    QPoint drag_point_;
    QDrag* drag_ = nullptr;
    QListWidgetItem* drag_item_ = nullptr;
};