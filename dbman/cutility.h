#ifndef CUTILITY_H
#define CUTILITY_H

#include "dbman_global.h"

#include <QMap>

class DBMAN_EXPORT CUtility
{
public:
    CUtility();

    static QString replaceTildeLetters(const QString &input);
    static QString removeNonAscii(const QString &input);
    static QString encryptPassword(const QString &password, const QString &key);
    static QString decryptPassword(const QString &encrypted, const QString &key);
    static QString suggestName(const QString &nu);
    static bool validName(const QString &n);


};

#endif // CUTILITY_H
