#ifndef CLOGENABLER_H
#define CLOGENABLER_H

#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QtMessageHandler>

#include "dbman_global.h"

class DBMAN_EXPORT CLogEnabler
{
public:
    static CLogEnabler *create();

private:
    static CLogEnabler *m_instance;
    static QFile *m_f;
    static QTextStream m_o;

    static void logToFile(QtMsgType type, const QMessageLogContext &context,
                          const QString &msg);

    CLogEnabler();
};

#endif // CLOGENABLER_H
