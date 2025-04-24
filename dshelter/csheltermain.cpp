#include "csheltermain.h"
#include "./ui_csheltermain.h"

CShelterMain::CShelterMain(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CShelterMain)
{
    ui->setupUi(this);
}

CShelterMain::~CShelterMain()
{
    delete ui;
}
