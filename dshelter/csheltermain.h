#ifndef CSHELTERMAIN_H
#define CSHELTERMAIN_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class CShelterMain;
}
QT_END_NAMESPACE

class CShelterMain : public QWidget
{
    Q_OBJECT

public:
    CShelterMain(QWidget *parent = nullptr);
    ~CShelterMain();

private:
    Ui::CShelterMain *ui;
};
#endif // CSHELTERMAIN_H
