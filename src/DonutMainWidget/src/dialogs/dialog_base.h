#pragma once
#include <QDialog>
#include "dialog_title.h"
#include "dialog_content_widgets.h"


class DialogBase : public QDialog
{
    Q_OBJECT

public:
    DialogBase(QWidget* parent = nullptr);
    ~DialogBase();


    void setTitleText(const QString& str);
signals:

public slots:
    virtual void accept() override;

protected:
    virtual void initUI() = 0;
    void initConnect();

    void mousePressEvent(QMouseEvent* ev) override;
    void mouseMoveEvent(QMouseEvent* ev) override;
    void paintEvent(QPaintEvent* ev) override;
protected:
    DialogTitle* title_ = nullptr;
    QWidget* content_widget_ = nullptr;
    bool left_bt_pressed_ = false;
    QPoint mouse_drag_point_;
};

class MediaFileDialog : public DialogBase
{
    Q_OBJECT

public:
    MediaFileDialog(QWidget* parent = nullptr);
    ~MediaFileDialog();

    QString getFilePath();
    QString getFileName();
protected:
    virtual void initUI() override;
private:
    QString file_name_;
    QString file_path_;
};