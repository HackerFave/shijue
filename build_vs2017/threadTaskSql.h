#pragma once

#include <QObject>
#include <QRunnable>
class threadTaskSql : public QObject, public QRunnable
{

    Q_OBJECT
public:
    explicit threadTaskSql(QObject *parent = nullptr);

    void setThreadType(const int &type);

protected:
    void run();

private:
    void thread1Workder();
    void thread2Workder();
    void thread3Workder();
    void thread4Workder();
signals:
    void signalThreadType(int);
private:
    int m_type;
};
