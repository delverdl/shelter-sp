#include "cdbbackup.h"
#include "cdbinit.h"

#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QRegularExpression>
#include <QStringList>

CDbBackup::CDbBackup(QObject *parent)
    : QObject{parent}
{}

void CDbBackup::makeBackup(const QString &sWhere)
{
    QString where;
    bool justCopy = true;

    if (sWhere.isEmpty())
    {   //Just to remove local DB
        where = QDir::currentPath();
        justCopy = false; //No copy to external location
    }
    where += QString("/%1").arg(CDbInit::dbFile());
    if (!justCopy && !ensureBackupFolder(where))
        emit cantProcessBackupFolder();
    else
    {
        QFile::remove(where); //Remove destination before copying or
                              //Origin if not copying
        if (justCopy && !QFile::copy(CDbInit::dbFile(), where))
            emit cantCreateBackup();
    }
}

#ifdef Q_OS_WIN32

#include <windows.h>

bool isUSBDrive(char driveLetter)
{
    char rootPath[4] = { driveLetter, ':', '\\', '\0' };
    UINT driveType = GetDriveTypeA(rootPath);

    if (driveType == DRIVE_REMOVABLE)
    {
        // Check if the removable drive is a USB drive
        HANDLE hDevice = CreateFileA(rootPath, 0, FILE_SHARE_READ | FILE_SHARE_WRITE,
                                     NULL, OPEN_EXISTING, 0, NULL);

        if (hDevice != INVALID_HANDLE_VALUE)
        {
            STORAGE_PROPERTY_QUERY query;
            DWORD bytesReturned;
            char buffer[1024];

            ZeroMemory(&query, sizeof(query));
            query.PropertyId = StorageDeviceProperty;
            query.QueryType = PropertyStandardQuery;

            if (DeviceIoControl(hDevice, IOCTL_STORAGE_QUERY_PROPERTY, &query, sizeof(query),
                                &buffer, sizeof(buffer), &bytesReturned, NULL))
            {
                STORAGE_DEVICE_DESCRIPTOR* descriptor = (STORAGE_DEVICE_DESCRIPTOR*)buffer;

                if (descriptor->BusType == BusTypeUsb)
                {
                    CloseHandle(hDevice);
                    return true;
                }
            }
            CloseHandle(hDevice);
        }
        else
        {
            DWORD nr = GetLastError();
            printf("Error: %d", nr);
        }
    }
    return false;
}

#endif

CDbBackup::HashedPairs CDbBackup::externalDrives() const
{
    HashedPairs r;

#ifdef Q_OS_WIN32
    DWORD nRet;

    nRet = GetLogicalDrives();

    if (nRet != 0)
    {
        int bitSel = 0;

        while (bitSel < 28)
        {
            char drv = 'A' + bitSel;
            DWORD nBs;

            nBs = nRet >> bitSel;
            bitSel++;
            if ((nBs & 1) == 1 && isUSBDrive(drv))
                r.insert(QString(drv), QString(drv));
        }
    }
#else
    HashedPairs mounts;
    QStringList ltmp = fileContents("/etc/mtab"), lmounts;
    QStringListIterator lit(ltmp);

    while (lit.hasNext())
    {
        QString s = lit.next();

        if (s.startsWith("/dev/"))
        {
            lmounts = s.split(QRegularExpression("\\s"));
            if (lmounts.count() > 1)
                            //Device      Mount point
                mounts.insert(lmounts[0], lmounts[1]);
        }
    }

    QDir d, d2;

    d.setPath("/dev/disk/by-id");
    d2.setPath("/dev/disk/by-label");
    if (d.exists() && d2.exists())
    {
        auto eif = d.entryInfoList(QStringList() << "usb*", QDir::Filter::Files);
        auto eif2 = d2.entryInfoList(QStringList() << "*", QDir::Filter::Files);
        QStringList slt;
        HashedPairs labels;

        if (!eif.isEmpty())
        {   //Device links to USB partitions
            for (const QFileInfo& fi : eif)
            {
                if (fi.isSymbolicLink() && !fi.symLinkTarget().isEmpty())
                    slt << fi.symLinkTarget(); //Respective /dev partition
            }
        }
        if (!eif2.isEmpty())
        {   //Device links labels
            for (const QFileInfo& fi : eif2)
            {
                if (fi.isSymbolicLink() && !fi.symLinkTarget().isEmpty())
                    labels.insert(fi.symLinkTarget(), fi.baseName());
            }
        }

        QHashIterator<QString, QString> iter(mounts);

        while (iter.hasNext())
        {
            auto kvp = iter.next();

            if (slt.contains(kvp.key())) //If usb devs list contains current mounted device
            {
                QString s1 = labels.value(kvp.key(), "[NO-LABEL]");

                r.insert(s1, kvp.value()); //Label -> mount point
            }
        }
    }
#endif
    return r;
}

QStringList CDbBackup::fileContents(const QString &fname)
{
    QFile f(fname);
    QStringList lines;

    if (f.open(QIODevice::ReadOnly))
    {
        QByteArray rl;

        do
        {
            rl = f.readLine(65535);
            if (rl.length() > 0)
                lines << rl.trimmed();
        }
        while (rl.length() > 0);
    }
    return lines;
}

bool CDbBackup::ensureBackupFolder(const QString &origf)
{
    QString bkf = QString("%1/backups").arg(QDir::currentPath());
    QDir d(bkf);
    QFileInfo fio(origf);
    bool r = false;

    if (!d.exists() && !d.mkdir(bkf))
        return false; //cannot create backups directory

    QString dfn = QString("%1/%2.%3").arg(bkf, fio.fileName(),
                    QDateTime::currentDateTime().toString("yyMMddHHmmss"));

    //Move database to backup folder
    if (!QFile::rename(origf, dfn))
        return r;

    //Archive files only when needed
    auto el = d.entryInfoList(QDir::Files, QDir::Name);
    QString sfx;
    QStringList lbkFiles;

    dfn = bkf + "/archive.7z";
    foreach (const QFileInfo &fi, el)
    {
        if (fi.fileName().toLower() == "archive.7z")
            continue;
        sfx = fi.suffix();
        m_rx.setPattern("\\d{12}");
        if (sfx.length() == 12 && sfx.indexOf(m_rx) == 0)
        {
            if (fi.size() < 2147483648) //2 GB top per file to archive
                lbkFiles << fi.fileName();
        }
    }
    if (lbkFiles.count() > 4 /*29*/)
    {   //Archive backups
        QProcess p;

        dfn = QDir::currentPath();
        p.setProgram(QString("%1/%2").arg(dfn, "comp.exe"));
        p.setArguments(QStringList() << "a" << "-sdel" << "archive.7z" << lbkFiles);
        QDir::setCurrent(bkf); //Set current dir to backups

        p.start();
        if (p.waitForStarted())
        {
            p.waitForFinished(); //Wait compression end (THIS BLOCKS MAIN THREAD)
            r = true;
        }
        else
            r = false;
        QDir::setCurrent(dfn); //Restore current dir
    }
    return r;
}
