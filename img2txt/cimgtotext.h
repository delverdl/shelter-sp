#ifndef CIMGTOTEXT_H
#define CIMGTOTEXT_H

#include <QObject>

class CImgToText : public QObject
{
    Q_OBJECT
public:
    explicit CImgToText(QObject *parent = nullptr);

    void run(const QString &fileName);

private:
    QString fname;

private slots:
    void process();
};

#endif // CIMGTOTEXT_H
