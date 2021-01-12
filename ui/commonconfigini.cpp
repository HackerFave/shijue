#include "commonconfigini.h"
#include "qsettings.h"
#include "QFileInfo"
CommonConfigIni::CommonConfigIni(QObject *parent)
	: QObject(parent)
{
	m_index = 0;
}

CommonConfigIni::~CommonConfigIni()
{
}

CommonConfigIni& CommonConfigIni::instance()
{
	static CommonConfigIni the_instance;
	return the_instance;
}

int CommonConfigIni::ReadConfigIni(const QString str)
{
	return CommonConfigIni::instance().CReadConfigIni(str);
}

configdata CommonConfigIni::ReadConfigIniData(int index)
{
	return CommonConfigIni::instance().CReadConfigIniData(index);
}

bool CommonConfigIni::WriteConfigIniData(configdata & data, int index)
{
	return CommonConfigIni::instance().CWriteConfigIniData(data,index);
}

bool CommonConfigIni::WriteConfigIniData(int index, QString group, QString key, QVariant value)
{
	return CommonConfigIni::instance().CWriteConfigIniData(index, group, key,value);
}

bool CommonConfigIni::SaveConfigIniData(int index)
{
	return CommonConfigIni::instance().CSaveConfigIniData(index);
}

/***************************************************************
  	*  @brief    通用读取配置文件，将读取到的配置文件放入缓存，
	返回读取文件的ID。
  	*  @param     str:配置文件所在路径。
  	*  @note     
  	*  @return	  返回去读数据的ID。
**************************************************************/
int CommonConfigIni::CReadConfigIni(const QString str)
{
	if (str.isEmpty())
		return -1;
	if (instance().m_nameconfigdata.count(str))
		return instance().m_nameconfigdata.at(str);

	instance().m_index++;
	instance().m_nameconfigdata[str] = instance().m_index;
	instance().m_intconfigdata[instance().m_index] = str;
	//将数据添加到内存中
	QSettings configini(str, QSettings::IniFormat);
	// 获取所有节点
	QStringList strlist = configini.childGroups();
	int icount = strlist.count();
	//获取每个节点的数据
	//存入数据
	QStringList childlist;
	configdata tconfigdata;
	std::map<QString, QVariant> data;

	for (int i = 0; i < icount; i++)
	{
		configini.beginGroup(strlist.at(i));
		childlist = configini.allKeys();
		data.clear();
		for (int j = 0; j < childlist.count(); j++)
		{
			data[childlist.at(j)] = configini.value(childlist.at(j));
		}
		tconfigdata[strlist.at(i)] = data;
		configini.endGroup();
	}
	instance().m_configdata[instance().m_index] = tconfigdata;
	return instance().m_index;
}


/***************************************************************
  	*  @brief    读取指定索引的数据。
  	*  @param    index:配置文件列表。
  	*  @note     
  	*  @return	 
**************************************************************/
configdata CommonConfigIni::CReadConfigIniData(int index)
{
	if (m_configdata.count(index) > 0)
		return m_configdata.at(index);
	return configdata();
}

/***************************************************************
  	*  @brief    写入指定索引的数据。
  	*  @param    index:配置文件列表。
  	*  @note     
  	*  @return
**************************************************************/
bool CommonConfigIni::CWriteConfigIniData(configdata& data, int index)
{
	if (m_configdata.count(index) > 0)
	{
		m_configdata[index] = data;
		return true;
	}
	return false;
}


/***************************************************************
  	*  @brief    保存指定索引的数据。
  	*  @param    index:配置文件列表。
  	*  @note     
  	*  @return
**************************************************************/
bool CommonConfigIni::CSaveConfigIniData(int index)
{
	if (m_configdata.count(index) < 1)
		return false;

	configdata data = m_configdata.at(index);
	QSettings configini(m_intconfigdata[index], QSettings::IniFormat);
	for each (auto var in data)
	{
		configini.beginGroup(var.first);
		for each (auto var1 in var.second)
		{
			configini.setValue(var1.first, var1.second);
		}
		configini.endGroup();
	}
	return true;
}
/***************************************************************
  	*  @brief    保存指定索引的数据。
  	*  @param    index:配置文件列表。
  	*  @note     
  	*  @return
**************************************************************/
bool CommonConfigIni::CWriteConfigIniData(int index, QString group,QString key, QVariant value)
{
	//if (m_configdata.count(index) < 1)
	//	return false;

	
	m_configdata[index][group][key] = value;
	configdata data = m_configdata.at(index);
	QSettings configini(m_intconfigdata[index], QSettings::IniFormat);
	configini.beginGroup(group);
	configini.setValue(key, value);
	configini.endGroup();
	return true;
}
