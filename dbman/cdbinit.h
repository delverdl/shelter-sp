#ifndef CDBINIT_H
#define CDBINIT_H

#include <QObject>
#include <QMap>

#include "dbman_global.h"

class DBMAN_EXPORT CDbInit : public QObject
{
    Q_OBJECT
public:
    explicit CDbInit(QObject *parent = nullptr);

    static const char *dbConnection();
    static const char *dbFile();
    static QList<std::string> const tables();
    const char *lastIdQuery();

signals:
    void done();
    void messageOut(const QString& msg, bool isError);

public slots:
    void run();
    bool openOnly();

private slots:
    void internalRun();

private:
    bool m_internal;
    bool m_postgres;
};

#endif // CDBINIT_H
