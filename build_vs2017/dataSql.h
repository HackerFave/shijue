#pragma once

#include <QObject>
#include <list>
#include <vector>
#include <QMap>
#include "sqlOperation.h"
#include "publicClass.h"
class dataSql : public QObject
{

    Q_OBJECT
public:
    explicit dataSql(QObject *parent = 0);
    ~dataSql() {
        if (p_dataSql != NULL)
        {
            delete p_dataSql;
            p_dataSql = NULL;
        }
        if (p_sqlOperation) {
            p_sqlOperation->CloseDB();
            delete p_sqlOperation;
        }
    }
    static dataSql *getInstance()
    {
        if (p_dataSql == NULL) {
            p_dataSql = new dataSql;
        }

        return p_dataSql;
    }
    QMap<QString, QString> getThreshold(const QString &type);
public slots:
    void slotThresholdJson(const QString &);
private slots:
public:
    void setData(const std::map<QString, QString> data)
    {
        emit sendDataSql(data);
    }
    std::vector<std::list<QString>> getRecordData();

private:
    
    static dataSql *p_dataSql;
    sqlOperation * p_sqlOperation = Q_NULLPTR;
    QString _modelName;//当前加载方案名称
    QString _settingFilePath;//json文件所在路径
signals:
    void sendDataSql(const std::map<QString, QString>);


};
