
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

        strKey = "fileExportMode";
        strValue = "onnx";
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

		//QString key = keys.at(i);
		//if (rootObj.contains(key))
		//{			
		//	QJsonObject subObj = rootObj.value(key).toObject();
		//	QStringList keysChild = subObj.keys();
			//for (int j = 0; j < keysChild.size(); j++) {
				//qDebug() << "keysChild" << keysChild.at(j) << subObj.value(keysChild.at(j)).toString() << "LLLLLLLLL";
				map.insert(keys.at(i), rootObj.value(keys.at(i)).toVariant());
			//}
		//}
	}
	return map;

}
void publicClass::writeJson(const QString &jsonFile ,QMap<QString,QVariant> map)
{
	QFile file(jsonFile);
	if (!file.open(QIODevice::WriteOnly)) {
		qDebug() << "File open failed!";
	}
	else {
		qDebug() << "File open successfully!";
	}

	QJsonObject rootObj;
	QMap<QString, QVariant>::const_iterator i;
	for (i = map.constBegin(); i != map.constEnd(); ++i) {
		qDebug() << i.key() << ":" << i.value();
		rootObj.insert(i.key(), i.value().toString());
	}
	QJsonDocument jdoc(rootObj);
	file.write(jdoc.toJson());
	file.flush();

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
		qDebug() << i.key() << ":" << i.value();
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
QByteArray publicClass::Image_To_Base64(QString ImgPath ,int width,int height)

{

    QImage image(ImgPath);
    image = image.scaled(width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    QByteArray ba;

    QBuffer buf(&ba);

    image.save(&buf, "PNG", 20);

    QByteArray hexed;
    hexed += "data:image/png;base64,";
    hexed += ba.toBase64();

    buf.close();

    return hexed;

}

QPixmap publicClass::Base64_To_Image(QByteArray bytearray, QString SavePath)

{

    QByteArray Ret_bytearray;

    Ret_bytearray = QByteArray::fromBase64(bytearray);

    QBuffer buffer(&Ret_bytearray);

    buffer.open(QIODevice::WriteOnly);

    QPixmap imageresult;

    imageresult.loadFromData(Ret_bytearray);

    if (SavePath != "")

    {

        qDebug() << "save";

        imageresult.save(SavePath);

    }

    return imageresult;

}


/* 调用

{

    QByteArray src = Image_To_Base64("d:/result.png");

    QPixmap dest = Base64_To_Image(src, "e:/result.png");

    //ui->label->setPixmap(dest);	//可用Qlabel显示

    return 0;

}
*/
void publicClass::saveHtmlFile(QList<QByteArray> listByteViewData,/* const QStringList &fileList,*/ const QString &Dirpath)
{
    QByteArray arr = getHtmlData(listByteViewData, /*fileList,*/Dirpath);
    QString fileName = QString("D:/workT/test/%1.html").arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz"));
    QFile file(fileName);
    if (file.open(QIODevice::ReadWrite | QIODevice::Truncate)) {
        file.write(arr);
    }
    file.close();
    if (QMessageBox::question(nullptr, ("完成"), ("文件已经导出，是否现在打开？"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
    {
        QDesktopServices::openUrl(QUrl("file:///" + QDir::toNativeSeparators(fileName)));
    }

}
/*导出html报告
listByteViewData: roc 与图片集曲线图
fileList : 图片文件列表
*/
QByteArray publicClass::getHtmlData(QList<QByteArray> listByteViewData,/* const QStringList &fileList,*/ const QString &Dirpath)
{
      //logo base数据
      QByteArray logoData = Image_To_Base64(QString("%1/icons/tool_logo.png").arg(getAppPath()),100,100);
      QByteArray vidiData = Image_To_Base64(QString("%1/icons/vidi.png").arg(getAppPath()),100,100);

      bool isDefect = true;

      QList<QByteArray> listPng;
      QString dirPath = Dirpath/*"D:/workT/manuvision-platform/build/shoes0616/imagedata/"*/;
      QDir dir(dirPath);
      QStringList nameFilters;
      nameFilters << "*.jpg" << "*.png";
      QStringList files = /*fileList */dir.entryList(nameFilters, QDir::Files | QDir::Readable, QDir::Name);
      for (int i = 0; i < files.size(); i++) {
          QByteArray byte = Image_To_Base64(QString("%1/%2").arg(dirPath).arg(files.at(i)),100,100);
         // QByteArray byte = Image_To_Base64(QString("%1").arg(files.at(i)), 100, 100);
          listPng.push_back(byte);
      }

      QString str1 = "100";
      QString str2 = "10";
      QString operitor = "谭创";
      QByteArray htmlStr = "";
      QFile file(QString("%1/icons/html.txt").arg(getAppPath()));
      if(file.open(QIODevice::ReadOnly)) {
          htmlStr += file.readAll();
      }
        
        htmlStr += "<body>" + '\n';
        htmlStr += "<div id = \"upper_div\">\n";
        htmlStr += "<table>\n";
        htmlStr += "<tr>\n";
        htmlStr += QString("<th>%1</th>\n").arg(operitor);//作者
        htmlStr += "<th>工具</th>\n";
        htmlStr += "<td>ManuVision\n";
        htmlStr += QString("<img class = \"tool_logo\" src = \"%1\" style = \"vertical-align:middle;\"/>\n")
            .arg(QString(logoData)).toLocal8Bit();//logo
        htmlStr += "</td>\n";
        htmlStr += "<td rowspan = \"2\" style = \"text-align:right;\">\n";
        htmlStr += QString("<img class = \"vidi_logo\" src = \"%1\" />\n").arg(QString(vidiData)).toLocal8Bit();//vidi logo
        htmlStr += "</td>\n";
        htmlStr += "</tr>\n";
        htmlStr += "<tr>\n";
        //样本数
        htmlStr += "<td style = \"text-align:center;\">";
        htmlStr += QString("%1").arg(str1).toLocal8Bit();
        htmlStr += "个样本，";
        htmlStr += QString("%1").arg(str2).toLocal8Bit();
        htmlStr += "个已训练</td>\n";
        htmlStr += "<th>日期</th>\n";
        htmlStr += QString("<td>%1</td>\n").arg(QDateTime::currentDateTime().toString("yyyy / MM / dd")).toLocal8Bit();//日期
        htmlStr += "</tr>\n";
        htmlStr += "</table>\n";
        htmlStr += "</div>\n";
        htmlStr += "<div id = \"lower_div\">\n";
        htmlStr += "<div class = \"stats\">\n";

        htmlStr += "<span style = \"text-align:center;display:inline-block;\">\n";
        htmlStr += "<h3>分布</h3>\n";//分布
        //htmlStr += "<img src = \"data:image/png;base64,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\"/>\n";
        htmlStr += QString("<img src = \"%1\"/>\n").arg(QString(listByteViewData.at(0)));
        htmlStr += "</span>\n";
        htmlStr += "<span style = \"text-align:center;display:inline-block;\">\n";
        htmlStr += "<h3>ROC</h3>\n";//ROC曲线
       // htmlStr += "<img src = \"data:image/png;base64,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\"/>\n";
        htmlStr += QString("<img src = \"%1\"/>\n").arg(QString(listByteViewData.at(1)));
        htmlStr += "</span>\n";
        htmlStr += "<span style = \"text-align:center;display:inline-block;\">\n";
        htmlStr += "<h3>混淆矩阵</h3>\n";//
       // htmlStr += "<img src = \"data:image/png;base64,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\"/>\n";
        htmlStr += QString("<img src = \"%1\"/>\n").arg(QString(listByteViewData.at(2)));
        htmlStr += "</span>\n";
       /* htmlStr += "<table style = \"border-style:none;\">\n";
        htmlStr += "<tr>\n";
        htmlStr += "<td colspan = \"2\"></td><td colspan = \"3\" style = \"border-bottom:1px solid gray;text-align:center;\">预期</td>\n";
        htmlStr += "</tr><tr>\n";
        htmlStr += "<td colspan = \"2\"></td><td style = \"color:Green;\"> 良好</td><td style = \"color:Orange;\"> 中间</td><td style = \"color:Red;\"> 不良</td><td> 合计</td>\n";
        htmlStr += "</tr><tr>\n";
        htmlStr += "<td rowspan = \"2\" style = \"border-right:1px solid gray;\">实际</td><td style = \"color:Green;\">良好</td>";
        htmlStr += QString("<td>%1 </td> <td>%2 </td> <td>%3 </td > <td>%4 </td>\n").arg(1).arg(2).arg(3).arg(4).toLocal8Bit();
        htmlStr += "</tr><tr>\n";
        htmlStr += QString("<td style = \"color:Red;\">不良</td> <td>%1 </td> <td>%2 </td> <td>%3 </td> <td>%4 </td>\n")
            .arg(100)
            .arg(200)
            .arg(300)
            .arg(400);
        htmlStr += "</tr>\n";
        htmlStr += "</table>\n";
        */
        htmlStr += "<div>\n";
        htmlStr += "<p class = \"link\" onclick = \"toggle_div(this);\">如何读取图表…</p>\n";
        htmlStr += "<div id = \"div_howto\">\n";
        htmlStr += "<span>\n";
        htmlStr += "<h4>分布</h4>\n";
        htmlStr += "显示不良和良好图像偏差的归一化分布。理想情况下，这两条曲线不应该交叉。\n";
        htmlStr += "绿线表示良好阈值，低于该阈值将被视为良好。红线表示不良阈值，高于该阈值将被视为不良。\n";
        htmlStr += "这些线条之间是灰色区域，在其中的部分将被视为中间区。\n";
        htmlStr += "</span>\n";
        htmlStr += "<span>\n";
        htmlStr += "<h4>ROC（受试者操作特征</h4>\n";
        htmlStr += "显示所有可能阈值类别的分离情况。您可以在蓝线上取点，然后读取图表左侧的相应真阳性值（系统发现缺陷的缺陷视图，理想情况下为 100 % ）。假正值率位于图表的底部（系统发现缺陷的非缺陷视图，理想情况下为 0 % ）。\n";
        htmlStr += "<b>AUC</b>\n";
        htmlStr += "AUC（曲线下面积）值汇总了该曲线，而最大值 1.00 对应完美分类器。\n";
        htmlStr += "</span>\n";
        htmlStr += "</div>\n";
        htmlStr += "</div>\n";
        htmlStr += "<div style = \"display:inline-block;\">\n";
        htmlStr += "∅ 处理时间 55.1 ± 8.2 毫秒\n";
        htmlStr += "</div>\n";
        htmlStr += "</div>\n";
        htmlStr += "<hr/>\n";
        htmlStr += "<div style = \"color:gray; font-size:0.8em; margin-bottom:0.5em; text-align:right\">\n";
        htmlStr += "（将鼠标移到标记上以获取详细信息，单击图像以隐藏 / 显示标记）\n";
        htmlStr += "</div>\n";
        htmlStr += "<h2>\n";
        htmlStr += QString( "训练集（%1 个视图）\n").arg(listPng.size());
        htmlStr += "</h2>\n";
        for (int j = 0; j < listPng.size(); j++) { //训练集
            htmlStr += "<div class = \"view\" onclick = \"toggle_show(this);\">\n";
            htmlStr += QString("<img src = \"%1\" width = \"100\" style = \"image-rendering:pixelated;\"/>\n").arg(QString(listPng.at(j)));
            //htmlStr += "<img src = \"data:image/png;base64,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\" usemap = \"#Basler_acA5472-17uc__23066837__20200318_171253710_0032.bmp|0:0\"/>\n";
           // htmlStr += "<map name = \"Basler_acA5472-17uc__23066837__20200318_171253710_0032.bmp|0:0\">\n";
           /* htmlStr += "<area shape = \"poly\" coords = \"89,117,89,118,89,118,89,120,88,120,88,121,88,121,88,124,87,124,87,125,87,125,87,126,87,127,87,127,86,127,86,128,86,128,86,129,85,130,85,130,84,131,84,132,84,132,84,133,84,134,84,134,88,134,88,133,88,133,88,132,89,132,89,131,89,131,89,130,89,130,89,127,90,127,90,126,90,125,90,123,91,122,91,122,91,121,91,118,91,118,91,117,\" title = \"区域	defect\n";
            htmlStr += "得分	0.94\n";
            htmlStr += "面积	3620.8\n";
            htmlStr += "中心	711, 1025.6\" />\n";
            htmlStr += "<area shape = \"poly\" coords = \"87,70,87,70,87,70,86,71,86,72,86,72,86,73,85,73,85,76,86,77,87,77,87,76,88,76,88,75,88,74,88,74,89,73,89,72,89,72,89,72,89,71,89,70,\" title = \"区域	defect\n";
            htmlStr += "得分	0.74\n";
            htmlStr += "面积	1294.3\n";
            htmlStr += "中心	705.5, 589\" />\n";
            htmlStr += "<area shape = \"poly\" coords = \"90,45,90,45,89,46,89,46,89,46,89,47,89,48,89,50,88,50,88,51,88,52,88,56,87,57,87,59,89,59,89,57,89,57,89,56,90,56,90,55,90,55,90,53,91,53,91,52,91,52,91,49,92,49,92,48,92,48,92,46,92,45,92,45,\" title = \"区域	defect\n";
            htmlStr += "得分	0.74\n";
            htmlStr += "面积	2265.1\n";
            htmlStr += "中心	724.6, 414.6\" />\n";*/
           // htmlStr += "</map>\n";
            htmlStr += "<br/>\n";
            //htmlStr += "<span>Basler_acA5472 - 17uc__23066837__20200318_171253710_0032.bmp | 0:0，已训练</span>\n";
            htmlStr += "</div>\n";
        }
        
        htmlStr += "<h2>\n";
        htmlStr += "测试集（67 个视图）\n";
        htmlStr += "</h2>\n";
        htmlStr += "<div class = \"view\" onclick = \"toggle_show(this);\">\n";
        htmlStr += "<img src = \"data:image/jpeg;base64,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\" width = \"320\" style = \"image-rendering:pixelated;\"/>\n";
        htmlStr += "<img src = \"data:image/png;base64,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\" usemap = \"#Basler_acA5472-17uc__23066837__20200318_171253710_0027.bmp|0:0\"/>\n";
        //htmlStr += "<map name = \"Basler_acA5472-17uc__23066837__20200318_171253710_0027.bmp|0:0\">\n";
       /* if (isDefect) {
            htmlStr += "<area shape = \"poly\" coords = \"91,47,90,48,90,48,90,53,91,52,91,51,92,51,92,50,92,50,92,48,92,48,92,47,\" title = \"区域	defect\n";
            htmlStr += "得分	0.44\n";
            htmlStr += "面积	1031\n";
            htmlStr += "中心	730, 409.7\"/>\n";
        }*/
       // htmlStr += "</map>\n";
        htmlStr += "<br/>\n";
        htmlStr += "<span>Basler_acA5472 - 17uc__23066837__20200318_171253710_0027.bmp | 0:0 </span>\n";
        htmlStr += "</div>\n";
        htmlStr += "</div>\n";
        htmlStr += "</body>\n";
        htmlStr += "</html>\n";
        return htmlStr;
}
