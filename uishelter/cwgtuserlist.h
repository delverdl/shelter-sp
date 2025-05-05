#ifndef CWGTUSERLIST_H
#define CWGTUSERLIST_H

#include <QSqlQueryModel>
#include <QWidget>

#include <cdbinit.h>

class QLabel;
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

    static QString savePic(const QPixmap &pm);
    static void showPic(const QString &data, QLabel *lbl);
    static const char *listQueryTail();

public slots:
    void populateDb(bool addVisible = false);
    void addOne();
    void editOne();
    void setCurrentGuard(int guardId = 0);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    Ui::CWgtUserList *ui;
    QSqlQueryModel *model;
    CDbInit *dbi;
    int guard;

    void refreshModel();
    void setHeaders();

private slots:
    void clicked(const QModelIndex &midx);
    void checkStateChanged(Qt::CheckState ck);

};

#endif // CWGTUSERLIST_H
