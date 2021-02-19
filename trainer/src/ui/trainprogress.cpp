#include "trainprogress.h"
#include "alg.h"
#include "qfile.h"
#include "qtextstream.h"
TrainProgress::TrainProgress(QObject *parent)
    : QObject(parent)
{
}

TrainProgress::~TrainProgress()
{
}

void TrainProgress::SetProgressBarValue(int n)
{
    emit ResultReady(-1);
}


void TrainProgress::DoWork(QString p)
{
    int num = 0;
    int max = 0;
    emit ResultReady(max);
    while (alg::getstatus()==1)
    {
        _sleep(1);
        num = 0;
        QFile file(p);
        if (!file.exists())
            continue;
        if (!file.open(QIODevice::ReadOnly))
            continue;

        QTextStream inStream(&file);
        //把第一行去掉
        QString lineStr = inStream.readLine();
        while (!inStream.atEnd()) {
            lineStr = inStream.readLine();
            if (lineStr.isEmpty()) {
                continue;
            }
            num++;
        }
        if (num > max) {
            max = num;
            //发送信号
            emit ResultReady(max);
        }
        file.close();
    }
    emit ReadFinish();

    emit finished();
}

