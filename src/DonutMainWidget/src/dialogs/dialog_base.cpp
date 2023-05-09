#include "dialog_base.h"

#include <QLabel>
#include <QFileDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialogButtonBox>
#include <QCheckBox>
#include <QButtonGroup>
#include <QDebug>
#include <QScreen>
#include <QMouseEvent>
#include <QList>
#include <QStyleOption>
#include <QPainter>

#include <sstream>

#include "config_helper.h"
#include "dialog_tabwidget.h"

DialogBase::DialogBase(QWidget* parent) : QDialog(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    //setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinMaxButtonsHint | Qt::WindowStaysOnTopHint);
    this->setStyleSheet(ConfigHelper::getInstance()->getQssString(":/resources/res/css/dialog_base.css"));
    //initUI();
}

DialogBase::~DialogBase()
{

    if (title_)
    {
        delete title_;
    }

    if (content_widget_)
    {
        delete content_widget_;
    }

    qDebug() << "dialog base destruct";
}

void DialogBase::setTitleText(const QString& str)
{
    if (title_)
    {
        title_->setTitleText(str);
    }
}

void DialogBase::paintEvent(QPaintEvent* ev)
{
    QStyleOption option;
    option.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &option, &painter, this);
    QWidget::paintEvent(ev);
}

void DialogBase::initUI()
{


    //setFixedSize(600, 380);


    //title_ = new DialogTitle();

    //QPushButton* pb_ok = new QPushButton();
    //QPushButton* pb_cancel = new QPushButton();
    //pb_ok->setFixedSize(60, 30);
    //pb_cancel->setFixedSize(60, 30);

    //pb_ok->setText("ok");
    //pb_cancel->setText("cancel");

    //QHBoxLayout* pb_layout = new QHBoxLayout();
    //QLabel* lb_empty = new QLabel();
    //lb_empty->setStyleSheet("background-color: #1c1c1c;");
    //lb_empty->setMinimumWidth(this->width());
    //
    //pb_layout->setSizeConstraint(QLayout::SetMaximumSize);
    //pb_layout->addSpacerItem(new QSpacerItem(150, 20, QSizePolicy::Expanding, QSizePolicy::Expanding));
    //pb_layout->addWidget(lb_empty, 1);
    //pb_layout->addWidget(pb_ok);
    //pb_layout->addWidget(pb_cancel);
    //QObject::connect(pb_ok, &QPushButton::clicked, this, &QDialog::accept);
    //QObject::connect(pb_cancel, &QPushButton::clicked, this, &QDialog::reject);

    //content_widget_ = new QWidget();

    //QVBoxLayout* vbox = new QVBoxLayout();
    //vbox->setContentsMargins(0, 0, 0, 0);
    //vbox->setSpacing(0);
    //vbox->addWidget(title_);
    //vbox->addWidget(content_widget_,1);
    //vbox->addLayout(pb_layout,0);
    //this->setLayout(vbox);
    ////this->setLayout(&pb_layout);

    //QObject::connect(title_, &DialogTitle::sigCloseBtnClicked, this, &QDialog::reject);
}

void DialogBase::initConnect() 
{

}

void DialogBase::mousePressEvent(QMouseEvent* ev)
{
    if (ev->button() == Qt::LeftButton)
    {
        left_bt_pressed_ = true;
        mouse_drag_point_ = ev->pos();
    }
    return QDialog::mousePressEvent(ev);
}

void DialogBase::mouseMoveEvent(QMouseEvent* ev)
{
    if (title_->geometry().contains(mouse_drag_point_) && left_bt_pressed_)
    {
        this->move(ev->globalPos() - mouse_drag_point_);
    }
    return QDialog::mouseMoveEvent(ev);
}

void DialogBase::accept() 
{
    QDialog::accept();
}

MediaFileDialog::MediaFileDialog(QWidget* parent)
{
    initUI();
}

MediaFileDialog::~MediaFileDialog()
{
}

QString MediaFileDialog::getFilePath()
{
    return file_path_;
}

QString MediaFileDialog::getFileName()
{
    return file_name_;
}

void MediaFileDialog::initUI()
{

    setFixedSize(600, 380);


    title_ = new DialogTitle();

    QPushButton* pb_ok = new QPushButton();
    QPushButton* pb_cancel = new QPushButton();
    pb_ok->setFixedSize(60, 30);
    pb_cancel->setFixedSize(60, 30);

    pb_ok->setText("ok");
    pb_cancel->setText("cancel");

    QHBoxLayout* pb_layout = new QHBoxLayout();
    QLabel* lb_empty = new QLabel();
    lb_empty->setStyleSheet("background-color: #1c1c1c;");
    lb_empty->setMinimumWidth(this->width());

    pb_layout->setSizeConstraint(QLayout::SetMaximumSize);
    pb_layout->addSpacerItem(new QSpacerItem(150, 20, QSizePolicy::Expanding, QSizePolicy::Expanding));
    //pb_layout->addWidget(lb_empty, 1);
    pb_layout->addWidget(pb_ok);
    pb_layout->addWidget(pb_cancel);

    QObject::connect(pb_ok, &QPushButton::clicked, this, &QDialog::accept);
    QObject::connect(pb_cancel, &QPushButton::clicked, this, &QDialog::reject);


    //DialogTabWidget* tab_ = new DialogTabWidget();
    //DesktopTab* desktop_tab_ = new DesktopTab(0);
    //CaptureTab* capture_tab_ = new CaptureTab(1);
    //capture_tab_->setStyleSheet("DialogContentBase{border-radius: 0px;}");
    //tab_->addTab(capture_tab_, tr("Camera"));
    //tab_->addTab(desktop_tab_, tr("Desktop"));

    //content_widget_ = tab_;

    FileTab* tab_ = new FileTab();
    content_widget_ = tab_;

    QObject::connect(tab_, &FileTab::sigAcceptFilePath, [&](QString file)
        {
            file_path_ = file;
            file_name_ = file;
        }
    );

    QObject::connect(tab_, &FileTab::sigAcceptFileName, [&](QString name)
        {
            file_name_ = name;
        }
    );

    QVBoxLayout* vbox = new QVBoxLayout();
    vbox->setContentsMargins(0, 0, 0, 0);
    vbox->setSpacing(0);
    vbox->addWidget(title_);
    vbox->addWidget(content_widget_, 1);
    vbox->addLayout(pb_layout, 0);
    this->setLayout(vbox);
    //this->setLayout(&pb_layout);

    QObject::connect(title_, &DialogTitle::sigCloseBtnClicked, this, &QDialog::reject);
}
