#include <QtWidgets/QApplication>

#include "main_widget.h"
#include "media_player_gui_class.h"

int main(int argc, char* argv[])
{

    QApplication a(argc, argv);

    MediaPlayerGuiClass& gui = MediaPlayerGuiClass::getInstance();
    
    MainWidget* w = MainWidget::getInstance();
    gui.connectSignalsAndSlots();
    w->show();

    return a.exec();
}