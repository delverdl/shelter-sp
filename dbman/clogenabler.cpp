#include "clogenabler.h"

CLogEnabler *CLogEnabler::m_instance = nullptr;
QFile *CLogEnabler::m_f = nullptr;
QTextStream CLogEnabler::m_o;

CLogEnabler *CLogEnabler::create()
{
    if (m_instance == nullptr)
    {
        time_t t;

        time(&t);
        srand((unsigned int) t);
        m_f = new QFile;
        m_instance = new CLogEnabler;
    }
    return m_instance;
}

void CLogEnabler::logToFile(QtMsgType type, const QMessageLogContext &context,
                            const QString &msg)
{
    m_o << qFormatLogMessage(type, context, msg) << Qt::endl;
}

CLogEnabler::CLogEnabler()
{
    QString sApp = qApp->applicationName(), sName, sBase = QString("log-%1.txt");

    //Get log filename
    if (sApp.isEmpty()) { int nRand = rand(); sName = sBase.arg(nRand, 8, 16); }
    else sName = sBase.arg(sApp.toLower());

    //Open ile
    m_f->setFileName(sName);
    m_f->open(QIODevice::WriteOnly | QIODevice::Append);
    m_o.setDevice(m_f);

    //Install messages handler
    qSetMessagePattern("[%{time yyyyMMdd h:mm:ss.zzz ttt} %{if-debug}D%{endif}%{if-info}I"
                       "%{endif}%{if-warning}W%{endif}%{if-critical}C%{endif}%{if-fatal}"
                       "F%{endif}] - %{message}");
    qInstallMessageHandler(logToFile);
}
