#include "cutility.h"

CUtility::CUtility() {}

QString CUtility::replaceTildeLetters(const QString &input) {
    std::initializer_list< std::pair<std::string, std::string> > replacements
    {
        {"á", "a"}, {"é", "e"}, {"í", "i"}, {"ó", "o"}, {"ú", "u"}, {"Á", "A"}, {"É", "E"},
        {"Í", "I"}, {"Ó", "O"}, {"Ú", "U"}, {"ñ", "n"}, {"Ñ", "N"}, {"ä", "a"}, {"Ä", "A"},
        {"ë", "e"}, {"Ë", "E"}, {"ï", "i"}, {"Ï", "I"}, {"ö", "o"}, {"ü", "u"}, {"Ü", "U"},
        {"ç", "z"}, {"Ç", "Z"}, {"ã", "a"}, {"õ", "o"}, {"Ã", "A"}, {"Õ", "O"}
    };

    QString result = input;
    for (auto it = replacements.begin(); it != replacements.end(); ++it) {
        result.replace(QString::fromStdString(it->first),
                       QString::fromStdString(it->second));
    }

    return result;
}

QString CUtility::removeNonAscii(const QString &input)
{
    QString result = input;

    for (int i = 0; i < result.length(); ++i)
    {
        uchar c = result[i].cell();

        if (c == 0x2e || isalnum(c)) continue;
        result.removeAt(i);
        --i;
    }
    return result;
}

QString CUtility::encryptPassword(const QString &password, const QString &key)
{
    QByteArray passwordBytes = password.toUtf8();
    QByteArray keyBytes = key.toUtf8();
    QByteArray encryptedBytes;

    for (int i = 0; i < passwordBytes.size(); ++i)
        encryptedBytes.append(passwordBytes[i] ^ keyBytes[i % keyBytes.size()]);
    return QString(encryptedBytes.toHex());
}

QString CUtility::decryptPassword(const QString &encrypted, const QString &key)
{
    QByteArray encryptedBytes = QByteArray::fromHex(encrypted.toUtf8());
    QByteArray keyBytes = key.toUtf8();
    QByteArray decryptedBytes;

    for (int i = 0; i < encryptedBytes.size(); ++i)
        decryptedBytes.append(encryptedBytes[i] ^ keyBytes[i % keyBytes.size()]);
    return QString(decryptedBytes);
}

QString CUtility::suggestName(const QString &nu)
{
    QString sMsg = CUtility::replaceTildeLetters(nu.toLower()), sr;
    QStringList sl, rep;
    bool foundMix, atEnd;

    foundMix = atEnd = false;
    sMsg = CUtility::removeNonAscii(sMsg);
    sl = sMsg.split(' ', Qt::SkipEmptyParts);
    for (int i = 0; i < sl.count(); ++i)
    {
        while (sl[i] == "de" || sl[i] == "la" || sl[i] == "von" || sl[i] == "mit" ||
               sl[i] == "van" || sl[i] == "der")
        {
            foundMix = true;
            sr += sl[i];
            i++;
            if (i >= sl.count())
            {
                atEnd = true;
                break;
            }
        }
        if (atEnd)
            rep << sr;
        else if (foundMix)
        {
            foundMix = false;
            rep << sr + sl[i];
            sr.clear();
        }
        else
            rep << sl[i];
    }
    if (rep.count() == 1) //No surnames
        //Name only
        return rep[0];
    else if (rep.count() == 2) //No middle name
        //Name             //Father family name
        return QString(sl[0][0]) + sl[1];
    else if (rep.count() > 2) //The second element is the middle name
        //Name             //Father family name
        return QString(sl[0][0]) + sl[2];
    else
        return "unknown_name";
}

bool CUtility::validName(const QString &n)
{
    foreach (const QChar &ch, n)
        if (!isalnum(ch.cell()) && ch != '_' && ch != '.')
            return false;
    return true;
}
