#pragma once
#include <QObject>
class TrainProgress : public QObject
{
    Q_OBJECT

public:
    TrainProgress(QObject *parent = NULL);
    ~TrainProgress();

public:
    void SetProgressBarValue(int n);

public slots:
    void DoWork(QString p);


public:signals:
    void ResultReady(int n);
public:signals:
    void ReadFinish();
public:signals:
    void finished();//Íê³ÉÐÅºÅ
};
