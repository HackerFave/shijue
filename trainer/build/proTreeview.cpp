#include "proTreeview.h"

proTreeview::proTreeview(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    m_pFileModel = new QFileSystemModel(this);
    setWindowTitle(QStringLiteral("QTreeView和QFileSystemModel的使用"));
    //_configIni = new QSettings(QString("%1/setting/proSetting.ini").arg(publicClass::instance()->getAppPath()), QSettings::IniFormat);
    
}

proTreeview::~proTreeview()
{
}
void proTreeview::on_treeView_clicked(const QModelIndex &index)
{
    QString sData;
    QString sDir;
    QString sfilePath;
    QString stype;
    QString sfileName;
    quint64 fileSize;
    QString sFromatStr = tr("data:%1 \nisDir:%2 \nfilePath:%3 \ntype:%4 \nfileName:%5 \nfileSize:%6 字节");
    sData = index.data().toString();
    sDir = QString::number(m_pFileModel->isDir(index));
    sfilePath = m_pFileModel->filePath(index);
    stype = m_pFileModel->type(index);
    sfileName = m_pFileModel->fileName(index);
    fileSize = m_pFileModel->size(index);
    emit signalSendFileInfo(sFromatStr.arg(sData).arg(sDir).arg(sfilePath).arg(stype).arg(sfileName).arg(fileSize));
}
void proTreeview::initTreeView(const QString &proPath)
{
    
    // 隐藏树的头显示用
    ui.treeView->header()->hide();
    // 设置为不可编辑
    ui.treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // 建立一个文件系统模型
    //m_pFileModel = new QFileSystemModel(this);
    m_pFileModel->setRootPath(proPath);
    QStringList sNameFilter;
    sNameFilter << "*";
    m_pFileModel->setNameFilterDisables(false);
    m_pFileModel->setNameFilters(sNameFilter);

    ui.treeView->setModel(m_pFileModel);
    ui.treeView->setRootIndex(m_pFileModel->index(proPath));
    // 隐藏后面三列
    ui.treeView->setColumnHidden(1, true);
    ui.treeView->setColumnHidden(2, true);
    ui.treeView->setColumnHidden(3, true);
    // ui->treeView->setIndexWidget(new QPushButton);
}
void proTreeview::setIniFile(const QString key, QVariant value)
{
    /*QFile file(QString("%1/setting/setting.ini").arg(_exePath));
    if (!file.open(QIODevice::WriteOnly)) {
        return;
    }*/
    

    _configIni->setValue(QString("setting/%1").arg(key), value);

}
QVariant proTreeview::readIniFile(const QString key)
{
    if (_configIni) {
        //QSettings *configIni = new QSettings(QString("%1/setting/setting.ini").arg(_exePath), QSettings::IniFormat);
        return _configIni->value(QString("setting/%1").arg(key)).toString();
    }
    return QVariant();
}
void proTreeview::slotOpenPro(QString & proPath)
{
    initTreeView(proPath);
}