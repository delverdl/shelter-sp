#include "cnewuser.h"
#include "cuserexitinfo.h"
#include "cwgtuserlist.h"
#include "ui_cnewuser.h"

#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>
#include <cdbinit.h>
#include <QTimer>
#include <QScreen>

const std::initializer_list<QString> csCountries{
    "Afganistán", "Albania", "Alemania", "Andorra", "Angola", "Antigua y Barbuda",
    "Arabia Saudita","Argelia", "Argentina", "Armenia", "Australia", "Austria",
    "Azerbaiyán", "Bahamas", "Bahréin", "Bangladés", "Barbados", "Bielorrusia", "Bélgica",
    "Belice", "Benín", "Bután", "Bolivia", "Bosnia-Herzegovina", "Botsuana", "Brasil",
    "Brunéi", "Bulgaria", "Burkina Faso", "Burundi", "Cabo Verde", "Camboya", "Camerún",
    "Canadá", "Chad", "República Checa", "Chequia", "Chile", "China", "Chipre",
    "Colombia", "Comoras", "Congo", "República Democrática del Congo", "Corea del Norte",
    "Corea del Sur", "Costa Rica", "Costa de Marfil", "Croacia", "Cuba", "Dinamarca",
    "Yibuti", "Dominica", "Ecuador", "Egipto", "El Salvador", "Emiratos Árabes Unidos",
    "Eritrea", "Eslovaquia", "Eslovenia", "España", "Estados Unidos", "Estonia", "Etiopía",
    "Fiyi", "Filipinas", "Finlandia", "Francia", "Gabón", "Gambia", "Georgia", "Ghana",
    "Granada", "Grecia", "Guatemala", "Guinea", "Guinea-Bissau", "Guinea Ecuatorial",
    "Guyana", "Haití", "Honduras", "Hungría", "India", "Indonesia", "Irán", "Iraq",
    "Irlanda", "Islandia", "Islas Marshall", "Israel", "Italia", "Jamaica", "Japón",
    "Jordania", "Kazajistán", "Kenia", "Kirguistán", "Kiribati", "Kuwait", "Laos",
    "Lesoto", "Letonia", "Líbano", "Liberia", "Libia","Liechtenstein", "Lituania",
    "Luxemburgo", "Macedonia", "Madagascar", "Malasia", "Malaui", "Maldivas", "Mali",
    "Malta", "Marruecos", "Mauricio", "Mauritania", "México", "Micronesia", "Moldavia",
    "Mónaco", "Mongolia", "Montenegro", "Mozambique", "Birmania", "Namibia", "Nauru", "Nepal",
    "Nicaragua", "Níger", "Nigeria", "Noruega", "Nueva Zelanda", "Omán", "Países Bajos",
    "Pakistán", "Palaos", "Panamá", "Papúa Nueva Guinea", "Paraguay", "Perú", "Polonia",
    "Portugal", "Qatar", "Reino Unido", "República Centroafricana", "República Dominicana",
    "Rumanía", "Rusia", "Ruanda", "San Cristóbal y Nieves", "Islas Salomón", "Samoa",
    "San Marino", "Santa Lucía", "Ciudad del Vaticano", "Santo Tomé y Príncipe",
    "San Vicente y las Granadinas", "Senegal", "Serbia", "Seychelles", "Sierra Leona",
    "Singapur", "Siria", "Somalia", "Sri Lanka", "Sudáfrica", "Sudán", "Sudán del Sur",
    "Suecia", "Suiza", "Surinam", "Suazilandia", "Tailandia", "Tanzania", "Tayikistán",
    "Timor Oriental", "Togo", "Tonga", "Trinidad y Tobago", "Túnez", "Turkmenistán",
    "Turquía", "Tuvalu", "Ucrania", "Uganda", "Uruguay", "Uzbekistán", "Vanuatu", "Venezuela",
    "Vietnam", "Yemen", "Zambia", "Zimbabue"
};

const char *csFilter = "Imágenes (*.png *.jpg *.xpm *.bmp *.gif *.mng *.jpge);; Todos (*.*)";

CNewUser::CNewUser(QWidget *parent)
    : QDialog(parent), ui(new Ui::CNewUser), imgWidget(nullptr), lblPic(nullptr),
    isExistingUser(false), existingId(0), showList(true), outModified(false)
{
    QStringList dl;

    ui->setupUi(this);
    connect(ui->pbAccept, &QPushButton::clicked, this, &CNewUser::acceptedClick);
    connect(ui->pbCancel, &QPushButton::clicked, this, &CNewUser::reject);
    connect(ui->pbRemove, &QPushButton::clicked, this, &CNewUser::userExitData);
    //Add countries to combobox
    ui->cbxCountry->addItem("");
    ui->cbxCountry->addItems(csCountries);
    //Set directory for searching images
    dl = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
    if (!dl.isEmpty()) lastFolder = dl.first();
    //Set current date-time
    ui->dteRegister->setDateTime(QDateTime::currentDateTime());
    connect(ui->leName, &QLineEdit::textChanged, this, &CNewUser::userNameChanged);

    //Create popup detached off this window
    uc = new CUserCheck(this);
    connect(uc, &CUserCheck::selectItem, this, &CNewUser::selectItem);
    connect(uc, &CUserCheck::showItem, this, &CNewUser::showItem);

    //Create menu for loading images
    prepareMenu();

    ui->lblIdPicture->installEventFilter(this);
    ui->lblFullPicture->installEventFilter(this);
    ui->lblFacePicture->installEventFilter(this);
    ui->pbRemove->setVisible(false);
    connect(ui->tbtPicture, &QToolButton::clicked, ui->tbtPicture, &QToolButton::showMenu);
    ui->leId->installEventFilter(this);
}

CNewUser::~CNewUser() { delete ui; }

QString CNewUser::name() const { return ui->leName->text(); }

QString CNewUser::nationalId() const { return ui->leId->text(); }

QDate CNewUser::birth() const { return ui->deBirth->date(); }

QString CNewUser::country() const { return ui->cbxCountry->currentText(); }

QString CNewUser::sex() const { return ui->cbxSex->currentText(); }

QString CNewUser::phone() const { return ui->lePhone->text(); }

QPixmap CNewUser::facePicture() const { return ui->lblFacePicture->pixmap(); }

QPixmap CNewUser::fullPicture() const { return ui->lblFullPicture->pixmap(); }

QPixmap CNewUser::idPicture() const { return ui->lblIdPicture->pixmap(); }

QDateTime CNewUser::registrationTime() const { return ui->dteRegister->dateTime(); }

QString CNewUser::reasonOut() const { return uReasonOut; }

bool CNewUser::evicted() const { return uEvicted; }

QDateTime CNewUser::exitTime() const { return uExitTime; }

bool CNewUser::existing() const { return isExistingUser; }

bool CNewUser::outDataModified() const { return outModified; }

int CNewUser::existingUserId() const { return existingId; }

void CNewUser::hideExtended()
{
    //Hide controls when adding a new guard, so user's specific data is not used
    ui->lblRegTime->setVisible(false);
    ui->dteRegister->setVisible(false);
    ui->lblShowId->setVisible(false);
    ui->lblShowFull->setVisible(false);
    ui->lblIdPicture->setVisible(false);
    ui->lblFullPicture->setVisible(false);
    actId->setEnabled(false);
    actFull->setEnabled(false);
    ui->tbtPicture->setVisible(false);
    ui->leId->setReadOnly(true);
    ui->lePhone->setReadOnly(true);
    ui->cbxCountry->setEnabled(false);
    ui->cbxSex->setEnabled(false);
    ui->deBirth->setReadOnly(true);
}

const char *cszQueryUserComplete{
    "SELECT a.name,a.nacid,a.birth,a.country,a.sex,a.phone,a.picture,b.whenreg,b.wholepic,"
    "b.docpic,b.whenrem,b.evicted,b.reasonout"
};

const char *cszQueryTailEdit{
    " FROM ausers a JOIN buregister b ON a.id=b.userid WHERE a.id=%1 AND b.id=%2"
};

void CNewUser::toEditUser(int id, int regId)
{
    QString sQuery1 = QString(cszQueryUserComplete) +
                      QString(cszQueryTailEdit).arg(id).arg(regId);
    QSqlDatabase db = QSqlDatabase::database(CDbInit::dbConnection());
    QSqlQuery q(db);
    QString sMessage;

    existingId = id;
    ui->dteRegister->setEnabled(false);
    ui->pbRemove->setVisible(true);
    showList = false;

    if (q.exec(sQuery1))
    {
        if (q.next())
        {
            usName = q.value(0).toString();
            ui->leName->setText(usName);
            uNacId = q.value(1).toString();
            ui->leId->setText(uNacId); //National ID
            uBirth = q.value(2).toDate();
            ui->deBirth->setDate(uBirth);
            uCountry = q.value(3).toString();
            ui->cbxCountry->setCurrentText(uCountry);
            uSex = q.value(4).toString();
            ui->cbxSex->setCurrentText(uSex);
            uPhone = q.value(5).toString();
            ui->lePhone->setText(uPhone);
            uPictureFace = q.value(6).toString();
            CWgtUserList::showPic(uPictureFace, ui->lblFacePicture);
            ui->dteRegister->setDateTime(q.value(7).toDateTime());
            uPictureFull = q.value(8).toString();
            CWgtUserList::showPic(uPictureFull, ui->lblFullPicture);
            uPictureNId = q.value(9).toString();
            CWgtUserList::showPic(uPictureNId, ui->lblIdPicture);
            isExistingUser = true; //No modification's taken place
            uExitTime = q.value(10).toDateTime();
            uEvicted = q.value(11).toInt() == 1;
            uReasonOut = q.value(12).toString();
        }
        else
        {
            sMessage = QString("No se encuentra el usuario, parece que ya no está"
                               " dentro del albergue");

            qWarning() << sMessage.toUtf8().data();
            QMessageBox::warning(this, "Advertencia", sMessage);
            QTimer::singleShot(150, this, SLOT(reject())); //Close this dialog
        }
    }
    else
    {
        sMessage = QString("No se puede buscar en la base de datos: %1")
                            .arg(q.lastError().text());

        qCritical() << sMessage.toUtf8().data();
        QMessageBox::critical(this, "Advertencia", sMessage);
        QTimer::singleShot(150, this, SLOT(reject())); //Close this dialog
    }
}

bool CNewUser::eventFilter(QObject *obj, QEvent *event)
{
    if (dynamic_cast<QLineEdit*>(obj) == ui->leId)
    {
        if (event->type() == QEvent::FocusIn && uc && uc->isVisible())
            uc->close();
    }
    else if (event->type() == QEvent::MouseButtonDblClick)
        QTimer::singleShot(150, this, [this, obj]() {
            showRealImage(obj);
        });
    return QObject::eventFilter(obj, event);
}

const char *cszPicFace{"Foto de rostro"};
const char *cszPicId{"Foto de identidad"};
const char *cszPicFull{"Foto completa"};

void CNewUser::prepareMenu()
{
    QMenu *mnu;

    //Prepare menu for pictures loading
    mnu = new QMenu(ui->tbtPicture);
    mnu->addAction(cszPicFace, QKeySequence("CTRL+R"), this, [this]() {
        if (selectPicture(ui->lblFacePicture, cszPicFace))
            isExistingUser = false; //Picture modified, then it aint existing
    });
    actId = mnu->addAction(cszPicId, QKeySequence("CTRL+I"), this, [this]() {
        selectPicture(ui->lblIdPicture, cszPicId);
    });
    actFull = mnu->addAction(cszPicFull, QKeySequence("CTRL+F"), this, [this]() {
        selectPicture(ui->lblFullPicture, cszPicFull);
    });
    ui->tbtPicture->setMenu(mnu);

    mnu->addAction("Cerrar vista previa", QKeySequence("CTRL+T"), this, [this] () {
        if (imgWidget != nullptr) imgWidget->close();});
}

bool CNewUser::selectPicture(QLabel *lbl, const QString &typePic)
{
    QString sFile;
    QFileInfo finf;

    sFile = QFileDialog::getOpenFileName(this, "Seleccione: " + typePic,
                                         lastFolder, csFilter, nullptr,
                                         QFileDialog::ReadOnly |
                                             QFileDialog::DontUseNativeDialog);
    finf.setFile(sFile);
    if (finf.exists())
    {
        int w = typePic.endsWith('o') ? 200 : (typePic.endsWith('d') ? 640 : 800);

        lbl->setPixmap(rescale(QPixmap(sFile), w));
        lastFolder = finf.absolutePath();
        return true;
    }
    else
        qWarning() << QString("El archivo de imagen seleccionado no existe: %1")
                          .arg(sFile).toUtf8().data();
    return false;
}

void CNewUser::setItemData(int id)
{
    QSqlDatabase db = QSqlDatabase::database(CDbInit::dbConnection());
    QSqlQuery q(db);
    QString sMessage;
    bool shl;

    if (q.exec(QString("SELECT name,nacid,birth,country,sex,phone,picture FROM ausers"
                       " WHERE id==%1").arg(id)))
    {
        if (q.next())
        {
            usName = q.value(0).toString();
            shl = showList;
            showList = false;
            ui->leName->setText(usName);
            uNacId = q.value(1).toString();
            ui->leId->setText(uNacId); //National ID
            uBirth = q.value(2).toDate();
            ui->deBirth->setDate(uBirth);
            uCountry = q.value(3).toString();
            ui->cbxCountry->setCurrentText(uCountry);
            uSex = q.value(4).toString();
            ui->cbxSex->setCurrentText(uSex);
            ui->lePhone->setText(q.value(5).toString());
            CWgtUserList::showPic(q.value(6).toString(), ui->lblFacePicture);
            isExistingUser = true; //Selected an existing user
            existingId = id; //User ID
            showList = shl;
        }
        else
        {
            sMessage = QString("Error de búsqueda del usuario (ID=%1) seleccionado")
                           .arg(id);

            qWarning() << sMessage.toUtf8().data();
        }
    }
    else
    {
        sMessage = QString("No se puede buscar en la base de datos: %1")
                               .arg(q.lastError().text());

        qCritical() << sMessage.toUtf8().data();
        QMessageBox::warning(this, "Advertencia", sMessage);
        QTimer::singleShot(150, this, SLOT(reject()));
    }
}

QPixmap CNewUser::rescale(const QPixmap &pm, int toWidth) const
{
    if (pm.width() < toWidth) return pm;
    return pm.scaledToWidth(toWidth);
}

const char *cslAcceptErrors[6]{
    "La fecha de nacimiento debe ser menor de ayer",
    "El nombre debe ser válido",
    "Debe seleccionar un país",
    "Debe seleccionar un género válido",
    "Debe proporcionar un teléfono válido o dejarlo en blanco",
    "Fecha y hora de registro inválida"
};

void CNewUser::acceptedClick()
{
    bool anyErr = true;
    int nError = 0;

    ex.setPattern(R"(^\+?\d+)");
    do
    {
        if (ui->deBirth->date() >= QDateTime::currentDateTime().date().addDays(-1)) break;
        nError++;
        if (ui->leName->text().isEmpty() || ui->leName->text().startsWith(' ')) break;
        nError++;
        if (ui->cbxCountry->currentText().isEmpty()) break;
        nError++;
        if (ui->cbxSex->currentText().isEmpty()) break;
        nError++;
        if (!ui->lePhone->text().isEmpty() && ui->lePhone->text().length() < 9 &&
            ui->lePhone->text().indexOf(ex) < 0) break;
        nError++;
        if (ui->dteRegister->dateTime() >= QDateTime::currentDateTime()) break;
        anyErr = false;
        //Check UI modification
        isExistingUser = ui->leId->text() == uNacId && ui->leName->text() == usName &&
                        ui->cbxCountry->currentText() == uCountry &&
                        ui->cbxSex->currentText() == uSex && ui->deBirth->date() == uBirth;
        //When editing an user, check images
        if (!showList && isExistingUser)
        {
            QString s1 = CWgtUserList::savePic(ui->lblFacePicture->pixmap()),
                s2 = CWgtUserList::savePic(ui->lblFullPicture->pixmap()),
                s3 = CWgtUserList::savePic(ui->lblIdPicture->pixmap());
            isExistingUser = ui->lePhone->text() == uPhone && s1 == uPictureFace &&
                             s2 == uPictureFull && s3 == uPictureNId;
        }
    } while (false);
    if (!anyErr) accept();
    else
    {
        qCritical() << QString("Error en datos del usuario: %1")
                           .arg(cslAcceptErrors[nError]).toUtf8().data();
        QMessageBox::critical(this, "Error", cslAcceptErrors[nError]);
    }
}

void CNewUser::userNameChanged(const QString &nt)
{
    //Show users list when changing user name and not editing an existing user
    if (showList)
    {
        bool qn = nt.length() > 2 && uc->queryName(nt);

        if (qn && uc->isHidden())
        {
            QRect rle = ui->leName->geometry();
            QPoint pgpos = mapToGlobal(rle.topRight());

            pgpos.setY(pgpos.y() + rle.height() + 2);
            pgpos.setX(pgpos.x() - rle.width());
            uc->move(pgpos);
            uc->showNormal();
        }
        else if (!qn && !uc->isHidden())
            uc->close();
    }
}

void CNewUser::showItem(int id) { setItemData(id); }

void CNewUser::selectItem(int id)
{
    setItemData(id);
    uc->close();
}

void CNewUser::showRealImage(QObject *obj)
{
    QLabel *lbl = dynamic_cast<QLabel*>(obj);

    if (lbl == ui->lblIdPicture || lbl == ui->lblFullPicture || lbl == ui->lblFacePicture )
    {
        if (lbl->pixmap().isNull())
            return;
        if (imgWidget == nullptr)
        {
            imgWidget = new QWidget(this, Qt::WindowType::Tool |
                                              Qt::WindowType::WindowStaysOnTopHint);
            QVBoxLayout *vbl = new QVBoxLayout(imgWidget);
            lblPic = new QLabel(imgWidget);

            imgWidget->setLayout(vbl);
            vbl->addWidget(lblPic);

            imgWidget->setWindowTitle("Imagen de identidad");
        }

        QScreen *scr = imgWidget->screen();
        int h = scr->availableGeometry().height(), w = scr->availableGeometry().width();

        lblPic->setMaximumSize(w - 50, h - 50);
        lblPic->setPixmap(lbl->pixmap());
        lblPic->setScaledContents(true);

        imgWidget->show();

        QRect r = imgWidget->geometry();
        QPoint p = imgWidget->mapToGlobal(r.topLeft());

        int hdiff = h - p.y() - imgWidget->height();
        int wdiff = w - p.x() - imgWidget->width();

        if (hdiff < 0)
            hdiff = h - imgWidget->height() - 20;
        else
            hdiff = p.y();

        if (wdiff < 0)
            wdiff = w - imgWidget->width() - 20;
        else
            wdiff = p.x();

        imgWidget->move(wdiff, hdiff);
    }
    else
        imgWidget->close();
}

void CNewUser::userExitData()
{
    CUserExitInfo *uei = new CUserExitInfo;

    uei->setReason(uReasonOut);
    uei->setEvicted(uEvicted);
    uei->setExitTime(uExitTime);
    if (uei->exec() == QDialog::Accepted)
    {
        outModified = (uReasonOut != uei->reason()) || (uEvicted != uei->evicted()) ||
                      (uExitTime != uei->exitTime());
        uReasonOut = uei->reason();
        uEvicted = uei->evicted();
        uExitTime = uei->exitTime();
    }
    delete uei;
}
