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
    ui.scrollArea_img->viewport()->installEventFilter(this);//QSCrollArea ����������viewport()
    SetCurrentFormObject(index_);
    P_templateIni_ = new QSettings(QString("%1/template/TemplateManager.ini").arg(QCoreApplication::applicationDirPath())
        , QSettings::IniFormat);
	P_templateIni_->setIniCodec(QTextCodec::codecForName("utf-8"));;//�������Ͳ������������
    InitItemLabel();
    P_TemplateRegisterForm = new TemplateRegisterForm(false,"","ע�ᴰ��");//ע�����
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
        TemplateItemLabelForm *form = new TemplateItemLabelForm(listkey.at(i), this->objectName());//ģ��չʾ����
		if (ini->value(QString("%1/checkedstatus").arg(keyini)).toBool()) {
			form->setEnabledOthers(false);
		}
        //form->setObjectName(QString::number(i));//��������
        QGridLayout *layout = new QGridLayout;
        QGroupBox *group = new QGroupBox;
        //ˢ�±༭�������
        connect(form, SIGNAL(signalRefreshListItemLabel()), this, SLOT(slotRefreshList()));
		form->SetCheckedLoadStatus(ini->value(QString("%1/checkedstatus").arg(keyini)).toBool());//�Ƿ������
        QString templatePath = ini->value(QString("%1/templatePath").arg(keyini)).toString();
        int index = templatePath.lastIndexOf("/"); //�Ӻ������"/"λ��
        QString title = templatePath.right(templatePath.length() - index - 1); //���ұ߽�ȡ
        group->setTitle(title);//group ����
        form->SetCurrentTemplateName(ini->value(QString("%1/templateName").arg(keyini)).toString());//����
        form->SetCurrentTemplatePath(templatePath);//��ǰģ��·��
        form->SetCurrentTemplateDemoFilePath(ini->value(QString("%1/templateDemoFilePath").arg(keyini)).toString());//�����ļ�·��
        QString str1 = ini->value(QString("%1/templateAuthor").arg(keyini)).toString();//����
        form->set_label_temname(ini->value(QString("%1/templateName").arg(keyini)).toString() +"(������:"+str1+")");
        form->SetCurrentTemplateAuthor(str1);//������
        QString str2 =  ini->value(QString("%1/templateOper").arg(keyini)).toString().toLocal8Bit();//����
        form->set_textEdit_info(str2);
        QString str3 = ini->value(QString("%1/templateCreateTime").arg(keyini)).toString();//����ʱ��
        form->set_label_createtime(str3);
        QString str4 = ini->value(QString("%1/templateChangeTime").arg(keyini)).toString();//�޸�ʱ��
        form->set_label_changetime(str4);
        QString str5 = ini->value(QString("%1/templateFunctionDescPath").arg(keyini)).toString();//����˵���ĵ�·��
        QString str6 = ini->value(QString("%1/templatePreviewImgPath").arg(keyini)).toString();//Ԥ��ͼ·��
        form->SetCurrentDocPath(str5);//�ĵ�·��
        form->SetCurrentImgPath(str6);//Ԥ��ͼ·��
        form->SetCurrentImgShow(str6);//��ʾԤ��ͼ
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
//�����ˢ���б�
void TemplateItemForm::slotRefreshList()
{
    QLayoutItem *child;
    while ((child = ui.verticalLayout_addlabel->takeAt(0)) != 0)
    {
        //setParentΪNULL����ֹɾ��֮����治��ʧ
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
            return true;      //ֻ���scrollArea���ڽ��ù��ֹ������ܣ�����true��ʾ����
        }
    }
    else
    {
        event->accept();
        return false;
    }
    return false;			//����false��ʾ������
}