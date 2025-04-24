#include "cusermods.h"
#include "./ui_cusermods.h"

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
}

CUserMods::~CUserMods()
{
    delete ui;
    delete dbi;
}
