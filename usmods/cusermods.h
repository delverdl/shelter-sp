#ifndef CUSERMODS_H
#define CUSERMODS_H

#include <QWidget>

#include <cdbinit.h>
#include <cwgtuserlist.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class CUserMods;
}
QT_END_NAMESPACE

class CUserMods : public QWidget
{
    Q_OBJECT

public:
    CUserMods(QWidget *parent = nullptr);
    ~CUserMods();

private:
    Ui::CUserMods *ui;
    CDbInit *dbi;
    CWgtUserList *wusers;
    QSqlQueryModel *model;

    void initList();

private slots:
    void addClicked();
    void removeClicked();
    void resetClicked();
    void refreshModel();
};
#endif // CUSERMODS_H
