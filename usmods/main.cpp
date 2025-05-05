#include "cusermods.h"

#include <QApplication>
#include <clogenabler.h>

int main(int argc, char *argv[])
{
    QApplication::setApplicationName("usmods");
    CLogEnabler::create();

    QApplication a(argc, argv);
    CUserMods w;

    w.show();
    return a.exec();
}
