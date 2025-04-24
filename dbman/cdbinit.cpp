#include "cdbinit.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QTimer>

#include <string>

const char *csDbConn{"dbShelter_"};
const char *csDbFile{"shelter.db"};
const std::initializer_list< std::pair<std::string, std::string> > cslTables
{
    {"ausers",        //Users list
     "id INTEGER PRIMARY KEY AUTOINCREMENT, nacid TEXT, name TEXT, birth TEXT, country TEXT,"
     " sex TEXT, picture BLOB, phone TEXT"},
    {"buregister",    //Users registration event (can be more than once)
     "id INTEGER PRIMARY KEY AUTOINCREMENT, userid INTEGER, whenreg TEXT, whenrem TEXT,"
     " wholepic BLOB, docpic BLOB, reasonout TEXT, reasonin text, evicted INTEGER,"
     " FOREIGN KEY (userid) REFERENCES ausers(id)"},
    {"cusrdayio",     //Daily in/out events
     "id INTEGER PRIMARY KEY AUTOINCREMENT, userid INTEGER, whenin TEXT, whenout TEXT"
     " DEFAULT CURRENT_TIMESTAMP, numticket INTEGER, countdepend INTEGER, FOREIGN"
     " KEY (userid) REFERENCES ausers(id)"},
    {"dworkers",      //Working persons (users can be working but not listed here)
     "id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT"},
    {"eworkio",       //Workers in/out events
     "id INTEGER PRIMARY KEY AUTOINCREMENT, workerid INTEGER, whenin TEXT default"
     " CURRENT_TIMESTAMP, whenout TEXT, FOREIGN KEY (workerid) REFERENCES eworkers(id)"},
    {"fguards",       //Users considered guards, with password
     "id INTEGER PRIMARY KEY AUTOINCREMENT, userid INTEGER, whenreg TEXT, passwd TEXT,"
     " enabled INTEGER DEFAULT 1, FOREIGN KEY (userid) REFERENCES ausers(id)"},
    {"gguardio",      //Login/logout events of guards
     "id INTEGER PRIMARY KEY AUTOINCREMENT, guardid INTEGER, whenlog TEXT DEFAULT"
     " CURRENT_TIMESTAMP, logoff INTEGER DEFAULT 1, FOREIGN KEY (guardid) REFERENCES"
     " fguards(id)"},
    {"hopersons",     //Personas registradas de organizaciones
     "id INTEGER PRIMARY KEY AUTOINCREMENT, orgname TEXT, name TEXT"},
    {"iorgsio",       //Entrada de personas de organizaciones
     "id INTEGER PRIMARY KEY AUTOINCREMENT, orgid INTEGER, whenin TEXT default"
     " CURRENT_TIMESTAMP, whenout TEXT, FOREIGN KEY (orgid) REFERENCES hopersons(id)"}
};

CDbInit::CDbInit(QObject *parent)
    : QObject{parent}, m_internal(false), m_postgres(false)
{}

const char *CDbInit::dbConnection() { return csDbConn; }
const char *CDbInit::dbFile() { return csDbFile; }
const QList<std::string> CDbInit::tables() { return QMap(cslTables).keys(); }

const char *cslLastId[2]{
    "last_insert_rowid",  //QSQLITE
    "lastval"             //PostgreSQL
};

const char *CDbInit::lastIdQuery()
{
    return m_postgres ? cslLastId[1] : cslLastId[0];
}

void CDbInit::run() { QTimer::singleShot(150, this, SLOT(internalRun())); }

bool CDbInit::openOnly()
{
    auto db = QSqlDatabase::addDatabase("QSQLITE", csDbConn);

    //Open DB
    db.setDatabaseName(csDbFile);
    if (!db.open())
    {
        emit messageOut("Can't open DB -> " + db.lastError().text(), true);
        emit done();
        return false;
    }
    emit messageOut("Database opened...", false);
    if (!m_internal) emit done();
    return true;
}

void CDbInit::internalRun()
{
    m_internal = true;

    bool r = openOnly();

    m_internal = false;
    if (!r) return;

    auto db = QSqlDatabase::database(csDbConn);

    //Create tables from map
    QMap<std::string, std::string> mpTbl(cslTables);
    QList<std::string> ltables = mpTbl.keys();
    QSqlQuery qy(db);

    foreach (std::string tn , ltables)
    {
        auto sq = QString("CREATE TABLE IF NOT EXISTS %1 (%2)").arg(tn, mpTbl[tn]);

        if (!qy.exec(sq))
        {
            emit messageOut(QString("Cannot create table (%1) -> %2")
                                .arg(tn, qy.lastError().text()), true);
            emit done();
            return;
        }
    }

    emit messageOut("Tables created OK", false);
    emit done();
}

QString encryptPassword(const QString &password, const QString &key) {
    QByteArray passwordBytes = password.toUtf8();
    QByteArray keyBytes = key.toUtf8();
    QByteArray encryptedBytes;

    for (int i = 0; i < passwordBytes.size(); ++i) {
        encryptedBytes.append(passwordBytes[i] ^ keyBytes[i % keyBytes.size()]);
    }

    return QString(encryptedBytes.toHex());
}

QString decryptPassword(const QString &encryptedPassword, const QString &key) {
    QByteArray encryptedBytes = QByteArray::fromHex(encryptedPassword.toUtf8());
    QByteArray keyBytes = key.toUtf8();
    QByteArray decryptedBytes;

    for (int i = 0; i < encryptedBytes.size(); ++i) {
        decryptedBytes.append(encryptedBytes[i] ^ keyBytes[i % keyBytes.size()]);
    }

    return QString(decryptedBytes);
}
