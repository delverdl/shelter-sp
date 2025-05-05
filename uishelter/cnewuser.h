#ifndef CNEWUSER_H
#define CNEWUSER_H

#include "cusercheck.h"

#include <QDate>
#include <QDialog>
#include <QRegularExpression>
#include <QShortcut>

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
    QString reasonOut() const;
    bool evicted() const;
    QDateTime exitTime() const;

    bool existing() const; //To modify an existing user
    bool outDataModified() const;
    int existingUserId() const; //Existing user id to modify

    void hideExtended();
    void toEditUser(int id, int regId);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    Ui::CNewUser *ui;
    QRegularExpression ex;
    QString lastFolder;
    CUserCheck *uc;
    QWidget *imgWidget;
    QLabel *lblPic;
    bool isExistingUser;
    int existingId;

    //---TO CHECK DATA CHANGE---
    QString usName;
    QString uNacId;
    QDate uBirth;
    QString uCountry;
    QString uSex;
    QString uPhone;
    QString uPictureFace;
    QString uPictureNId;
    QString uPictureFull;
    bool uEvicted;
    QString uReasonOut;
    QDateTime uExitTime;
    //---TO CHECK DATA CHANGE---

    QAction *actId;
    QAction *actFull;
    bool showList;
    bool outModified;

    void prepareMenu();
    bool selectPicture(QLabel *lbl, const QString &typePic);
    void setItemData(int id);
    QPixmap rescale(const QPixmap &pm, int toWidth) const;

private slots:
    void acceptedClick();
    void userNameChanged(const QString &nt);
    void showItem(int id);
    void selectItem(int id);
    void showRealImage(QObject *obj);
    void userExitData();
};

#endif // CNEWUSER_H
