#include "cnewuser.h"
#include "ui_cnewuser.h"

#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStandardPaths>
#include <cdbinit.h>

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
    : QDialog(parent), ui(new Ui::CNewUser), mayBeExisting(false), mayBeId(0)
{
    QStringList dl;

    ui->setupUi(this);
    connect(ui->pbAccept, &QPushButton::clicked, this, &CNewUser::acceptedClick);
    connect(ui->pbCancel, &QPushButton::clicked, this, &CNewUser::reject);
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
    uc = new CUserCheck();
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

bool CNewUser::existing() const { return mayBeExisting; }

int CNewUser::existingId() const { return mayBeId; }

void CNewUser::hideExtended()
{
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

void CNewUser::prepareMenu()
{
    QMenu *mnu;

    //Prepare menu for pictures loading
    mnu = new QMenu(ui->tbtPicture);
    mnu->addAction("Foto de rostro", QKeySequence("CTRL+R"), this, [this]() {
        if (selectPicture(ui->lblFacePicture))
            mayBeExisting = false; //Picture modified, then it aint existing
    });
    actId = mnu->addAction("Foto de identidad", QKeySequence("CTRL+I"), this, [this]() {
        selectPicture(ui->lblIdPicture);
    });
    actFull = mnu->addAction("Foto completa", QKeySequence("CTRL+F"), this, [this]() {
        selectPicture(ui->lblFullPicture);
    });
    ui->tbtPicture->setMenu(mnu);
}

bool CNewUser::selectPicture(QLabel *lbl)
{
    QString sFile;
    QFileInfo finf;

    sFile = QFileDialog::getOpenFileName(this, "Seleccione imagen", lastFolder, csFilter,
                                         nullptr, QFileDialog::ReadOnly | QFileDialog::DontUseNativeDialog);
    finf.setFile(sFile);
    if (finf.exists())
    {
        lbl->setPixmap(QPixmap(sFile));
        lastFolder = finf.absolutePath();
        return true;
    }
    return false;
}

void CNewUser::setItemData(int id)
{
    QSqlDatabase db = QSqlDatabase::database(CDbInit::dbConnection());
    QSqlQuery q(db);

    if (q.exec(QString("SELECT name,nacid,birth,country,sex,phone,picture FROM ausers"
                       " WHERE id==%1").arg(id)) && q.next())
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
        ui->lePhone->setText(q.value(5).toString());
        ui->lblFacePicture->setPixmap(QPixmap(QByteArray::fromBase64(q.value(6)
                                                                         .toByteArray())));
        mayBeExisting = true; //Selected an existing user
        mayBeId = id; //User ID
    }
}

const char *cslAcceptErrors[]{
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
        if (nError++ && ui->deBirth->date() >=
                            QDateTime::currentDateTime().date().addDays(-1)) break;
        if (nError++ && (ui->leName->text().isEmpty() ||
                         ui->leName->text().startsWith(' '))) break;
        if (nError++ && ui->cbxCountry->currentText().isEmpty()) break;
        if (nError++ && ui->cbxSex->currentText().isEmpty()) break;
        if (nError++ && !ui->lePhone->text().isEmpty() && ui->lePhone->text().length() < 9
            && ui->lePhone->text().indexOf(ex) < 0) break;
        if (ui->dteRegister->dateTime() >= QDateTime::currentDateTime()) break;
        anyErr = false;
        mayBeExisting = ui->leId->text() == uNacId && ui->leName->text() == usName &&
                        ui->cbxCountry->currentText() == uCountry &&
                        ui->cbxSex->currentText() == uSex && ui->deBirth->date() == uBirth;
    } while (false);
    if (!anyErr) accept();
    else
        QMessageBox::critical(this, "Error", cslAcceptErrors[nError]);
}

void CNewUser::userNameChanged(const QString &nt)
{
    bool qn = uc->queryName(nt);

    if (qn && uc->isHidden())
    {
        QRect rle = ui->leName->geometry();
        QPoint pgpos = mapToGlobal(rle.topRight());

        pgpos.setY(pgpos.y() + rle.height());
        uc->move(pgpos);
        uc->show();
    }
    else if (!qn && !uc->isHidden())
        uc->close();
}

void CNewUser::showItem(int id) { setItemData(id); }

void CNewUser::selectItem(int id)
{
    setItemData(id);
    uc->close();
}
