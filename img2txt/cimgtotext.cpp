#include "cimgtotext.h"

#include <QFile>
#include <QTextStream>
#include <QTimer>

CImgToText::CImgToText(QObject *parent)
    : QObject{parent}
{}

void CImgToText::run(const QString &fileName)
{
    fname = fileName;
    QTimer::singleShot(150, this, SLOT(process()));
}

void CImgToText::process()
{
    QFile f(fname);
    QTextStream tx(stdout);

    if (f.open(QIODevice::ReadOnly))
    {
        QByteArray dimg = f.readAll(), ba;

        ba = dimg.toBase64(QByteArray::Base64UrlEncoding);
        f.close();
        f.setFileName(f.fileName() + ".txt");
        if (f.open(QIODevice::WriteOnly | QIODevice::Truncate))
        {
            f.write(ba);
            tx << "Se ha convertido la imagen!";
        }
        else
            tx << "No se puede abrir el archivo para escribirle!";
    }
    else
        tx << QString("Imposible abrir el archivo de entrada!: %1").arg(fname);
    tx << Qt::endl;
}
