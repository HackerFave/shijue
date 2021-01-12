
#include "publicClass.h"
#include <QUuid>
#include <QCryptographicHash>

publicClass* publicClass::P_publicClass = Q_NULLPTR;
publicClass::publicClass() 
{
	getIconPath();
    //iniSystemConfigFile();
}

QString publicClass::getIconPath()
{
	QString exePath = QApplication::applicationDirPath();
    if (exePath.contains("msdev")) {
        exePath = exePath.left(exePath.lastIndexOf("msdev"));
    }
	_exePath = exePath;
	return exePath;

}
void publicClass::iniSystemConfigFile()
{
    QString strExePath = QCoreApplication::applicationDirPath() + "/setting";
    QDir iniDir(strExePath);
    if (!iniDir.exists())
    {
        iniDir.mkdir(strExePath);
    }
    QString strFile = QString("%1/SysSetting.ini").arg(strExePath);
    QFile file(strFile);
    if (!file.exists())
    {
        QString strKey; 
        QVariant strValue;
        strKey = "spinBoxPenSize";
        strValue = "1";
        setIniFile(strKey, strValue);//画笔线宽

        strKey = "switchBtnFill";
        strValue = true;
        setIniFile(strKey, strValue);////标注显示是否填充

        strKey = "switchBtnInproportion";
        strValue = false;
        setIniFile(strKey, strValue);//是否保持按照原比例

        strKey = "switchWidgeTest";
        strValue = false;
        setIniFile(strKey, strValue);//是否保存测试结果

        strKey = "switchBtnChangeImageName";
        strValue = false;
        setIniFile(strKey, strValue);//增加图片时是否修改图片名

        strKey = "switchBtnFileLoadMode";
        strValue = false;
        setIniFile(strKey, strValue);//文件加载模式

        strKey = "moreTypeNum";
        strValue = 1;
        setIniFile(strKey, strValue);//文件加载模式

        strKey = "1";
        strValue = "class_1:128,0,0";
        setIniFile(strKey, strValue);//文件加载模式

    }
    else {
        if (configIni){
            delete configIni;
        }
        configIni = new QSettings(strFile, QSettings::IniFormat);
    }
}
void publicClass::iniSystemConfigFile(const QString &path)
{

    QString strExePath = path + "/setting";
    QDir iniDir(strExePath);
    if (!iniDir.exists())
    {
        iniDir.mkdir(strExePath);
    }
    QString strFile = QString("%1/SysSetting.ini").arg(strExePath);
    QFile file(strFile);
    if (!file.exists())
    {
        configIni = new QSettings(strFile, QSettings::IniFormat);
        QString strKey;
        QVariant strValue;
        strKey = "spinBoxPenSize";
        strValue = "1";
        setIniFile(strKey, strValue);//画笔线宽

        strKey = "switchBtnFill";
        strValue = true;
        setIniFile(strKey, strValue);////标注显示是否填充

        strKey = "switchBtnInproportion";
        strValue = false;
        setIniFile(strKey, strValue);//是否保持按照原比例

        strKey = "switchWidgeTest";
        strValue = false;
        setIniFile(strKey, strValue);//是否保存测试结果

        strKey = "switchBtnChangeImageName";
        strValue = false;
        setIniFile(strKey, strValue);//增加图片时是否修改图片名

        strKey = "switchBtnFileLoadMode";
        strValue = true;
        setIniFile(strKey, strValue);//文件加载模式

        strKey = "moreTypeNum";
        strValue = 1;
        setIniFile(strKey, strValue);//文件加载模式
        strKey = "1";
        strValue = "class_1:128,0,0";
        setIniFile(strKey, strValue);//文件加载模式


    }
    else {
        if (configIni){
            delete configIni;
        }
        configIni = new QSettings(strFile, QSettings::IniFormat);
    }
}
QString publicClass::getExeDirPath()
{
   return QApplication::applicationDirPath();
}
QMap<QString,QVariant> publicClass::readJson(const QString &jsonFile)
{
	QFile loadFile(jsonFile);
	if (!loadFile.open(QIODevice::ReadOnly)) {
		qDebug() << "File open failed!";
	}
	else {
		qDebug() << "File open successfully!";
	}
	QMap<QString, QVariant> map;
	QByteArray allData = loadFile.readAll();
	loadFile.close();
	QJsonParseError json_error;
	QJsonDocument jsonDoc(QJsonDocument::fromJson(allData, &json_error));

	if (json_error.error != QJsonParseError::NoError)
	{
		qDebug() << "json error!";
	}
	QJsonObject rootObj = jsonDoc.object();

	QStringList keys = rootObj.keys();


	for (int i = 0; i < keys.size(); i++)
	{
		qDebug() << "key" << i << " is:" << keys.at(i);

		QString key = keys.at(i);
		if (rootObj.contains(key))
		{			
			QJsonObject subObj = rootObj.value(key).toObject();
            if (subObj.isEmpty()) {
                map.insert(key, rootObj.value(key).toVariant());
            }
			QStringList keysChild = subObj.keys();
			for (int j = 0; j < keysChild.size(); j++) {
				//qDebug() << "keysChild" << keysChild.at(j) << subObj.value(keysChild.at(j)).toString() << "LLLLLLLLL";
				//map.insert(keys.at(i), rootObj.value(keys.at(i)).toVariant());
                map.insert(keysChild.at(j), subObj.value(keysChild.at(j)).toVariant());
			}
		}
	}
	return map;

}
void publicClass::writeJson(const QString &jsonFile ,QMap<QString,QMap<QString,QVariant>> map)
{
     int first = jsonFile.lastIndexOf("/"); //从后面查找"/"位置
    //QString pngName = jsonFile.right(jsonFile.length() - first - 1); //从右边截取
    QString dirPath = jsonFile.mid(0, first + 1);
    QDir dir(dirPath);
    if (!dir.exists()) {
        //dir.mkdir(dirPath);
        if (!dir.mkpath(dirPath)) {
            QMessageBox::question(nullptr, ("提示"),("文件保存失败"), QMessageBox::Yes);
        }
    }
	QFile file(jsonFile);
	if (!file.open(QIODevice::WriteOnly| QFile::Truncate)) {
		qDebug() << "File open failed!";
	}
	else {
		qDebug() << "File open successfully!";
	}

    QJsonObject rootJsonObject;
	QMap<QString, QMap<QString, QVariant>>::const_iterator it;
    // 使用QJsonObject对象插入键值对。
	for (it = map.constBegin(); it != map.constEnd(); ++it) {
        if (it.value().size() == 1) {
            QMap<QString, QVariant>::const_iterator iter;
            for (iter = it.value().begin(); iter != it.value().end(); iter++) {
                rootJsonObject.insert(iter.key(), iter.value().toString());
            }
            
        }
        else
        {
            QJsonObject childJson;
            QMap<QString, QVariant>::const_iterator iter;
            for (iter = it.value().begin(); iter != it.value().end(); iter++) {
                childJson.insert(iter.key(), iter.value().toString());
            }
            rootJsonObject.insert(it.key(), childJson);
        }
	}

    
    // 使用QJsonObject对象插入键值对。
    /*QJsonObject jsonObject;
    QJsonObject subJson;
    subJson.insert("111", "1");
    subJson.insert("222", "2");
    subJson.insert("333", "3");
    jsonObject.insert("name", "tom");
    jsonObject.insert("age", "18");
    jsonObject.insert("time", subJson);
    */
    // 使用QJsonDocument设置该json对象
    QJsonDocument jsonDoc;
    jsonDoc.setObject(rootJsonObject);

    // 将json以文本形式写入文件并关闭文件。
    file.write(jsonDoc.toJson());
    file.close();

}
void publicClass::changeJson(const QString &jsonFile,QMap<QString, QVariant> map)
{
	//QFile file(jsonFile);
	//QDir* dir = new QDir();
	//if (!dir->exists(jsonFile)) {
	//	dir->mkpath(jsonFile);
	//}

	//如果文件不存在，则创建
	QFile file(jsonFile);
	if (!file.exists()) {
		bool istrue = file.open(QIODevice::WriteOnly);
		file.close();
	}
	
	if (!file.open(QIODevice::ReadWrite)) {
		qDebug() << "File open failed!";

	}
	else {
		qDebug() << "File open successfully!";
	}

	QJsonDocument jdc(QJsonDocument::fromJson(file.readAll()));
	QJsonObject obj = jdc.object();
	QStringList keys = obj.keys();
	QMap<QString, QVariant>::const_iterator i;
	for (i = map.constBegin(); i != map.constEnd(); ++i) {
		//qDebug() << i.key() << ":" << i.value();
		for (int j = 0; j < keys.size(); j++)
		{
			
			if (i.key().simplified() == keys.at(j).simplified()) {
				obj[keys.at(j)] = i.value().toString();
			}
			obj.insert(i.key(), i.value().toString());
		}
		
	}		
			jdc.setObject(obj);
			//file.seek(0);
			file.close();
			if (file.open(QIODevice::WriteOnly)) {

				file.write(jdc.toJson());
					file.flush();
				}
			
		
}

QVariant publicClass::getJsonVariant(const QByteArray &json)
{
	// 1. 创建 QJsonParseError 对象，用来获取解析结果
	QJsonParseError error;//报告JSON过程中出现的错误
	QVariant var;    // 2. 使用静态函数获取 QJsonDocument 对象
	QJsonDocument jsonDocument = QJsonDocument::fromJson(json, &error);
	// 3. 根据解析结果进行处理
	if (error.error == QJsonParseError::NoError)
	{
		if (!(jsonDocument.isNull() || jsonDocument.isEmpty()))
		{
			if (jsonDocument.isObject())
			{
				var = jsonDocument.toVariant().toMap();
			}
			else if (jsonDocument.isArray())//数组
			{
				var = jsonDocument.toVariant().toList();
			}
		}
		else
		{
			//                   SCWarning<<"jsonDocument.isEmpty()";
			qWarning() << "jsonDocument.isEmpty()";
		}
	}
	else
	{
		//       SCWarning<<error.errorString().toUtf8().constData();
		qWarning() << "error:" << error.errorString().toUtf8().constData();
	}
	return var;
}
//Json转成QVariant
QVariant publicClass::getJsonVarian(const QString &filePath)
{
	
	QByteArray json;
	QFileInfo fileInfo(filePath);
	if (fileInfo.isFile()) {
		QFile file(fileInfo.absoluteFilePath());
		if (file.open(QIODevice::ReadOnly)) {
			
			json = file.readAll();
			
			file.close();
		}
		
	}
	else {
		qDebug() << fileInfo.filePath() << "is not a file";
	}
	
	return getJsonVariant(json);
}
void publicClass::addFolderImages(QString path)
{
	//判断路径是否存在
	QDir dir(path);
	if (!dir.exists())//判断目录是否存在
	{
		return;
	}
	dir.setFilter(QDir::Files | QDir::NoSymLinks);//设置类型过滤器，只为文件格式
//    dir.setNameFilters(filters);  //设置文件名称过滤器，只为filters格式（后缀为.jpeg等图片格式
//    entryList()可以获得目录中的所有文件和目录的名称的字符串列表。
//    如果你希望得到QFileInfo指针的列表请使用entryInfoList()
	QFileInfoList list = dir.entryInfoList();//获取文件目录的指针列表
	int file_count = list.count();
	if (file_count <= 0)
	{
		return;
	}
	QStringList string_list;
	for (int i = 0; i < list.size(); i++)
	{
		QFileInfo file_info = list.at(i);
		QString suffix = file_info.suffix();//判定文件的后缀、扩展名
		if (QString::compare(suffix, QString(""), Qt::CaseInsensitive) == 0)//Qt::CaseInsensitive不区分大小写
		{
			QString absolute_file_path = file_info.absoluteFilePath();
			string_list.append(absolute_file_path);
		}

	}
}
void publicClass::addJsonData(const QString &key, const QString &value, QJsonArray &classArray)
{
	//classArray[]
}
void publicClass::addJsonData(const QString &jsonFile, QJsonObject obj, QJsonArray classArray,QString &key)
{

	QFile file(jsonFile);
	if (!file.open(QIODevice::ReadWrite)) {
		qDebug() << "File open failed!";
	}
	else {
		qDebug() << "File open successfully!";
	}

	QJsonDocument jdc(QJsonDocument::fromJson(file.readAll()));

	QJsonObject rootObj = jdc.object();

	classArray.append(obj);

	rootObj[key] = classArray;
    
	QJsonDocument jdoc(rootObj);
	file.write(jdoc.toJson());
	file.flush();
}
bool publicClass::removeJson(const QString &jsonFile, const QString &key)
{
	QFile file(jsonFile);
	if (!file.open(QIODevice::ReadWrite)) {
		qDebug() << "File open failed!";
		return false;
	}
	else {
		qDebug() << "File open successfully!";
	}

	QJsonDocument jdc(QJsonDocument::fromJson(file.readAll()));
	file.close();
	QJsonObject obj = jdc.object();
	obj.remove(key);
	return true;
}
QMap<int,QStringList> publicClass::getfielInfo(const QString &filePath)
{
	QMap<int, QStringList> map;
	QDir dir(filePath);
	//QStringList filter;
	//filter<<"*.json";
	//dir.setNameFilters(filter); //过滤文件类型
	dir.setFilter(QDir::Files);
	//加过滤器，选择前缀为name的文件
	QStringList filters;
	filters << "*.*";
	dir.setNameFilters(filters);
	QFileInfoList list = dir.entryInfoList();
	QFileInfo fileInfo;
	for (int i = 0; i < list.size(); i++)
	{
		 QFileInfo fileInfo = list.at(i);
		 QDateTime time;
		// time = fileInfo.created();// 创建时间
		 time = fileInfo.lastModified();// 最近修改时间
		// time = fileInfo.lastRead();// 最近读时间
		 QString strCreatTime = time.toString("yyyy-MM-dd hh:mm:ss");
		 QStringList filelist;
		 filelist << fileInfo.fileName() << fileInfo.absoluteFilePath() << strCreatTime;
		 map.insert(i, filelist);

	}
	return map;
}
QMap<int, QStringList> publicClass::getFileInfoSort(const QString &filePath)
{
	int i, j; QMap<int, QStringList> map;
	QDir *dir = new QDir(filePath);
	QStringList filter;
	filter << "*.*";
	dir->setNameFilters(filter);
	QList<QFileInfo> fileInfo = QList<QFileInfo>(dir->entryInfoList(filter));

	if (fileInfo.size() > 49) {//如果>=50个，删除更早的图片
		QList<QDateTime> fileTime;
		for (i = 0; i < fileInfo.size(); i++) {
			fileTime.append(fileInfo.at(i).lastModified());
		}
		qSort(fileTime.begin(), fileTime.end());//容器元素的递增排序
		for ( i = 0; i < fileTime.size(); i++)
		{
			if (fileInfo.at(i).lastModified() == fileTime.at(i)) {
				QString strCreatTime = fileTime.at(i).toString("yyyy-MM-dd hh:mm:ss");
				QStringList filelist;
				filelist << fileInfo.at(i).fileName() << fileInfo.at(i).absoluteFilePath() << strCreatTime;
				map.insert(_num, filelist);
				_num++;
			}

		}
		//qSort(fileTime.begin(), fileTime.end(), qGreater<QDateTime>());
		/*for (i = 0; i < (fileTime.size() - 49); i++) {
			for (j = 0; j < fileTime.size(); j++) {
				if (fileInfo.at(j).lastModified() == fileTime.at(i))
					dir->remove(fileInfo.at(j).fileName());
			}
		}*/
	}
	_num = 0;
	return map;
}
void publicClass::saveReport(const QString &fileName, const QByteArray &data)
{
	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly| QIODevice::Text)) {
		return;
	}
	file.write(data);
	file.close();

}
void publicClass::saveReportExecl(const QString &fileName, const QByteArray &data)
{
	
}
void publicClass::saveReportExecl(const QString & fileName, int data)
{
	
}
QVector<QStringList> publicClass:: getfielInfoVector(const QString &filePath,const QString newFileName)
{
	QVector<QStringList> vectorFileList;
	QDir dir(filePath);
	//QStringList filter;
	//filter<<"*.json";
	//dir.setNameFilters(filter); //过滤文件类型
	dir.setFilter(QDir::Files);
	//加过滤器，选择前缀为name的文件
	QStringList filters;
	filters << "*.*";
	dir.setNameFilters(filters);
	QFileInfoList list = dir.entryInfoList();
	QFileInfo fileInfo;
	for (int i = 0; i < list.size(); i++)
	{
		QFileInfo fileInfo = list.at(i);
		QDateTime time;
		// time = fileInfo.created();// 创建时间
		time = fileInfo.lastModified();// 最近修改时间
	   // time = fileInfo.lastRead();// 最近读时间
		QString strCreatTime = time.toString("yyyy-MM-dd hh:mm:ss");
		QStringList filelist;
		filelist << fileInfo.baseName() << fileInfo.absoluteFilePath() << strCreatTime;
		if (QFileInfo(newFileName.simplified()).baseName() == fileInfo.baseName().simplified()) {
			/*|| fileInfo.baseName().simplified().contains(QFileInfo(newFileName.simplified()).baseName())*/
			vectorFileList.insert(0, filelist);
		}
		vectorFileList.insert(i, filelist);
		for (int n = 0; n < vectorFileList.size(); n++) {
			for (int m = n + 1; m < vectorFileList.size(); m++) {
				if (vectorFileList[m] == vectorFileList[n]) {
					vectorFileList.remove(m);
					n--;
				}
			}
		}

	}
	return vectorFileList;
}
void publicClass::setIniFile(const QString key,QVariant value)
{
	/*QFile file(QString("%1/setting/setting.ini").arg(_exePath));
	if (!file.open(QIODevice::WriteOnly)) {
		return;
	}*/
	//QSettings *configIni = new QSettings(QString("%1/setting/setting.ini").arg(_exePath), QSettings::IniFormat);

	configIni->setValue(QString("setting/%1").arg(key), value); 
	
}
void publicClass::removeIniValue()
{
    configIni->clear();
}
QVariant publicClass::readIniFile(const QString key)
{
    if (configIni) {
        //QSettings *configIni = new QSettings(QString("%1/setting/setting.ini").arg(_exePath), QSettings::IniFormat);
        return configIni->value(QString("setting/%1").arg(key)).toString();
    }
    return QVariant();
}

QString publicClass::getSelectCamera()
{
    QString strKey = "selectCamera";
    QString strSelectCamera = readIniFile(strKey).toString();
    QMap<QString, QString> mapCameraInfo = getCameraInfo();
    if (!mapCameraInfo.empty())
    {
        if (mapCameraInfo.contains(strSelectCamera))
        {
            return mapCameraInfo.find(strSelectCamera).value();
        }
    }
    return "";
}

QMap<QString, QString> publicClass::getCameraInfo()
{
     QMap<QString, QString> mapCameraInfo;
     QString strKeyCameraName = "CameraName";
     QString strKeyCameraVid = "CameraVid";
    QString strCameraName = readIniFile(strKeyCameraName).toString();
    QString strCameraVid = readIniFile(strKeyCameraVid).toString();
    QStringList strCameraNameList = strCameraName.split(",");
    QStringList strCameraVidList = strCameraVid.split(",");
    for (int i = 0; i < strCameraNameList.size();i++)
    {
        mapCameraInfo[static_cast<QString>(strCameraNameList.at(i))] = static_cast<QString>(strCameraVid.at(i));
     
    }
     return mapCameraInfo;
}

void publicClass::setNewFile(QString &fileName)
{
	_newFileName = fileName;
	
}


int publicClass::getDataBaseCount(QString text)
{
	return 0;
}
void publicClass::setPyConfig(const QMap<QString, QString> &mapConfig)
{
	QString filePath = QString("%1/algorithm/sys_config.py").arg(getIconPath());
	QFile file(filePath);
	if (file.open(QIODevice::ReadOnly)) {
		QString dataAll = QString(file.readAll());
		QString dataLine;
		QString dataReplace;
		file.close();
		if (file.open(QIODevice::ReadOnly)) {
			while (!file.atEnd())
			{
				dataLine = QString(file.readLine());
				dataReplace = dataLine;
				QMap<QString, QString>::iterator it; //遍历map  
				QMap<QString, QString> m_map = mapConfig;
				for (it = m_map.begin(); it != m_map.end(); ++it)
				{
					if (dataLine.contains(QString("%1 =").arg(it.key()))) {
						QStringList list;
						QStringList strList = dataLine.split("=");
						dataLine = dataLine.replace(strList.at(1), QString("'%1'\r\n").arg(it.value().simplified()));
					}
				}

				dataAll = dataAll.replace(dataReplace, dataLine);

			}
			file.close();
		}
		if (file.open(QIODevice::WriteOnly)) {
			file.write(dataAll.toLocal8Bit());
		}

	}
	file.close();
}
QMap<QString, QVariant> publicClass::readPyConfig(const QMap<QString, QVariant> &mapConfig)
{

	QMap<QString, QVariant> map;
	QString filePath = QString("%1/algorithm/sys_config.py").arg(getIconPath());
	QFile file(filePath);
	if (file.open(QIODevice::ReadOnly)) {
			
			while (!file.atEnd())
			{
				QString dataLine;
				dataLine = QString(file.readLine());
				QMap<QString, QVariant>::iterator it; //遍历map  
				QMap<QString, QVariant> m_map = mapConfig;
				for (it = m_map.begin(); it != m_map.end(); ++it)
				{
					if (dataLine.contains(QString("%1 =").arg(it.key()))) {
						QStringList list;
						QStringList strList = dataLine.split("=");
						//dataLine = dataLine.replace(strList.at(1), QString("'%1'\r\n").arg(it.value()));
						QString strReplace = strList.at(1);
						if (strReplace.contains("'")) {
							strReplace = strReplace.replace("'", "");
						 }
						if (strReplace.contains("\"")) {
							strReplace = strReplace.replace("\"", "");
						}
						
						map.insert(strList.at(0).simplified(), strReplace);
					}
				}			

			}
		}

	file.close();
	return map;
}

QString publicClass::getAppPath()
{
    return QApplication::applicationDirPath();
}

QString publicClass::dataTime(long long dataTime)
{
    auto startTime =std::chrono::duration_cast<std::chrono::seconds>(std::chrono::duration<long long,std::ratio_multiply<std::hecto,std::nano>>(dataTime));
    std::chrono::time_point<std::chrono::system_clock> stampTime(startTime);
    auto cur_time = std::chrono::system_clock::to_time_t(stampTime);
    struct tm stm;
    localtime_s(&stm, &cur_time);
    return QString().sprintf("%d-%02d-%02d %02d:%02d:%02d", stm.tm_year + 1900, stm.tm_mon + 1, stm.tm_mday, stm.tm_hour, stm.tm_min, stm.tm_sec);
}
long long publicClass::dataTime(QString dataTime)
{
    struct tm tm_;
    int year, month, day, hour, minute, second;
    std::string strTime = dataTime.toStdString();
    sscanf(strTime.c_str(), "%d-%02d-%02d %02d:%02d:%02d", &year, &month, &day, &hour, &minute, &second);
    tm_.tm_year = year - 1900;
    tm_.tm_mon = month - 1;
    tm_.tm_mday = day;
    tm_.tm_hour = hour;
    tm_.tm_min = minute;
    tm_.tm_sec = second;
    tm_.tm_isdst = 0;
    std::chrono::time_point<std::chrono::system_clock> tp = std::chrono::system_clock::from_time_t(mktime(&tm_));
    return  tp.time_since_epoch().count();
}
QMap<QString, QMap<int, qreal>> publicClass::getAllPointsMap(QString &filePath)
{
    QMap<QString, QMap<int, qreal>> allPointsMap;
    QStringList csvLineStrList;
    int numLine = 0;
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return allPointsMap;
    }
    while (!file.atEnd())
    {
        numLine++;
        QString line = QString(file.readLine()).replace(QString("\r\n"), QString("")).simplified();
        if (!line.isEmpty()) {
            csvLineStrList.append(line);
     }
    }
    if (csvLineStrList.size()<=0)
    {
        return allPointsMap;
    }
    QStringList listLine0 = csvLineStrList.at(0).split(',');
    for (int i = 0; i < listLine0.size(); i++)
    {
        QMap<int, qreal> map;
        for (int nLine = 1; nLine < csvLineStrList.size(); ++nLine) {
            map.insert(nLine, csvLineStrList.at(nLine).split(',').at(i).toDouble());            
        }
        allPointsMap.insert(listLine0.at(i), map);
    }
    file.close();
   /* QMap<QString, QMap<int, qreal>> ::const_iterator it;
    for (it = allPointsMap.constBegin(); it != allPointsMap.constEnd(); ++it) {
        if (it.key() == listLine0.at(1)) {
        }
        
    }*/
    _allPointsSize = numLine;
    return allPointsMap;   
}
int publicClass::getNumLine()
{
    return _allPointsSize;
}
void publicClass::addData()
{
    QFile file(QString("%1/train_log.csv").arg(getAppPath()));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append))
        return;
    t2 = t2 + 0.01;
    t3 = t3 + 0.02;
    t4 = t4 + 0.006;
    t5 = t5 + 0.041;
    t6 = t6 + 0.016;
    t7 = t7 + 0.019;
    t8 = t8 + 0.0021;
    t9 = t9 + 0.0044;

    QString data = QString("%1,%2,%3,%4,%5,%6,%7,%8,%9\r\n").arg(t1++).arg(t2).arg(t3).arg(t4).arg(t5).arg(t6).arg(t7).arg(t8).arg(t9);
    file.write(data.toLocal8Bit());
    file.close();
 }
QStringList publicClass::hexToRgb(const QString & colorStr)
{
    QString HexStr = colorStr;
    HexStr = HexStr.replace("#", "").simplified();
    if (HexStr.size() != 6) {
        return QStringList();
    }
    QString red = QString("%1%2").arg(HexStr.at(0)).arg(HexStr.at(1)).simplified();
    QString green = QString("%1%2").arg(HexStr.at(2)).arg(HexStr.at(3)).simplified();
    QString blue = QString("%1%2").arg(HexStr.at(4)).arg(HexStr.at(5)).simplified();
    QStringList list;
    list << QString::number(red.toInt(NULL, 16)) << QString::number(green.toInt(NULL, 16)) << QString::number(blue.toInt(NULL, 16));
    return list;

}
//获取文件夹里所有文件
QFileInfoList publicClass::GetFileList(QString path)
{

    QStringList filters;
    filters << QString("*.json");
   
    QDir dir(path);
    QFileInfoList file_list = dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    QFileInfoList folder_list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    dir.setFilter(QDir::Files | QDir::NoSymLinks); //设置类型过滤器，只为文件格式
    dir.setNameFilters(filters);  //设置文件名称过滤器，只为filters格式

    for (int i = 0; i != folder_list.size(); i++)
    {
        QString name = folder_list.at(i).absoluteFilePath();
        QFileInfoList child_file_list = GetFileList(name);
        file_list.append(child_file_list);
    }

    return file_list;
}
//初始化qcombobox样式
void publicClass::initComboBoxStyle(QMap<int, QComboBox *> comBoxMap)
{
    for (size_t j = 0; j < comBoxMap.size(); j++)
    {
        QStandardItemModel *pItemModel = qobject_cast<QStandardItemModel*>(comBoxMap.value(j)->model());
        //修改某项文本
        for (int i = 0; i < comBoxMap.value(j)->count(); i++)
        {
            pItemModel->item(i)->setForeground(QColor(255, 255, 255));            //修改某项文本颜色
            pItemModel->item(i)->setBackground(QColor(90, 90, 90));    //修改某项背景颜色    
            pItemModel->item(i)->setSizeHint(QSize(180, 30));
            pItemModel->item(i)->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);    //修改某项文本对齐方式
        }
    }

}


