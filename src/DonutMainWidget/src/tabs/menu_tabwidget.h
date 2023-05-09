#pragma once
#include <QTabWidget>
#include <QTabBar>

class MenuTabBar : public QTabBar
{
public:
    MenuTabBar(QWidget* a_parent);
    void setWidth(int a_width);
protected:
    QSize tabSizeHint(int index) const  override;
    void paintEvent(QPaintEvent* ev) override;
private:
    QSize size_;
};


class MenuTabWidget : public QTabWidget
{
	Q_OBJECT

public:
    MenuTabWidget(QWidget* a_parent = nullptr);
    ~MenuTabWidget();
    void resizeEvent(QResizeEvent* e) override;
protected:

private:
    MenuTabBar* tab_bar_ = nullptr;

};

