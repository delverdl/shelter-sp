#ifndef CUSERCHECK_H
#define CUSERCHECK_H

#include <QDialog>
#include <QListWidget>

namespace Ui {
class CUserCheck;
}

class CUserCheck : public QDialog
{
    Q_OBJECT

public:
    explicit CUserCheck(QWidget *parent = nullptr);
    ~CUserCheck();

public slots:
    bool queryName(const QString &n);

signals:
    void showItem(int id);
    void selectItem(int id);

private:
    Ui::CUserCheck *ui;

private slots:
    void currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void itemDoubleClicked(QListWidgetItem *item);

};

#endif // CUSERCHECK_H
