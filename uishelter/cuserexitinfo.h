#ifndef CUSEREXITINFO_H
#define CUSEREXITINFO_H

#include <QDialog>

namespace Ui {
class CUserExitInfo;
}

class CUserExitInfo : public QDialog
{
    Q_OBJECT

public:
    explicit CUserExitInfo(QWidget *parent = nullptr);
    ~CUserExitInfo();

    QDateTime exitTime() const;
    bool evicted() const;
    QString reason() const;

public slots:
    void setExitTime(const QDateTime &dt);
    void setEvicted(bool evt);
    void setReason(const QString &rzn);

private:
    Ui::CUserExitInfo *ui;
};

#endif // CUSEREXITINFO_H
