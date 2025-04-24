#ifndef CWGTUSERLIST_H
#define CWGTUSERLIST_H

#include <QSqlQueryModel>
#include <QWidget>

#include <cdbinit.h>

namespace Ui {
class CWgtUserList;
}

#include "uishelter_global.h"

class UISHELTER_EXPORT CWgtUserList : public QWidget
{
    Q_OBJECT

public:
    explicit CWgtUserList(QWidget *parent = nullptr);
    ~CWgtUserList();

    void setDbInit(CDbInit *dbi_);

signals:
    void queryError(const QString &err);

public slots:
    void populateDb(bool addVisible);
    void addOne();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    Ui::CWgtUserList *ui;
    QSqlQueryModel *model;
    CDbInit *dbi;

    void showPic(const QString &data);

private slots:
    void clicked(const QModelIndex &midx);
    void checkStateChanged(Qt::CheckState ck);

};

#endif // CWGTUSERLIST_H
