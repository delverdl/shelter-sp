#include "cusermods.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CUserMods w;

    w.show();
    return a.exec();
}
