#include "TemplateItemForm.h"
#include <qgroupbox.h>
#include "TemplateItemLabelForm.h"
#include "PublicClass.h"
#include "qtextcodec.h"
#pragma execution_character_set("utf-8")
TemplateItemForm::TemplateItemForm(int index, QWidget *parent)
    : QWidget(parent)
    ,index_(index)
{
    ui.setupUi(this);
    ui.scrollArea_img->viewport()->installEventFilter(this);//QSCrollArea 滚动区域是viewport()
    SetCurrentFormObject(index_);
    P_templateIni_ = new QSettings(QString("%1/template/TemplateManager.ini").arg(QCoreApplication::applicationDirPath())
        , QSettings::IniFormat);
	P_templateIni_->setIniCodec(QTextCodec::codecForName("utf-8"));;//添上这句就不会出现乱码了
    InitItemLabel();
    P_TemplateRegisterForm = new TemplateRegisterForm(false,"","注册窗口");//注册界面
    connect(P_TemplateRegisterForm, SIGNAL(signalRefreshList()), this, SLOT(slotRefreshList()));
}

TemplateItemForm::~TemplateItemForm()
{
}
void TemplateItemForm::InitItemLabel()
{
    QString currentPath = QDir::currentPath().simplified();
    QString key = this->objectName().simplified();
    QString tempPath = ReadIni(key);
    QSettings * ini = new QSettings(currentPath+QString("%1TempInfo.ini").arg(tempPath)
        , QSettings::IniFormat);
     QStringList listkey = ini->childGroups();
     int count = listkey.size();
    for (int i = 0; i < count; i++) {
        QString keyini = listkey.at(i);
        TemplateItemLabelForm *form = new TemplateItemLabelForm(listkey.at(i), this->objectName());//模板展示界面
		if (ini->value(QString("%1/checkedstatus").arg(keyini)).toBool()) {
			form->setEnabledOthers(false);
		}
        //form->setObjectName(QString::number(i));//属性名称
        QGridLayout *layout = new QGridLayout;
        QGroupBox *group = new QGroupBox;
        //刷新编辑后的数据
        connect(form, SIGNAL(signalRefreshListItemLabel()), this, SLOT(slotRefreshList()));
		form->SetCheckedLoadStatus(ini->value(QString("%1/checkedstatus").arg(keyini)).toBool());//是否加载中
        QString templatePath = ini->value(QString("%1/templatePath").arg(keyini)).toString();
        int index = templatePath.lastIndexOf("/"); //从后面查找"/"位置
        QString title = templatePath.right(templatePath.length() - index - 1); //从右边截取
        group->setTitle(title);//group 标题
        form->SetCurrentTemplateName(ini->value(QString("%1/templateName").arg(keyini)).toString());//名称
        form->SetCurrentTemplatePath(templatePath);//当前模板路径
        form->SetCurrentTemplateDemoFilePath(ini->value(QString("%1/templateDemoFilePath").arg(keyini)).toString());//程序文件路径
        QString str1 = ini->value(QString("%1/templateAuthor").arg(keyini)).toString();//作者
        form->set_label_temname(ini->value(QString("%1/templateName").arg(keyini)).toString() +"(创建人:"+str1+")");
        form->SetCurrentTemplateAuthor(str1);//创建人
        QString str2 =  ini->value(QString("%1/templateOper").arg(keyini)).toString().toLocal8Bit();//描述
        form->set_textEdit_info(str2);
        QString str3 = ini->value(QString("%1/templateCreateTime").arg(keyini)).toString();//创建时间
        form->set_label_createtime(str3);
        QString str4 = ini->value(QString("%1/templateChangeTime").arg(keyini)).toString();//修改时间
        form->set_label_changetime(str4);
        QString str5 = ini->value(QString("%1/templateFunctionDescPath").arg(keyini)).toString();//功能说明文档路径
        QString str6 = ini->value(QString("%1/templatePreviewImgPath").arg(keyini)).toString();//预览图路径
        form->SetCurrentDocPath(str5);//文档路径
        form->SetCurrentImgPath(str6);//预览图路径
        form->SetCurrentImgShow(str6);//显示预览图
        layout->addWidget(form);
        group->setLayout(layout);
        ui.verticalLayout_addlabel->addWidget(group);
        ui.verticalLayout_addlabel->addStretch();
    }
}
void TemplateItemForm::on_toolButton_addTemp_clicked()
{
    P_TemplateRegisterForm->SetCurrentTemIniKey(this->objectName());
    P_TemplateRegisterForm->show();   
}
void TemplateItemForm::SetCurrentFormObject(int index)
{
    switch (index)
    {
    case 0: {
        this->setObjectName("algtempath");
    }
        break;
    case 1: {
        this->setObjectName("historicaldatatempath");
    }
            break;
    case 2: {
        this->setObjectName("defectfiltertempath");
    }
            break;
    case 3: {
        this->setObjectName("runformtempath");
    }
            break;
    case 4: {
        this->setObjectName("plctempath");
    }
            break;

    default:
        break;
    }
}
QString TemplateItemForm::ReadIni(const QString &key)
{
    return P_templateIni_->value(QString("pluginpath/%1").arg(key)).toString();
}
//添加完刷新列表
void TemplateItemForm::slotRefreshList()
{
    QLayoutItem *child;
    while ((child = ui.verticalLayout_addlabel->takeAt(0)) != 0)
    {
        //setParent为NULL，防止删除之后界面不消失
        if (child->widget())
        {
            child->widget()->setParent(NULL);
        }

        delete child;
    }
    InitItemLabel();
}
void TemplateItemForm::on_toolButton_updateform_clicked()
{
    slotRefreshList();
}
bool TemplateItemForm::eventFilter(QObject *target, QEvent *event)
{
    if (target == ui.scrollArea_img->viewport() && PublicClass::instance()->GetKeyStatus())
    {
        if (event->type() == QEvent::Wheel)
        {
            event->ignore();
            return true;      //只针对scrollArea窗口禁用滚轮滚动功能，返回true表示过滤
        }
    }
    else
    {
        event->accept();
        return false;
    }
    return false;			//返回false表示不过滤
}