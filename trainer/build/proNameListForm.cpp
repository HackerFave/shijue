#include "proNameListForm.h"

proNameListForm::proNameListForm(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    this->setWindowTitle("解决方案");
    this->setAttribute(Qt::WA_ShowModal);
    this->setWindowFlags( Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint |Qt::WindowCloseButtonHint );
    _configIni = new QSettings(QString("%1/setting/proSetting.ini").arg(publicClass::instance()->getAppPath()), QSettings::IniFormat);
    connect(ui.tableViewLIstPro, SIGNAL(signalFindPro(QString &)), ui.treeViewProList, SLOT(slotOpenPro(QString &)));
    connect(ui.tableViewLIstPro, SIGNAL(signalProName(QString &)), this, SIGNAL(signalProPath(QString &)));
    connect(ui.treeViewProList, SIGNAL(signalSendFileInfo(QString &)), this, SLOT(slotFileInfo(QString &)));
    
}

proNameListForm::~proNameListForm()
{
}
void proNameListForm::setIniFile(const QString key, QVariant value)
{
    /*QFile file(QString("%1/setting/setting.ini").arg(_exePath));
    if (!file.open(QIODevice::WriteOnly)) {
        return;
    }*/


    _configIni->setValue(QString("setting/%1").arg(key), value);

}
QVariant proNameListForm::readIniFile(const QString key)
{
    if (_configIni) {
        //QSettings *configIni = new QSettings(QString("%1/setting/setting.ini").arg(_exePath), QSettings::IniFormat);
        return _configIni->value(QString("setting/%1").arg(key)).toString();
    }
    return QVariant();
}
QMap<QString, QVariant> proNameListForm::getProData()
{
    QMap<QString, QVariant> mapProData;
    QFile file;
    file.setFileName(QString("%1/setting/proSetting.ini").arg(publicClass::instance()->getAppPath()));
    if (file.open(QIODevice::ReadOnly)) {
        while (!file.atEnd())
        {
            QString dataStr = QString(file.readLine());
            QStringList listProPath = dataStr.split("=");
            if (listProPath.size() > 1) {
                QString keyName = listProPath.at(0);
                QString proPath = listProPath.at(1);
                proPath.chop(2);
                QDir dir(proPath);
                if (dir.exists())
                {
                    mapProData.insert(keyName, proPath);
                }
            }
        }
    }
    //ui.tableViewLIstPro.
    //ui.tableViewLIstPro->addItem();
    
    return mapProData;
}
void proNameListForm::on_toolButton_openPro_clicked()
{
    emit signalOpenPro();
}
void proNameListForm::showForm()
{
    ui.tableViewLIstPro->initTableView(getProData());

    this->show();
}
void proNameListForm::slotFileInfo(QString &textInfo)
{
    ui.textEdit_info->clear();
    ui.textEdit_info->append(textInfo);
}