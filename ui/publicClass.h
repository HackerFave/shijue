#pragma once
#include <QObject>
#include <QFile>
#include <QFileDialog>
#include <QApplication>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QFile>
#include <QJsonObject>
#include <QDebug>
#include <QJsonArray>
#include <QDebug>
#include <QVariantMap>
#include <QVariant>
#include <qmap.h>
#include <qdatetime.h>
#include <qvector.h>
#include <QSettings>
#include <qmessagebox.h>
#include <qtabwidget.h>
#include <qheaderview.h>
#include <QComboBox>
#include <QStandardItemModel>

//#include <databasesqlite.h>
class publicClass
{
public:
	publicClass();
	static publicClass* instance()
	{
		if (P_publicClass == NULL)
		{
			P_publicClass = new publicClass();
		}
		return P_publicClass;
	}
	static void Release()
	{
		if (P_publicClass != NULL)
		{
			delete P_publicClass;
			P_publicClass = NULL;
		}
	}
	QString getIconPath();
    QString getExeDirPath();
    void iniSystemConfigFile();
    void iniSystemConfigFile(const QString &path);
	QMap<QString, QVariant> readJson(const QString &jsonFile);
	void writeJson(const QString &jsonFile, QMap<QString, QMap<QString, QVariant>> map);
	void changeJson(const QString &jsonFile, QMap<QString, QVariant> map );
	bool removeJson(const QString &jsonFile, const QString &key);//删除json
	QVariant getJsonVariant(const QByteArray &json);
	QVariant getJsonVarian(const QString &filePath);
	void addJsonData(const QString &key, const QString &value, QJsonArray &classArray);
	void addJsonData(const QString &jsonFile, QJsonObject obj, QJsonArray classArray, QString &key);
	void addFolderImages(QString path);
	QMap<int, QStringList> getfielInfo(const QString &filePath);
	QMap<int, QStringList> getFileInfoSort(const QString &filePath);
	QVector<QStringList> getfielInfoVector(const QString &filePath, const QString newFileName);
	void saveReport(const QString &fileName, const QByteArray &data);
	void saveReportExecl(const QString &fileName, const QByteArray &data);
	void saveReportExecl(const QString &fileName, int data);
	void setNewFile(QString &fileName);
	void setIniFile(const QString key, QVariant value);
    void removeIniValue();
	QVariant readIniFile(const QString key);
    QString getSelectCamera();  //增加获取系统设置选择的相机
    QMap<QString, QString> getCameraInfo();
	//QList <QStringList> DatabaseInterface(const int id);//获取数据库数据
	//QList <QStringList> DatabaseInterfaceFind(const int id,QString &text);//获取数据库数据
	int getDataBaseCount(QString text);
	void setPyConfig(const QMap<QString, QString> &mapConfig);
	QMap<QString, QVariant> readPyConfig(const QMap<QString, QVariant> &mapConfig);
    QString dataTime(long long dataTime);
     long long dataTime(QString dataTime);
    QString getAppPath();
    QMap<QString, QMap<int, qreal>> getAllPointsMap(QString &filePath);
    int getNumLine();
    void addData();
    QStringList hexToRgb(const QString & colorStr);
    QFileInfoList GetFileList(QString path);
    void initComboBoxStyle(QMap<int, QComboBox *> comBoxMap);
private:
	static publicClass* P_publicClass;
	QString _exePath;
	unsigned int _num = 0;
public:
	QString _newFileName;
    QSettings *configIni = Q_NULLPTR;
    int _allPointsSize = 0;
    int t1 = 25; 
    float t2 = 0.0763621263206005;
    float t3 = 0.8636335452561719;
    float t4 = 0.8165197866384245;
    float t5 = 0.764227011324462;
    float t6 = 0.99999990000001;
    float t7 = 0.6249999609375024;
    float t8 = 0.7692302366866823;
    float t9 = 0.8165197866384245;

};

