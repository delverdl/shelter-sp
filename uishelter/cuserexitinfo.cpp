#include "cuserexitinfo.h"
#include "ui_cuserexitinfo.h"

CUserExitInfo::CUserExitInfo(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CUserExitInfo)
{
    ui->setupUi(this);
    setWindowTitle("Información de salida");
}

CUserExitInfo::~CUserExitInfo()
{
    delete ui;
}

QDateTime CUserExitInfo::exitTime() const
{
    return ui->dteExit->dateTime();
}

bool CUserExitInfo::evicted() const
{
    return ui->ckbEvicted->checkState() == Qt::Checked;
}

QString CUserExitInfo::reason() const
{
    return ui->tedReason->toPlainText();
}

void CUserExitInfo::setExitTime(const QDateTime &dt)
{
    ui->dteExit->setDateTime(dt);
}

void CUserExitInfo::setEvicted(bool evt)
{
    ui->ckbEvicted->setChecked(evt);
}

void CUserExitInfo::setReason(const QString &rzn)
{
    ui->tedReason->setPlainText(rzn);
}
