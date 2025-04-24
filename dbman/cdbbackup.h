#ifndef CDBBACKUP_H
#define CDBBACKUP_H

#include <QHash>
#include <QRegularExpression>
#include <QObject>

#include "dbman_global.h"

class DBMAN_EXPORT CDbBackup : public QObject
{
    Q_OBJECT
public:

    typedef QHash<QString, QString> HashedPairs;

    explicit CDbBackup(QObject *parent = nullptr);

    void makeBackup(const QString &sWhere = QString());

    HashedPairs externalDrives() const;

signals:
    void notImplemented();
    void cantProcessBackupFolder();
    void cantCreateBackup();

private:
    QRegularExpression  m_rx;

    QStringList fileContents(const QString &fname) const;
    bool ensureBackupFolder(const QString &origf);
};

#endif // CDBBACKUP_H
