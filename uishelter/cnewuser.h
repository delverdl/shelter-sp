#ifndef CNEWUSER_H
#define CNEWUSER_H

#include "cusercheck.h"

#include <QDate>
#include <QDialog>
#include <QRegularExpression>

class QLabel;

namespace Ui {
class CNewUser;
}

#include "uishelter_global.h"

class UISHELTER_EXPORT CNewUser : public QDialog
{
    Q_OBJECT

public:
    explicit CNewUser(QWidget *parent = nullptr);
    ~CNewUser();

    QString name() const;
    QString nationalId() const;
    QDate birth() const;
    QString country() const;
    QString sex() const;
    QString phone() const;
    QPixmap facePicture() const;
    QPixmap fullPicture() const;
    QPixmap idPicture() const;
    QDateTime registrationTime() const;

    bool existing() const;
    int existingId() const;

    void hideExtended();

private:
    Ui::CNewUser *ui;
    QRegularExpression ex;
    QString lastFolder;
    CUserCheck *uc;
    bool mayBeExisting;
    int mayBeId;
    QString usName;
    QString uNacId;
    QDate uBirth;
    QString uCountry;
    QString uSex;
    QAction *actId;
    QAction *actFull;

    void prepareMenu();
    bool selectPicture(QLabel *lbl);
    void setItemData(int id);

private slots:
    void acceptedClick();
    void userNameChanged(const QString &nt);
    void showItem(int id);
    void selectItem(int id);

};

#endif // CNEWUSER_H
