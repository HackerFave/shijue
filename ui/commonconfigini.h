#pragma once
#include <QObject>
#include <QString>
typedef std::map<QString, std::map<QString, QVariant>> configdata;
typedef std::map<int, configdata> listconfigdata;
typedef std::map<QString, int> nameconfigdata;
typedef std::map<int, QString> intconfigdata;
class CommonConfigIni : public QObject
{
	Q_OBJECT

public:
	static int ReadConfigIni(const QString str);
	static configdata ReadConfigIniData(int index);
	static bool WriteConfigIniData(configdata & data, int index);
	static bool WriteConfigIniData(int index, QString group, QString key, QVariant value);
	static bool SaveConfigIniData(int index);

private:
	static CommonConfigIni& instance();

private:
	CommonConfigIni(QObject *parent = NULL);
	~CommonConfigIni();

	int CReadConfigIni(const QString str);

	configdata CReadConfigIniData(int index);

	bool CWriteConfigIniData(configdata & data,int index);

	bool CSaveConfigIniData(int index);

	bool CWriteConfigIniData(int index, QString group, QString key, QVariant value);

private:
	listconfigdata	m_configdata;
	nameconfigdata	m_nameconfigdata;
	intconfigdata	m_intconfigdata;
	int m_index;
};
