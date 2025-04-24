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
    QHash<int, QString> names;
    int i = 0;

    if (q.exec("SELECT id,name FROM ausers WHERE name LIKE '%" + n + "%'"))
    {
        while (q.next())
            names.insert(q.value(0).toInt(), q.value(1).toString());
    }
    while (ui->lwUsers->count() < names.count())
        ui->lwUsers->addItem(new QListWidgetItem());
    while (ui->lwUsers->count() > names.count())
    {
        auto itf = ui->lwUsers->item(0);
        delete itf;
    }
    for (auto [key, value] : names.asKeyValueRange())
    {
        auto lwi = ui->lwUsers->item(i++);

        lwi->setData(Qt::UserRole, key);
        lwi->setText(value);
    }
    return ui->lwUsers->count() != 0;
}

void CUserCheck::currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    int id = current->data(Qt::UserRole).toInt();

    emit showItem(id);
}

void CUserCheck::itemDoubleClicked(QListWidgetItem *item)
{
    int id = item->data(Qt::UserRole).toInt();

    emit selectItem(id);
}
