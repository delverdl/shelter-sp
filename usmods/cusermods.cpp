#include "cusermods.h"
#include "./ui_cusermods.h"

#include <CNewUser.h>
#include <QInputDialog>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <cpasswordinput.h>
#include <cutility.h>

CUserMods::CUserMods(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CUserMods)
{
    dbi = new CDbInit(this);

    dbi->openOnly();
    ui->setupUi(this);

    wusers = new CWgtUserList(ui->tw);

    QVBoxLayout *loUsers = new QVBoxLayout;

    ui->tabUsers->setLayout(loUsers);
    loUsers->addWidget(wusers);

    wusers->setDbInit(dbi);
    wusers->populateDb(true);
    wusers->setCurrentGuard(); //Default guard to null cuz it's not main app

    connect(ui->pbAdd, &QPushButton::clicked, this, &CUserMods::addClicked);
    connect(ui->pbRemove, &QPushButton::clicked, this, &CUserMods::removeClicked);

    initList();
}

CUserMods::~CUserMods()
{
    delete ui;
    delete dbi;
}

void CUserMods::initList()
{
    QSqlDatabase db = QSqlDatabase::database(CDbInit::dbConnection());

    model = new QSqlQueryModel(ui->tvwGuards);
    //Show only enabled guards users
    model->setQuery("SELECT u.name,u.country,u.birth,r.whenreg,u.id,r.id FROM fguards r"
                    " JOIN ausers u ON u.id=r.userid WHERE r.enabled=1", db);
    model->setHeaderData(0, Qt::Horizontal, tr("Nombre"));
    model->setHeaderData(1, Qt::Horizontal, tr("País"));
    model->setHeaderData(2, Qt::Horizontal, tr("Nacimiento"));
    model->setHeaderData(3, Qt::Horizontal, tr("Registro"));
    ui->tvwGuards->setModel(model);
    ui->tvwGuards->hideColumn(4);
    ui->tvwGuards->hideColumn(5);
}

void CUserMods::addClicked()
{
    QString sMsg;
    CNewUser *nu = new CNewUser(this);

    nu->hideExtended(); //Pictures ain't needed here
    if (nu->exec() == QDialog::Accepted)
    {
        //User must exits on DB, otherwise it ain't possible adding a guard
        if (!nu->existing())
        {
            QMessageBox::warning(this, "Advertencia",
                                 "El usuario no existe");
            delete nu;
            return;
        }
        //Verify a valid user ID: if invalid this is a software error.
        if (nu->existingUserId() < 1)
        {
            sMsg = QString("El usuario no se puede econtrar: %1").arg(nu->existingUserId());
            qCritical() << sMsg.toUtf8().data();
            QMessageBox::critical(this, "Error", sMsg);
            delete nu;
            return;
        }

        QSqlDatabase db = QSqlDatabase::database(CDbInit::dbConnection());
        QSqlQuery q(db);
        bool rq;

        rq = q.exec(QString("SELECT enabled FROM fguards WHERE userid=%1")
                        .arg(nu->existingUserId()));
        if (rq)
        {   //Check if there's an existing guard
            if (q.next())
            {   //Guard exists
                if (q.value(0).toInt() == 1) //Is enabled?
                {
                    rq = false;
                    sMsg = QString("El guardia existe y está activado: %1 (%2)")
                               .arg(nu->existingUserId()).arg(nu->name());
                    qWarning() << sMsg.toUtf8().data();
                    QMessageBox::warning(this, "Advertencia", sMsg);
                }
                else
                {   //Disabled
                    int r = QMessageBox::warning(
                        this, "Advertencia",
                        QString("Desea rehabilitar al usuario %1 (# %2)")
                            .arg(nu->name()).arg(nu->existingUserId()),
                        QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No);

                    if (r == QMessageBox::StandardButton::Yes)
                    {   //Enable user and set password to be updated on next start
                        rq = q.exec(QString("UPDATE fguards SET password=NULL,enabled=1"
                                            " WHERE userid=%1").arg(nu->existingUserId()));
                        if (!rq)
                            sMsg = QString("No se puede reactivar al usuario: %1")
                                       .arg(q.lastError().text());
                        else
                            qInfo() << QString("Se reactivó correctamente el guardia:"
                                               " %1 (%2)").arg(nu->existingUserId())
                                           .arg(nu->name());
                    }
                }
            }
            else
            {
                sMsg = CUtility::suggestName(nu->name());

                bool ok = false;
                QString sUser;

                do
                {
                    //Ask for user name
                    sUser = QInputDialog::getText(this, "Entre...", "Nombe de usuario",
                                              QLineEdit::Normal, sMsg, &ok);

                    if (!ok)
                    {
                        delete nu;
                        qDebug() <<"User canceled guard creation";
                        return;
                    }
                } while (!CUtility::validName(sUser));

                //Insert guard
                rq = q.exec(QString("INSERT INTO fguards (userid,username,password)"
                                    " VALUES(%1,'%2',NULL)").arg(nu->existingUserId()));
                if (!rq)
                    sMsg = QString("No se puede añadir al usuario '%1' (%2) como"
                                   " guardia: %3")
                               .arg(nu->name())
                               .arg(nu->existingUserId())
                               .arg(q.lastError().text());
                else
                    qInfo() << QString("Se adicionó correctamente el guardia: %1 (%2)")
                                   .arg(nu->existingUserId()).arg(nu->existingUserId());
            }
        }
        else
            sMsg = QString("No se puede verificar el guardia activo: %1")
                       .arg(q.lastError().text());
        if (!rq)
            qCritical() << sMsg.toUtf8().data();
        else
            refreshModel();
    }
    delete nu;
}

void CUserMods::removeClicked()
{
    QModelIndex midx = ui->tvwGuards->currentIndex();
    bool ok = false;

    if (midx.isValid())
    {
        int r = midx.row(), idn = model->index(r, 5).data().toInt(&ok);
        QString sMsg;

        if (ok && idn != 0)
        {
            QSqlDatabase db = QSqlDatabase::database(CDbInit::dbConnection());
            QSqlQuery q(db);

            if (!q.exec(QString("UPDATE fguards SET enabled=0 WHERE id=%1").arg(idn)))
                sMsg = QString("Error al desactivar el guardia: %1").arg(q.lastError().text());
            else
            {
                sMsg = QString("Desactivación exitosa del guardia seleccionado: %1 (%2)")
                           .arg(idn).arg(model->index(r, 0).data().toString());
                QMessageBox::information(this, "Información", sMsg);
            }
            refreshModel();
        }
        else
            sMsg = QString("El índice de lista de los guardias no es válido: %1").arg(idn);
        qCritical() << sMsg.toUtf8().data();
    }
}

void CUserMods::resetClicked()
{
    QModelIndex midx = ui->tvwGuards->currentIndex();
    bool ok = false;

    if (midx.isValid())
    {
        int r = midx.row(), idn = model->index(r, 5).data().toInt(&ok);
        QString sMsg;

        if (ok && idn != 0)
        {
            QSqlDatabase db = QSqlDatabase::database(CDbInit::dbConnection());
            QSqlQuery q(db);

            if (!q.exec(QString("UPDATE fguards SET password=NULL WHERE id=%1").arg(idn)))
                sMsg = QString("Error al restablecer la contraseña del guardia: %1")
                           .arg(q.lastError().text());
            else
            {
                sMsg = QString("Reinicio de contraseña exitoso para el guardia"
                               " seleccionado: %1 (%2)").arg(idn).arg(model->index(r,
                                             0).data().toString());
                QMessageBox::information(this, "Información", sMsg);
            }
        }
        else
            sMsg = QString("El índice de lista de los guardias no es válido: %1").arg(idn);
        qCritical() << sMsg.toUtf8().data();
    }
}

void CUserMods::refreshModel()
{
    model->refresh();
    ui->tvwGuards->hideColumn(4);
    ui->tvwGuards->hideColumn(5);
}
