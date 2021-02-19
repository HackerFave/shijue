#pragma once

#include <QWidget>
#include <QStandardItemModel>
#include <QDebug>
#include <QMessageBox>
#include <QFileSystemModel>
#include <QAbstractItemView>
#include <qsettings.h>
#include "publicClass.h"
#include "ui_proTreeview.h"

class proTreeview : public QWidget
{
    Q_OBJECT

public:
    proTreeview(QWidget *parent = Q_NULLPTR);
    ~proTreeview();
    void initTreeView(const QString &proPath);
    void setIniFile(const QString key, QVariant value);//д��ini
    QVariant readIniFile(const QString key);//ini�ļ�
private slots:
    void on_treeView_clicked(const QModelIndex &index); 
    void slotOpenPro(QString &);
signals:
    void signalSendFileInfo(QString &);
private:
    Ui::proTreeview ui;
    QStandardItemModel *m_pModel;
    QFileSystemModel *m_pFileModel;
    QSettings *_configIni = Q_NULLPTR;//ini�ļ�
};
