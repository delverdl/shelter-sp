#ifndef CPASSWORDINPUT_H
#define CPASSWORDINPUT_H

#include <QDialog>

namespace Ui {
class CPasswordInput;
}

#include "uishelter_global.h"

class UISHELTER_EXPORT CPasswordInput : public QDialog
{
    Q_OBJECT

public:
    explicit CPasswordInput(QWidget *parent = nullptr);
    ~CPasswordInput();

    QString password() const;

    void setOnlyPassword();

private:
    Ui::CPasswordInput *ui;
};

#endif // CPASSWORDINPUT_H
