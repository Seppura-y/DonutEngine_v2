//#pragma once
//#include <QDialog>
//#include <QTabWidget>
//#include <QLineEdit>
//#include <QComboBox>
//#include <QButtonGroup>
//#include <QCheckBox>
//#include <QLineEdit>
//
////#include "capturer/media_info.h"
////#include "ui_desktop_tab.h"
////#include "ui_media_src_dialog.h"
//
//enum capture_type
//{
//    CAP_TYPE_VIDEO = 0,
//    CAP_TYPE_AUDIO
//};
//
//
//
//
//class FileTab : public QWidget
//{
//    Q_OBJECT
//public:
//    explicit FileTab(QWidget* parent = nullptr);
//
//public:
//    QLineEdit* edit;
//    QPushButton* btnBrowse;
//};
//
//class CaptureTab : public QWidget
//{
//    Q_OBJECT
//public:
//    explicit CaptureTab(QWidget* parent = nullptr, capture_type type = CAP_TYPE_VIDEO);
//
//public:
//    QComboBox* cb_dev_name_ = nullptr;
//    QComboBox* cb_dev_resolution_ = nullptr;
//    capture_type cap_type_;
//};
//
//class DesktopTab : public QWidget
//{
//    Q_OBJECT
//public:
//    explicit DesktopTab(QWidget* parent = nullptr, capture_type type = CAP_TYPE_VIDEO);
//    ~DesktopTab();
//
//signals:
//    void sigDeskTopTabProcessingScreenCap();
//    void sigDeskTopTabProcessingScreenCapFinshed();
//protected slots:
//    //void onCaptureAreaChecked();
//    void onCaptureAreaChecked(int index);
//protected:
//    void connectSignals();
//public:
//    QButtonGroup* group_type_ = nullptr;
//    QButtonGroup* group_size_ = nullptr;
//    QCheckBox* ck_dxgi_ = nullptr;
//    QCheckBox* ck_dshow_ = nullptr;
//    QCheckBox* ck_gdi_ = nullptr;
//    QCheckBox* ck_full_screen_ = nullptr;
//    QCheckBox* ck_capture_area_ = nullptr;
//
//    QLineEdit* le_width_ = nullptr;
//    QLineEdit* le_height_ = nullptr;
//    QLineEdit* le_x_ = nullptr;
//    QLineEdit* le_y_ = nullptr;
//    capture_type cap_type_;
//
//    QPushButton* pb_ok_ = nullptr;
//    QPushButton* pb_cancel_ = nullptr;
//
//    QRect screen_rect_;
//
//private:
//    Ui::DesktopTab ui;
//};
//
//
//
//class MediaSrcDialog : public QDialog
//{
//    Q_OBJECT
//
//public:
//    MediaSrcDialog(QWidget* parent = nullptr);
//
//    MediaInfo getMediaInfo();
//signals:
//    void sigMediaDialogProcessScreenCap();
//    void sigMediaDialogProcessScreenCapFinished();
//public slots:
//    virtual void accept() override;
//
//protected:
//    void initUI();
//    void initConnect();
//
//public:
//    QTabWidget* tab;
//    MediaInfo media_info_;
//    DesktopTab* desktop_tab_ = nullptr;
//private:
//
//};