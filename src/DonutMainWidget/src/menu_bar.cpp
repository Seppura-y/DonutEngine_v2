#include "menu_bar.h"
#include "config_helper.h"
#include "font_icon.h"

#include <QHBoxLayout>

static QAction* newAction(const QString& txt, QMenu* parent, QAction*& act, bool autoRepeat, const QIcon& icon, bool checkable, QAction::MenuRole role = QAction::NoRole)
{
    act = new QAction(txt, parent);
    act->setAutoRepeat(autoRepeat);
    act->setCheckable(checkable);
    parent->addAction(act);
    act->setMenuRole(role);
    act->setIcon(icon);
    return act;
}

MenuBar::MenuBar(QWidget* parent) : QMenuBar(parent)
{
    QHBoxLayout* layout = new QHBoxLayout();
    this->setLayout(layout);

    pb_extend_ = new QPushButton(this);
    layout->addWidget(pb_extend_);
    //pb_extend_->setMaximumSize(50, 50);
    //pb_extend_->setMinimumSize(50, 50);
    pb_extend_->setText("ext");
    connect(pb_extend_, SIGNAL(clicked()), this, SLOT(onExtendButtonClicked()));

    addMenu(menu_file_ = new FileMenu());
    addMenu(menu_opt_ = new OptionMenu());
    addMenu(menu_about_ = new AboutMenu());

    setStyleSheet(ConfigHelper::getQssString(":/resources/res/css/media_manager.css"));

}

MenuBar::~MenuBar()
{
    if (menu_file_)delete menu_file_;
    if (menu_opt_) delete menu_opt_;
    if (menu_about_) delete menu_about_;
}

void MenuBar::onExtendButtonClicked()
{
    emit sigExtendButtonClicked();
}

MenuBar::FileMenu::FileMenu()
{
    //newAction(QString("add"))
}

MenuBar::FileMenu::~FileMenu()
{

}

MenuBar::OptionMenu::OptionMenu()
{

}

MenuBar::OptionMenu::~OptionMenu()
{

}

MenuBar::AboutMenu::AboutMenu()
{

}

MenuBar::AboutMenu::~AboutMenu()
{

}