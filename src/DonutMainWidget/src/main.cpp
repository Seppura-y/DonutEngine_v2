#include <QtWidgets/QApplication>

#include "main_widget.h"
#include "media_player_gui_class.h"
#include "graphics_widget/graphics_widget.h"
#include "graphics_widget/graphics_test.h"

int main(int argc, char* argv[])
{

    QApplication a(argc, argv);

    MediaPlayerGuiClass& gui = MediaPlayerGuiClass::getInstance();
    
    MainWidget* w = MainWidget::getInstance();
    gui.connectSignalsAndSlots();
    w->show();

    GraphicsWiget wid;
    wid.show();
    wid.run();

    //GraphicsTest wid;
    //wid.setTitle("graphcis test");
    //wid.initializeGL();
    //wid.show();
    //wid.run();

    return a.exec();
}