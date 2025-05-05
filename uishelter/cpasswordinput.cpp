#include "cpasswordinput.h"
#include "ui_cpasswordinput.h"

#include <QTimer>

const char* cszInfo{R"(<html><head/><body><p><span style="color:#%1;">%2</span></p></body></html>)"};

CPasswordInput::CPasswordInput(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CPasswordInput)
{
    ui->setupUi(this);

    connect(ui->lePassword, &QLineEdit::textChanged, this, [this](const QString &t) {
        QString sMsg;
        int clNum;

        if (t.length() <= 6)
        {
            sMsg = "Contraeña muy corta...";
            clNum = 0xff0000;
        }
        else
        {
            bool hasUpper, hasLower, hasNumber, hasSpecial;
            int level = 0;

            hasUpper = hasLower = hasNumber = hasSpecial = false;
            foreach (const QChar ch, t)
            {
                if (!hasUpper && ch.isUpper()) { hasUpper = true; level += 20; }
                else if (!hasLower && ch.isLower()) { hasLower = true; level += 20; }
                else if (!hasNumber && ch.isNumber()) { hasNumber = true; level += 20; }
                else if (!hasSpecial && (ch.isSymbol() || ch.isMark() || ch.isPunct() ||
                                           ch.isSpace())) { hasSpecial = true; level += 20; }
            }
            switch (level)
            {
            case 100:
                clNum = 0x00ff;
                sMsg = "Excelente calidad";
                break;
            case 80:
                clNum = 0x103f;
                sMsg = "Buena calidad";
                break;
            case 60:
                clNum = 0x10a010;
                sMsg = "Calidad media!";
                break;
            case 40:
                clNum = 0xa1c300;
                sMsg = "Baja calidad!!";
                break;
            default:
                clNum = 0xe42f00;
                sMsg = "Mala contraseña!!!";
            }
        }
        ui->lblInfo->setText(QString(cszInfo).arg(clNum, 6, 16, '0').arg(sMsg));
    });

    connect(ui->leRepeat, &QLineEdit::textChanged, this, [this](const QString &t) {
        if (ui->lePassword->text() == t)
            ui->lblInfo->setText(QString(cszInfo).arg(0xff, 6, 16, '0')
                                     .arg("Contraseñas iguales"));
        else
            ui->lblInfo->setText(QString(cszInfo).arg(0xff0000, 6, 16, '0')
                                     .arg("Contraseñas diferentes!!!"));
        QTimer::singleShot(850, this, [this]() {
            QString sSave = ui->lePassword->text();

            ui->lePassword->clear();
            ui->lePassword->setText(sSave);
        });
    });

    connect(ui->pbAccept, &QPushButton::clicked, this, &CPasswordInput::accept);
    connect(ui->pbCancel, &QPushButton::clicked, this, &CPasswordInput::reject);

    connect(ui->tbtShow, &QToolButton::toggled, this, [this](bool ck) {
        QLineEdit::EchoMode em = ck ? QLineEdit::EchoMode::Normal :
                                     QLineEdit::EchoMode::Password;
        ui->lePassword->setEchoMode(em);
        ui->leRepeat->setEchoMode(em);
    });

    setWindowTitle("Entre la contraseña...");
}

CPasswordInput::~CPasswordInput()
{
    delete ui;
}

QString CPasswordInput::password() const
{
    return ui->lePassword->text();
}

void CPasswordInput::setOnlyPassword()
{
    ui->leRepeat->setVisible(false);
    ui->lblRepeat->setVisible(false);
    setMaximumHeight(156);
}
