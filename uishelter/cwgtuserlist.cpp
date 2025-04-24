#include "cnewuser.h"
#include "cwgtuserlist.h"
#include "ui_cwgtuserlist.h"

#include <cdbinit.h>

#include <QBuffer>
#include <QDate>
#include <QKeyEvent>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

CWgtUserList::CWgtUserList(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CWgtUserList)
{
    ui->setupUi(this);
    connect(ui->pbAdd, &QPushButton::clicked, this, &CWgtUserList::addOne);
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

const char *cszListQuery{
    "SELECT a.name,a.birth,a.country,b.whenreg,a.phone,a.nacid,a.picture"
    " FROM ausers a JOIN buregister b ON a.id=b.userid"
    " WHERE b.whenreg=(SELECT max(b.whenreg) FROM b WHERE a.id=b.userid)"
    " AND (b.whenrem='' OR b.whenrem IS NULL)"
};

void CWgtUserList::populateDb(bool addVisible)
{
    ui->pbAdd->setVisible(addVisible);
    ui->ckbListAll->setVisible(addVisible);

    QSqlDatabase db = QSqlDatabase::database(CDbInit::dbConnection());
    QTableView *view = ui->tvwData;

    model = new QSqlQueryModel(view);
    model->setHeaderData(0, Qt::Horizontal, tr("Nombre"));
    model->setHeaderData(1, Qt::Horizontal, tr("Nacido"));
    model->setHeaderData(2, Qt::Horizontal, tr("País"));
    model->setHeaderData(3, Qt::Horizontal, tr("Registro"));
    model->setHeaderData(4, Qt::Horizontal, tr("Teléfono"));
    model->setHeaderData(5, Qt::Horizontal, tr("ID nac."));

    //Show only registered users
    model->setQuery(cszListQuery, db);

    view->setModel(model);
    view->hideColumn(6);
    view->setEditTriggers(QAbstractItemView::NoEditTriggers); //Read-only view

    connect(view, &QTableView::clicked, this, &CWgtUserList::clicked);
    view->installEventFilter(this);
}

QString savePic(const QPixmap &pm)
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
        int userId = nu->existingId();

        if (!nu->existing())
        {
            sData1 = savePic(nu->facePicture());
            sQry1 = QString("INSERT INTO ausers (nacid, name, birth, country, sex, "
                            "picture, phone) VALUES ('%1', '%2', '%3', '%4', '%5', '%6', '%7')")
                        .arg(nu->nationalId(), nu->name(), nu->birth().toString(), nu->country(),
                             nu->sex(), sData1, nu->phone());
            rq = q.exec(sQry1) && q.exec(QString("SELECT %1()").arg(dbi->lastIdQuery())) &&
                 q.next(); //Insert new user
            userId = rq ? q.value(0).toInt() : 0; //Get id of last user added
        }
        if (rq)
        {
            //Registration data on complementary table
            sData1 = savePic(nu->fullPicture());
            sData2 = savePic(nu->idPicture());
            sQry1 = QString("INSERT INTO buregister (userid, whenreg, wholepic, docpic) "
                            "VALUES (%1, '%2', '%3', '%4')").arg(userId)
                        .arg(nu->registrationTime().toString("yyyy-MM-dd HH:mm:ss"),
                             sData1, sData2);

            rq = q.exec(sQry1);
            if (!rq)
                sData2 = q.lastError().text();
        }
        else
            sData2 = q.lastError().text();
        if (!rq)
            emit queryError(sData2);
        else
        {
            ui->lblPicture->clear();
            model->refresh();
        }
    }
    delete nu;
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
            showPic(model->index(r, 6).data().toString());
        }
        return QObject::eventFilter(obj, event);
    }
    else
        // standard event processing
        return QObject::eventFilter(obj, event);
}

void CWgtUserList::showPic(const QString &data)
{
    QByteArray bPix = QByteArray::
        fromBase64(data.toLatin1(), QByteArray::Base64Option::Base64UrlEncoding);

    if (!bPix.isEmpty())
    {
        QPixmap pm;

        if (pm.loadFromData(bPix))
            ui->lblPicture->setPixmap(pm);
    }
}

void CWgtUserList::clicked(const QModelIndex &midx)
{
    int r = midx.row();
    QString sPic = model->index(r, 6).data().toString();

    showPic(sPic);
}

const char *cszListQueryAll{
    "SELECT a.name,a.birth,a.country,b.whenreg,a.phone,a.nacid,a.picture"
    " FROM ausers a JOIN buregister b ON a.id=b.userid"
};

void CWgtUserList::checkStateChanged(Qt::CheckState ck)
{
    QSqlDatabase db = QSqlDatabase::database(CDbInit::dbConnection());

    if (ck == Qt::Checked) model->setQuery(cszListQueryAll, db);
    else model->setQuery(cszListQuery, db);
    model->refresh();
    ui->tvwData->hideColumn(6);
}
