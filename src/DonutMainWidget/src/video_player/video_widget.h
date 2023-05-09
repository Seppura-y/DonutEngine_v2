#ifndef VIDEO_WIDGET_H
#define VIDEO_WIDGET_H

#include <QWidget>
#include <QTime>
#include <memory>
#include <QTimer>
#include <QMenu>
#include <QAbstractNativeEventFilter>

#include "vlc_manager.h"
//class VLCManager;

class VideoWidget : public QWidget
{
    Q_OBJECT

public:
    VideoWidget(QWidget* parent = nullptr);
    VideoWidget(int row, int colum, int width = 1, int height = 1, QWidget* parent = nullptr);
    ~VideoWidget();

    void openMediaFile(QString file_path);
    void setMediaPause(bool pause);
    void setSeekPos(double value);
    void setSoundVolume(int value);
    void setMediaStop();

    void setLayoutInfo(int row, int colum, int width = 1, int height = 1) { row_ = row; colum_ = colum; width_ = width; height_ = height; };
    bool getFullscreen();

public:
    ////////////////////////////////////////////////////////////////////////
    // For split
    bool isCovered() { return is_covered_; }
    bool isSelected() { return is_selected_; }
    bool isFirstPoint() { return is_first_point_; }

    void setCovered(bool status) { is_covered_ = status; }
    void setSelected(bool status) { is_selected_ = status; }
    void setFirstPoint(bool status) { is_first_point_ = status; }

    int getWidth() { return width_; }
    int getHeight() { return height_; }

    int getRow() { return row_; }
    int getColum() { return colum_; }

    int getBlockRow() { return block_row_; }
    int getBlockColum() { return block_colum_; }

    void setBlockRow(int row) { block_row_ = row; }
    void setBlockColum(int col) { block_colum_ = col; }

signals:
    void sigOpenMediaFileSuccess();
    void sigUpdateTotalDuration(QTime time);
    void sigUpdateCurrentTimePos(QTime time);
    void sigUpdateCurrentSoundVolume(int value);
    void sigRenderMediaEndReached();

    void sigShowFullscreen(bool);

    //////////////////////////////////////////////////////////////
    // For split
    void sigSelected(int, int);
public slots:
    //void onOpenMediaFile(QString file_path);


    void onRefreshUi();
private slots:
    void onMouseHideTimeout();

protected:
    void paintEvent(QPaintEvent* ev) override;
    void mouseMoveEvent(QMouseEvent* ev) override;
    void dragEnterEvent(QDragEnterEvent* ev)override;
    void dropEvent(QDropEvent* ev) override;
    //void mouseDoubleClickEvent(QMouseEvent* ev) override;
    //bool nativeEventFilter(const QByteArray& eventType, void* message, long* result) override;

    //////////////////////////////////////////////////////////////
    // For split
    void mousePressEvent(QMouseEvent* ev) override;

private:
    void initVlcManager();
private:

    bool media_pause_ = false;

    //std::unique_ptr <VlcManager> render_manager_;
    VlcManager* render_manager_ = nullptr;

    QTimer timer_mouse_hide_;
    QTimer timer_fullscreen_;
    bool is_mouse_hide_ = false;
    bool is_fullscreen_ = false;

    QString media_url_;

    ///////////////////////////////////////////////////////////////////
    // For Split
    bool is_covered_ = false;
    bool is_selected_ = false;
    bool is_first_point_ = false;

    int width_ = -1;
    int height_ = -1;
    int colum_ = -1;
    int row_ = -1;

    int block_row_ = -1;
    int block_colum_ = -1;

    int item_type_ = -1;
    int item_index_ = -1;
    QString name_;
    QString url_;
    QString sub_url_;
    QMenu menu_;
};

#endif