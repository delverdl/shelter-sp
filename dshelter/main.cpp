#include "csheltermain.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CShelterMain w;

    w.show();
    return a.exec();
}
