#pragma once
#include <QTabWidget>
#include <QTabBar>

class DialogTabBar : public QTabBar
{
public:
    DialogTabBar(QWidget* a_parent);
    void setWidth(int a_width);
protected:
    QSize tabSizeHint(int index) const  override;
    void paintEvent(QPaintEvent* ev) override;
private:
    QSize size_;
};


class DialogTabWidget : public QTabWidget
{
	Q_OBJECT

public:
    DialogTabWidget(QWidget* a_parent = nullptr);
    ~DialogTabWidget();
    void resizeEvent(QResizeEvent* e) override;
protected:

private:
    DialogTabBar* tab_bar_ = nullptr;

};

