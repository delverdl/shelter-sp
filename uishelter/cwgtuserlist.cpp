#include "cnewuser.h"
#include "cwgtuserlist.h"
#include "ui_cwgtuserlist.h"

#include <cdbinit.h>

#include <QBuffer>
#include <QDate>
#include <QKeyEvent>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QtDebug>

CWgtUserList::CWgtUserList(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CWgtUserList)
    , guard(0) //Null guard
{
    ui->setupUi(this);
    connect(ui->pbAdd, &QPushButton::clicked, this, &CWgtUserList::addOne);
    connect(ui->pbEdit, &QPushButton::clicked, this, &CWgtUserList::editOne);
    connect(ui->ckbListAll, &QCheckBox::checkStateChanged, this, &CWgtUserList::checkStateChanged);
}

CWgtUserList::~CWgtUserList()
{
    delete ui;
}

void CWgtUserList::setDbInit(CDbInit *dbi_)
{
    dbi = dbi_;
}

const char *cszListQueryTail
{
    " FROM ausers a JOIN buregister b ON a.id=b.userid"
    " WHERE b.whenreg=(SELECT max(b.whenreg) FROM buregister b WHERE a.id=b.userid)"
    " AND (b.whenrem='' OR b.whenrem IS NULL)"
};

const char *cszListQuery{
    "SELECT a.name,a.birth,a.country,b.whenreg,b.whenrem,a.nacid,a.phone,a.picture,a.id,b.id"
};

void CWgtUserList::populateDb(bool addVisible)
{
    qInfo() << "Listando usuarios...";
    ui->pbAdd->setVisible(addVisible);
    ui->pbEdit->setVisible(addVisible);
    ui->ckbListAll->setVisible(addVisible);

    QSqlDatabase db = QSqlDatabase::database(CDbInit::dbConnection());
    QTableView *view = ui->tvwData;

    model = new QSqlQueryModel(view);

    //Show only registered users
    model->setQuery(QString(cszListQuery) + QString(cszListQueryTail), db);
    setHeaders();
    view->setModel(model);
    view->hideColumn(7);
    view->hideColumn(8);
    view->hideColumn(9);
    view->setEditTriggers(QAbstractItemView::NoEditTriggers); //Read-only view

    connect(view, &QTableView::clicked, this, &CWgtUserList::clicked);
    view->installEventFilter(this);
    qDebug() << "Listado finalizado";
}

QString CWgtUserList::savePic(const QPixmap &pm)
{
    QByteArray ba;
    QBuffer buff(&ba);
    QString sData1;

    buff.open(QIODevice::WriteOnly);
    if (!pm.isNull())
    {
        pm.save(&buff, "PNG");
        sData1 = ba.toBase64(QByteArray::Base64UrlEncoding);
    }
    return sData1;
}

void CWgtUserList::addOne()
{
    CNewUser *nu = new CNewUser(this);

    if (nu->exec() == QDialog::Accepted)
    {
        QString sData1, sData2, sQry1;
        QSqlDatabase db = QSqlDatabase::database(CDbInit::dbConnection());
        QSqlQuery q(db);
        bool rq = true;
        int userId = nu->existingUserId();

        if (!nu->existing())
        {   //Modify data of existing user
            sData1 = savePic(nu->facePicture());
            sQry1 = QString("INSERT INTO ausers (nacid, name, birth, country, sex, "
                            "picture, phone) VALUES ('%1', '%2', '%3', '%4', '%5', '%6', '%7')")
                        .arg(nu->nationalId(), nu->name(),
                             nu->birth().toString("yyyy-MM-dd"), nu->country(),
                             nu->sex(), sData1, nu->phone());
            rq = q.exec(sQry1) && q.exec(QString("SELECT %1()").arg(dbi->lastIdQuery())) &&
                 q.next(); //Insert new user
            userId = rq ? q.value(0).toInt() : 0; //Get id of last user added
        }
        if (rq)
        {   //When adding a new user also add other properties to "buregister" table
            //Registration data on complementary table
            sData1 = savePic(nu->fullPicture());
            sData2 = savePic(nu->idPicture());
            sQry1 = QString("INSERT INTO buregister (userid, whenreg, wholepic, docpic,"
                            " guardid) VALUES (%1, '%2', '%3', '%4', %5)")
                        .arg(userId)
                        .arg(nu->registrationTime().toString("yyyy-MM-dd HH:mm:ss"),
                             sData1, sData2, guard == 0 ? QString("NULL") :
                                 QString("%1").arg(guard));

            rq = q.exec(sQry1);
            if (!rq)
                sData2 = q.lastError().text();
        }
        else
            sData2 = q.lastError().text();
        if (!rq)
        {
            sData1 = QString("No se puede modificar la información del usuario (%1): %2")
                         .arg(userId).arg(sData2);
            qCritical() << sData1;
            QMessageBox::critical(this, "Error", sData1);
        }
        else
        {
            ui->lblPicture->clear();
            refreshModel();
        }
    }
    delete nu;
}

void CWgtUserList::editOne()
{
    QModelIndex currIdx = ui->tvwData->currentIndex();

    if (currIdx.isValid())
    {
        int r = currIdx.row();
        int idNum = model->index(r, 8).data().toInt();
        int regNum = model->index(r, 9).data().toInt();
        CNewUser *nu = new CNewUser(this);

        nu->toEditUser(idNum, regNum);
        if (nu->exec() == QDialog::Accepted && //Dialog accepted
            !nu->existing()) //Any modification found
        {
            QSqlDatabase db = QSqlDatabase::database(CDbInit::dbConnection());
            QSqlQuery q(db);
            bool rq = true;
            QString sData1, sData2, sQry1;

            sData1 = savePic(nu->facePicture());
            sQry1 = QString("UPDATE ausers SET nacid='%1',name='%2',birth='%3',country='%4',"
                            "sex='%5',phone='%6',picture='%7' WHERE id=%8")
                        .arg(nu->nationalId(), nu->name(),
                             nu->birth().toString("yyyy-MM-dd"), nu->country(),
                             nu->sex(), nu->phone(), sData1).arg(idNum);
            rq = q.exec(sQry1);
            if (rq)
            {
                sData1 = savePic(nu->fullPicture());
                sData2 = savePic(nu->idPicture());
                sQry1 = QString("UPDATE buregister SET wholepic='%1',docpic='%2',"
                                "guardid=%3 WHERE id=%4")
                            .arg(sData1, sData2, guard == 0 ? QString("NULL") :
                                                     QString("%1").arg(guard))
                            .arg(regNum);
                rq = q.exec(sQry1);
                if (!rq) sData2 = q.lastError().text();
                else if (nu->outDataModified())
                {
                    sQry1 = QString("UPDATE buregister SET reasonout='%1',evicted=%2")
                               .arg(nu->reasonOut()).arg(nu->evicted() ? 1 : 0);
                    if (nu->exitTime().isValid())
                        sQry1 += QString(",whenrem='%1'").arg(nu->exitTime().toString(
                            "yyyy-MM-dd HH:mm:ss"));
                    rq = q.exec(sQry1);
                    if (!rq) sData2 = q.lastError().text();
                }
            }
            else
                sData2 = q.lastError().text();
            if (rq)
            {
                ui->lblPicture->clear();
                refreshModel();
            }
            else
            {
                sData1 = QString("No se puede modificar la información del usuario (%1): %2")
                             .arg(idNum).arg(sData2);
                qCritical() << sData1;
                QMessageBox::critical(this, "Error", sData1);
            }
        }
        delete nu;
    }
}

void CWgtUserList::setCurrentGuard(int guardId)
{
    guard = guardId;
}

bool CWgtUserList::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj)

    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = reinterpret_cast<QKeyEvent *>(event);

        if (keyEvent->key() == Qt::Key_Up || keyEvent->key() == Qt::Key_Down)
        {
            QModelIndex prevIdx = ui->tvwData->currentIndex();
            int cnt = ui->tvwData->model()->rowCount(), r = prevIdx.row();

            if (keyEvent->key() == Qt::Key_Down && (r != cnt - 1))
                r++;
            else if (r != 0)
                r--;
            showPic(model->index(r, 7).data().toString(), ui->lblPicture);
        }
        return QObject::eventFilter(obj, event);
    }
    else
        // standard event processing
        return QObject::eventFilter(obj, event);
}

void CWgtUserList::refreshModel()
{
    model->refresh();
    ui->tvwData->hideColumn(7);
    ui->tvwData->hideColumn(8);
    ui->tvwData->hideColumn(9);
}

void CWgtUserList::setHeaders()
{
    model->setHeaderData(0, Qt::Horizontal, tr("Nombre"));
    model->setHeaderData(1, Qt::Horizontal, tr("Nacido"));
    model->setHeaderData(2, Qt::Horizontal, tr("País"));
    model->setHeaderData(3, Qt::Horizontal, tr("Registro"));
    model->setHeaderData(4, Qt::Horizontal, tr("Salida"));
    model->setHeaderData(5, Qt::Horizontal, tr("ID nac."));
    model->setHeaderData(6, Qt::Horizontal, tr("Teléfono"));
}

void CWgtUserList::showPic(const QString &data, QLabel *lbl)
{
    QByteArray bPix = QByteArray::
        fromBase64(data.toLatin1(), QByteArray::Base64Option::Base64UrlEncoding);

    lbl->clear();
    if (!bPix.isEmpty())
    {
        QPixmap pm;

        if (pm.loadFromData(bPix))
            lbl->setPixmap(pm);
    }
}

const char *CWgtUserList::listQueryTail()
{
    return cszListQueryTail;
}

void CWgtUserList::clicked(const QModelIndex &midx)
{
    int r = midx.row();
    QString sPic = model->index(r, 7).data().toString();

    showPic(sPic, ui->lblPicture);
}

const char *cszListQueryAll{
    "SELECT a.name,a.birth,a.country,b.whenreg,b.whenrem,a.nacid,a.phone,a.picture,a.id,b.id"
    " FROM ausers a JOIN buregister b ON a.id=b.userid"
};

void CWgtUserList::checkStateChanged(Qt::CheckState ck)
{
    QSqlDatabase db = QSqlDatabase::database(CDbInit::dbConnection());

    if (ck == Qt::Checked) model->setQuery(cszListQueryAll, db);
    else model->setQuery(QString(cszListQuery) + QString(cszListQueryTail), db);

    setHeaders();
    refreshModel();
}
