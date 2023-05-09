#ifndef SPLITTER_WIDGET_H
#define SPLITTER_WIDGET_H

#include <QWidget>
#include <map>

class QGridLayout;
class SplitterScrollArea;
class RenderWidget;

class SplitterWidget : public QWidget
{
	Q_OBJECT

public:
	SplitterWidget(QWidget* parent = nullptr);
	virtual ~SplitterWidget();

    void initUi();
	void initSplitterContent();

    void resetSplitterContent();

    void getRange();

    void setWidgetsLayout(int count);
signals:
    

protected slots:
    void onMerge();
    void onSplit();
    void onClear();
    void onSave();
    void onLoad();
    void onSelectedUpdate();
    void onRefreshUi(int row, int colum);
public slots:
    void onSetControlerMode(bool status);
    void onResetContent(int count);
    void onResetSplitterContent(int count);
protected:

    void resizeEvent(QResizeEvent* ev) override;
    void timerEvent(QTimerEvent* ev) override;
    void mousePressEvent(QMouseEvent* ev) override;
    void mouseMoveEvent(QMouseEvent* ev) override;
    void mouseReleaseEvent(QMouseEvent* ev) override;


private:
    void clearSelection();
private:
    QWidget* content_widget_ = nullptr;
    SplitterScrollArea* scroll_area_ = nullptr;
    QGridLayout* grid_layout_ = nullptr;

    bool is_leftbutton_pressed_ = false;
    bool first_point_selected_ = false;
    bool second_point_selected_ = false;
    bool is_client_connected_ = false;

    int first_colum_ = -1;
    int first_row_ = -1;
    int second_colum_ = -1;
    int second_row_ = -1;

    int start_colum_ = -1;
    int start_row_ = -1;
    int selected_width_ = -1;
    int selected_height_ = -1;
    int end_colum_ = -1;
    int end_row_ = -1;

    bool is_need_init_ = true;
    int grid_width_ = -1;
    int grid_height_ = -1;
    int grid_rows_ = -1;
    int grid_colums_ = -1;

    int controler_width_ = 5760;
    int controler_height_ = 1080;
    int controler_width_prev_ = -1;
    int controler_height_prev_ = -1;

    int current_widgets_count_ = 1;
    bool is_controler_mode_ = false;
    bool is_customlayout_mode_ = false;
    bool is_content_init_ = false;

    std::map<std::pair<int, int>, RenderWidget*> render_widgets_;
};

#endif