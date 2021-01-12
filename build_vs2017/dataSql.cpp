#include "dataSql.h"
dataSql * dataSql::p_dataSql = NULL;
dataSql::dataSql(QObject *parent)
    : QObject(parent)
{
    _settingFilePath = publicClass::instance()->getAppPath() + "/setting/json/";
    p_sqlOperation = new sqlOperation(0);
    if (!p_sqlOperation->Init("127.0.0.1", "root", "123456", "zhongji", "typeJson"))
    {
        QMessageBox::warning(nullptr, tr("错误"), tr("初始化数据库失败"), QMessageBox::Ok);
    }
   
}
std::vector<std::list<QString>> dataSql::getRecordData()
{
    std::vector<std::list<QString>> data;
    return data;
}
QMap<QString, QString> dataSql::getThreshold(const QString &type)
{
    QMap<QString, QString> data;
   
    //获取json文件列表
    QFileInfoList fileInfoList;
    QSqlQuery * ret;
    QString table = "threshold_table";
    QMap<QString, QString> where;
   // where["model_name"] = _modelName;
    where["json_path"] = QString("%1%2_%3.json").arg(_settingFilePath.simplified())
        .arg(type.simplified()).arg(_modelName.simplified());
    QStringList keyList;
    keyList << "thresholdID" << "type" << "updatetime"
        << "gansha_width" << "gansha_height" << "kailie_width"
        << "kailie_height" << "huangban_color" << "xuhua_width"
        << "wuwu_area" << "bujun_width" << "wanzhe_curvature"
        << "model_name" << "json_path";
    QList<QMap<QString, QString>> rowData;
    if (!p_sqlOperation->findData(table, where, keyList, rowData, &ret)) {
        QMessageBox::warning(nullptr, tr("错误"), 
            tr("查询json数据失败（%1）").arg(ret->lastError().text()), QMessageBox::Ok);
    }
    if (rowData.size() <= 0) {
        return data;
    }
    return rowData.at(0);
}
void dataSql::slotThresholdJson(const QString &str)
{
    _modelName = str;
}