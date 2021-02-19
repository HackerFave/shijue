#include "sysSettingForm.h"
#include "publicClass.h"
#include <QHBoxLayout>

sysSettingForm::sysSettingForm(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    this->setAttribute(Qt::WA_ShowModal);
    _publicClass = publicClass::instance();
    this->setWindowIcon(QIcon(tr("%1/icons/system/setting.png").arg(_publicClass->getAppPath())));
    ui.line_width->setFixedWidth(ui.spinBox_penSize->value());
    connect(ui.spinBox_penSize, SIGNAL(valueChanged(int)), this, SLOT(slotValueChangePen(int)));
    connect(ui.checkBox_selectAll, SIGNAL(stateChanged(int)), this, SLOT(onStateChanged(int)));
    QMap<int, QComboBox*> map;
    map.insert(0, ui.comboBox_exportMode);
    initComboBoxStyle(map);
    initColor();
}

sysSettingForm::~sysSettingForm()
{
}
void sysSettingForm::initColor()
{
    _mapColorDefault.insert(0, QColor(128, 0, 0));
    _mapColorDefault.insert(1, QColor(0, 128, 0));
    _mapColorDefault.insert(2, QColor(128, 128, 0));
    _mapColorDefault.insert(3, QColor(0, 0, 128));
    _mapColorDefault.insert(4, QColor(128, 0, 128));
    _mapColorDefault.insert(5, QColor(0, 128, 128));
    _mapColorDefault.insert(6, QColor(128, 128, 128));
    _mapColorDefault.insert(7, QColor(64, 0, 0));
    _mapColorDefault.insert(8, QColor(192, 0, 0));
    _mapColorDefault.insert(9, QColor(64, 128, 0));
    _mapColorDefault.insert(10, QColor(192, 128, 0));
    _mapColorDefault.insert(11, QColor(64, 0, 128));
    _mapColorDefault.insert(12, QColor(192, 0, 128));
    _mapColorDefault.insert(13, QColor(64, 128, 128));
    _mapColorDefault.insert(14, QColor(192, 128, 128));
    _mapColorDefault.insert(15, QColor(0, 64, 0));
    _mapColorDefault.insert(16, QColor(128, 64, 0));
    _mapColorDefault.insert(17, QColor(0, 192, 0));
    _mapColorDefault.insert(18, QColor(128, 192, 0));
    _mapColorDefault.insert(19, QColor(0, 64, 128));

}
void sysSettingForm::closeEvent(QCloseEvent *event)
{
    getInitStatus();
    ui.spinBox_penSize->setValue(_PenSize);
    ui.switchBtn_fill->setChecked(_switchBtn_fillChecked);
    ui.switchBtn_inproportion->setChecked(_switchBtn_inproportionChecked);
    ui.widget_test->setChecked(_widget_testChecked);
    ui.switchBtn_changeImageName->setChecked(_switchBtn_changeImageNameChecked);
    ui.widget_lodeFileMode->setChecked(_switchBtnFileLoadMode);
    ui.comboBox_exportMode->setCurrentText(_fileExportMode);
    //ui.comboBox_loadMode.setcu
    
    this->setVisible(false);
    emit signalFormChecked(false);
}
void sysSettingForm::show()
{
    getInitStatus();
    ui.spinBox_penSize->setValue(_PenSize);
    ui.switchBtn_fill->setChecked(_switchBtn_fillChecked);
    ui.switchBtn_inproportion->setChecked(_switchBtn_inproportionChecked);
    ui.widget_test->setChecked(_widget_testChecked);
    ui.switchBtn_changeImageName->setChecked(_switchBtn_changeImageNameChecked);
    ui.widget_lodeFileMode->setChecked(_switchBtnFileLoadMode);
    ui.comboBox_exportMode->setCurrentText(_fileExportMode);
    this->setVisible(true);
    emit signalFormChecked(true);
}
void sysSettingForm::slotValueChangePen(int value)
{
    ui.line_width->setFixedWidth(value);
   
}
void sysSettingForm::onStateChanged(int checked)
{
    _checked = checked;
    QMap<int, moreTypeForm*>::iterator it;
    for (it = _mapMoreTypeForm.begin(); it != _mapMoreTypeForm.end(); ++it) {
        it.value()->setChecked(_checked);
    }
}
void sysSettingForm::slotCheckedChange(bool checked)
{
    
}
//确定
void sysSettingForm::on_pushButton_ok_clicked()
{
    setSysIni();
    emit signalSwitchChacked(1, ui.switchBtn_fill->getChecked());//标注显示是否填充
    emit signalSwitchChacked(2, ui.switchBtn_inproportion->getChecked());//是否保持按照原比例
    emit signalSwitchChacked(3, ui.widget_test->getChecked());//是否保存测试结果
    emit signalSwitchChacked(4, ui.switchBtn_changeImageName->getChecked());//增加图片时是否修改图片名
    emit signalValuePen(ui.spinBox_penSize->value());//画笔
    emit signalLoadMode(ui.widget_lodeFileMode->getChecked());//是否启用文件加载模式:
    emit signalFileExportMode(ui.comboBox_exportMode->currentText());//文件导出格式
    QList<QVariantList> listMoreType;
    if (!_mapMoreTypeForm.isEmpty()) {
        
        QMap<int, moreTypeForm*>::iterator it;
        for (it = _mapMoreTypeForm.begin(); it != _mapMoreTypeForm.end(); ++it) {
            QVariantList list;
            list << it.value()->getCheckeBoxStr() << it.value()->getLabelColor()<<it.value()->getTypeText();
            listMoreType.append(list);
        }          
    }
    emit signalMoreTypeData(listMoreType);//多类别
    this->setVisible(false);
    emit signalFormChecked(false);
}
//取消
void sysSettingForm::on_pushButton_cancel_clicked() 
{
    getInitStatus();
    ui.spinBox_penSize->setValue(_PenSize);
    ui.switchBtn_fill->setChecked(_switchBtn_fillChecked);
    ui.switchBtn_inproportion->setChecked(_switchBtn_inproportionChecked);
    ui.widget_test->setChecked(_widget_testChecked);
    ui.switchBtn_changeImageName->setChecked(_switchBtn_changeImageNameChecked);
    ui.comboBox_exportMode->setCurrentText(_fileExportMode);
    ui.widget_lodeFileMode->setChecked(_switchBtnFileLoadMode);
    this->setVisible(false);
    emit signalFormChecked(false);

}
void sysSettingForm::getInitStatus()
{
     initMoreTypeMap();
     _switchBtn_fillChecked = _publicClass->readIniFile("switchBtnFill").toBool(); //ui.switchBtn_fill->getChecked();
     _switchBtn_inproportionChecked = _publicClass->readIniFile("switchBtnInproportion").toBool(); //ui.switchBtn_inproportion->getChecked();
     _widget_testChecked = _publicClass->readIniFile("switchWidgeTest").toBool(); //ui.widget_test->getChecked();
     _switchBtn_changeImageNameChecked = _publicClass->readIniFile("switchBtnChangeImageName").toBool(); //ui.switchBtn_changeImageName->getChecked();
     _switchBtnFileLoadMode = _publicClass->readIniFile("switchBtnFileLoadMode").toBool();
     _fileExportMode = _publicClass->readIniFile("fileExportMode").toString();//文件导出格式
     _PenSize = _publicClass->readIniFile("spinBoxPenSize").toInt();// ui.spinBox_penSize->value();
     
}
void sysSettingForm::setSysIni()
{
    _publicClass->removeIniValue();
    _publicClass->setIniFile("switchBtnFill", ui.switchBtn_fill->getChecked());
    _publicClass->setIniFile("switchBtnInproportion", ui.switchBtn_inproportion->getChecked());
    _publicClass->setIniFile("switchWidgeTest", ui.widget_test->getChecked());
    _publicClass->setIniFile("switchBtnChangeImageName", ui.switchBtn_changeImageName->getChecked());
    _publicClass->setIniFile("switchBtnFileLoadMode", ui.widget_lodeFileMode->getChecked());
    _publicClass->setIniFile("fileExportMode", ui.comboBox_exportMode->currentText());//文件导出格式
    _publicClass->setIniFile("moreTypeNum", _mapMoreTypeForm.size());//多类别数量  
    if (!_mapMoreTypeForm.isEmpty()) {
        QMap<int, moreTypeForm*>::iterator it;
        for (it = _mapMoreTypeForm.begin(); it != _mapMoreTypeForm.end(); it++)
        {
            _publicClass->setIniFile(tr("%1").arg(it.key()), tr("%1:%2,%3,%4").arg(it.value()->getTypeText()).arg(it.value()->getLabelColor().red())
            .arg(it.value()->getLabelColor().green()).arg(it.value()->getLabelColor().blue()));
        }
       
    }
    _publicClass->setIniFile("spinBoxPenSize", ui.spinBox_penSize->value());
}
void sysSettingForm::initMoreTypeMap()
{

    for (int i = 0; i < ui.verticalLayout_addType->count(); ++i)
    {
        QLayoutItem *layoutItem = ui.verticalLayout_addType->itemAt(i);

        if (layoutItem->spacerItem())
        {
            ui.verticalLayout_addType->removeItem(layoutItem);
            --i;
        }
    }
    ui.verticalLayout_addType->setContentsMargins(0, 0, 0, 0);
    QMap<int, moreTypeForm*>::iterator it;
    for (it = _mapMoreTypeForm.begin(); it != _mapMoreTypeForm.end(); ++it) {
        
        it.value()->close();
      
    }
    _mapMoreTypeForm.clear();
    int moreTypeNum = _publicClass->readIniFile("moreTypeNum").toUInt();
    for (int i = 1; i < moreTypeNum+1; i++)
    {
        moreTypeForm *form = new moreTypeForm;
        //int key = _mapMoreTypeForm.size();
        form->setCheckBoxStr(QString("%1").arg(QString::number(i)));//配置文件key
        form->setLineEditTypeStr(_publicClass->readIniFile(QString::number(i)).toString().split(":").at(0));
        QString color = _publicClass->readIniFile(QString::number(i)).toString().split(":").at(1);
        qreal r = color.split(",").at(0).toInt();
        qreal g = color.split(",").at(1).toInt();
        qreal b = color.split(",").at(2).toInt();
        form->setLabelColor(QColor(r,g,b));
        //form->setChecked(_checked);
        _mapMoreTypeForm.insert(i, form);
        ui.verticalLayout_addType->addWidget(form);           
    }
    ui.verticalLayout_addType->addStretch();
    ui.label_typeNum->setText(QString::number(_mapMoreTypeForm.size()));
}
void sysSettingForm::on_toolButton_addMoreType_clicked()
{
    for (int i = 0; i < ui.verticalLayout_addType->count(); ++i)
    {
        QLayoutItem *layoutItem = ui.verticalLayout_addType->itemAt(i);

        if (layoutItem->spacerItem())
        {
            ui.verticalLayout_addType->removeItem(layoutItem);
            --i;
        }
    }
    ui.verticalLayout_addType->setContentsMargins(0,0,0,0);
    if (_mapMoreTypeForm.size()>TYPE_NUM-1)
    {
        return;
    }
    moreTypeForm *form = new moreTypeForm;
    int key = _mapMoreTypeForm.size();
    form->setCheckBoxStr(QString("%1").arg(key+1));
    form->setLineEditTypeStr(QString("class_%1").arg(key+1));
    form->setLabelColor(_mapColorDefault.value(key+1));
    //form->setChecked(_checked);
    _mapMoreTypeForm.insert(key+1, form);
    ui.verticalLayout_addType->addWidget(form);
    ui.verticalLayout_addType->addStretch();
    ui.label_typeNum->setText(QString::number(_mapMoreTypeForm.size()));
}
void sysSettingForm::on_toolButton_del_clicked()
{
   
    if (_mapMoreTypeForm.size() == 1) {
        _mapMoreTypeForm.value(1)->setChecked(false);
        ui.checkBox_selectAll->setChecked(false);
        return;
    }
    QMap<int, moreTypeForm*> map;
    int i = 1;
    QMap<int, moreTypeForm*>::iterator it;
        for (it = _mapMoreTypeForm.begin(); it != _mapMoreTypeForm.end(); ++it) {
            if (it.value()->getChecked()) {
                it.value()->setVisible(false);
            }
            else
            {
                map.insert(i++, it.value());
            }
        }
        _mapMoreTypeForm.clear();
        _mapMoreTypeForm = map;
        for (int i = 0; i < ui.verticalLayout_addType->count(); ++i)
        {
            QLayoutItem *layoutItem = ui.verticalLayout_addType->itemAt(i);

            if (layoutItem->spacerItem())
            {
                ui.verticalLayout_addType->removeItem(layoutItem);
                --i;
            }
        }
        QMap<int, moreTypeForm*>::iterator it_;
        for (it_ = _mapMoreTypeForm.begin(); it_ != _mapMoreTypeForm.end(); ++it_) {
            
            ui.verticalLayout_addType->addWidget(it_.value());
            it_.value()->setCheckBoxStr(QString("%1").arg(it_.key()));
            qApp->processEvents();
            QString str = it_.value()->getTypeText();
            if (str.contains("class_")) {
                static_cast<moreTypeForm*>(it_.value())->setLineEditTypeStr(QString("class_%1").arg(it_.key()));
            }           
            it_.value()->setLabelColor(_mapColorDefault.value(it_.key()));
        }      
        ui.verticalLayout_addType->addStretch();
        ui.label_typeNum->setText(QString::number(_mapMoreTypeForm.size()));
        ui.checkBox_selectAll->setChecked(false);
        if (_mapMoreTypeForm.size() <= 0) {
            on_toolButton_addMoreType_clicked();
        }
}
void sysSettingForm::initComboBoxStyle(QMap<int, QComboBox *> comBoxMap)
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

