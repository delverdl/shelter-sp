#include <cdbinit.h>
#include <cdbbackup.h>

#include <QCoreApplication>
#include <QFile>
#include <QTextStream>

static QTextStream xo(stdout);
static QTextStream xi(stdin);

QString queryUser(const QString& q, const QString& def, const QStringList& posibles)
{
    return QString();
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    CDbInit dbi(&a);
    QString ch;

    if (QFile::exists(CDbInit::dbFile()))
    {
        do
        {
            xo << "Quiere respaldar la base de datos (Y/n): ";
            xo.flush();
            xi.flush();
            ch = xi.readLine();
            ch = ch.toLower();
            xi.flush();
            xo << Qt::endl;
            if (ch.isEmpty()) ch = "y";
        } while (ch.length() > 1 || ch != "y" && ch != "n");

        if (ch == 'y')
        {   //Whant to make backup
            CDbBackup bk;

            bk.makeBackup();
        }
    }

    a.connect(&dbi, &CDbInit::done, &a, &QCoreApplication::quit);
    dbi.run();
    return a.exec();
}
