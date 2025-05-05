#include "cusercheck.h"
#include "ui_cusercheck.h"

#include <QListWidget>
#include <QListWidgetItem>
#include <QSqlDatabase>
#include <QSqlQuery>

#include <cdbinit.h>

CUserCheck::CUserCheck(QWidget *parent)
    : QDialog(parent, Qt::WindowType::Tool | Qt::WindowType::WindowStaysOnTopHint)
    , ui(new Ui::CUserCheck)
{
    ui->setupUi(this);
    connect(ui->lwUsers, &QListWidget::currentItemChanged, this, &CUserCheck::currentItemChanged);
    connect(ui->lwUsers, &QListWidget::itemDoubleClicked, this, &CUserCheck::itemDoubleClicked);
}

CUserCheck::~CUserCheck()
{
    delete ui;
}

bool CUserCheck::queryName(const QString &n)
{
    QSqlDatabase db = QSqlDatabase::database(CDbInit::dbConnection());
    QSqlQuery q(db);
    QListWidgetItem *lwi;

    ui->lwUsers->clear();
    if (q.exec("SELECT id,name FROM ausers WHERE name LIKE '%" + n + "%'"))
    {
        while (q.next())
        {
            lwi = new QListWidgetItem;
            lwi->setText(q.value(1).toString());
            lwi->setData(Qt::UserRole, q.value(0).toInt());
            ui->lwUsers->addItem(lwi);
        }
    }
    return ui->lwUsers->count() != 0;
}

void CUserCheck::currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (current == nullptr) return;

    int id = current->data(Qt::UserRole).toInt();

    if (id <= 0) return; //Validate invalid ID
    emit showItem(id);
}

void CUserCheck::itemDoubleClicked(QListWidgetItem *item)
{
    int id = item->data(Qt::UserRole).toInt();

    if (id <= 0) return; //Validate invalid ID
    emit selectItem(id);
}
