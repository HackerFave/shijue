#include "threadTaskSql.h"
#include <QDebug>
#include <QThread>
#include <QSqlDatabase>
#include <QDateTime>

threadTaskSql::threadTaskSql(QObject *parent)
    : QObject(parent),
    m_type(0)
{
}

void threadTaskSql::setThreadType(const int &type)
{
    m_type = type;
}

void threadTaskSql::run()
{
    switch (m_type) {
    case 0:
        //thread1Workder();
        emit signalThreadType(0);
        break;
    case 1:
        //thread2Workder();
        emit signalThreadType(1);
        break;
    case 2:
        emit signalThreadType(2);
       // thread3Workder();
        break;
    case 3:
       // thread4Workder();
        emit signalThreadType(3);
        break;
    default:
        break;
    }
}

void threadTaskSql::thread1Workder()
{
    /*
    QString connectionName = QString::number(*static_cast<int*>(QThread::currentThreadId()));
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", connectionName);
    db.setHostName("127.0.0.1");
    db.setDatabaseName("testdb");
    db.setUserName("root");
    db.setPassword("1234");
    if (!db.open())
    {
        qDebug() << "thread 1 open db failed!";
    }
    qDebug() << "thread 1: " << QThread::currentThreadId() << connectionName;
    */
}

void threadTaskSql::thread2Workder()
{
    QString connectionName = QString::number(*static_cast<int*>(QThread::currentThreadId()));
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", connectionName);
    db.setHostName("127.0.0.1");
    db.setDatabaseName("testdb");
    db.setUserName("root");
    db.setPassword("1234");
    if (!db.open())
    {
        qDebug() << "thread 1 open db failed!";
    }
    qDebug() << "thread 2: " << QThread::currentThreadId() << connectionName;
}

void threadTaskSql::thread3Workder()
{
    QString connectionName = QString::number(*static_cast<int*>(QThread::currentThreadId()));
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", connectionName);
    db.setHostName("127.0.0.1");
    db.setDatabaseName("testdb");
    db.setUserName("root");
    db.setPassword("1234");
    if (!db.open())
    {
        qDebug() << "thread 1 open db failed!";
    }
    qDebug() << "thread 3: " << QThread::currentThreadId() << connectionName;
}

void threadTaskSql::thread4Workder()
{
    QString connectionName = QString::number(*static_cast<int*>(QThread::currentThreadId()));
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", connectionName);
    db.setHostName("127.0.0.1");
    db.setDatabaseName("testdb");
    db.setUserName("root");
    db.setPassword("1234");
    if (!db.open())
    {
        qDebug() << "thread 1 open db failed!";
    }
    qDebug() << "thread 4: " << QThread::currentThreadId() << connectionName;
}
