#include <QCoreApplication>
#include <QFile>
#include <QTextStream>

#include "cimgtotext.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    CImgToText i2t;
    QTextStream ts(stdout);

    if (argc >= 2)
    {
        if (!QFile::exists(argv[1]))
        {
            ts << "El archivo '" << argv[1] << "' no existe!" << Qt::endl;
            return ENOENT;
        }
        i2t.run(argv[1]);
        return a.exec();
    }
    else
    {
        ts << "No se indica un archivo de entrada!" << Qt::endl;
        return EINVAL;
    }
}
