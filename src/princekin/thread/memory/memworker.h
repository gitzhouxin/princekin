#ifndef MEMWORKER_H
#define MEMWORKER_H

#include "windows.h"
#include "globalvar.h"

#include <QDebug>
#include <QString>

#include <QByteArray>
#include <QRegExp>
#include <QProcess>
#include <QTextCodec>

namespace MemWorker
{
class Worker : public QObject
{
    Q_OBJECT
public:
    Worker();

public:
    void initVar();
    void setDeviceId(const QString &);
    void setCmdLine(const QString &);
    void startWorker();

signals:
    void sendResult(const QString &,const QString &,int,int);

private slots:
    void receiveStopWorker();

public:
    bool qIsStop;

private:
    QString qDeviceId;
    QString qCmdLine;
    QTextCodec *qTc=QTextCodec::codecForName("gb2312");
};

}

#endif // MEMWORKER_H
