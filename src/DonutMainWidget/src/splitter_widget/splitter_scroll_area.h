#ifndef SPLITTER_SCROLL_AREA_H
#define SPLITTER_SCROLL_AREA_H
#include <QScrollArea>
#include <QMenu>
#include <QContextMenuEvent>
class SplitterScrollArea : public QScrollArea
{
	Q_OBJECT
public:
	SplitterScrollArea(QWidget* parent = nullptr);

	void contextMenuEvent(QContextMenuEvent* ev) override;

	void setMergeEnable(bool en = true);
	void setSplitEnable(bool en = true);
	int getHorizontalScrollbarHeight();
signals:
	void sigMerge();
	void sigSplit();
	void sigClear();
	void sigSave();
	void sigLoad();
	void sigRstsvr();
	void sigSendfile();
	void sigSetaddr();

public slots:
	void onMerge();
	void onSplit();
	void onClear();
	void onSave();
	void onLoad();
	void onRstsvr();
	void onSendfile();
	void onSetaddr();

	void onSetControlerMode(bool status);
private:
	QMenu menu_;
};

#endif