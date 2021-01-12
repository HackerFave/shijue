#include "TemplateItemLabelForm.h"
#include <QDesktopServices>
#include "mrun_messagebox.h"
#include "PublicClass.h"
#include "zipper.h"
#include "unzipper.h"
#include <fstream>
#include <QTextCodec>
#pragma execution_character_set("utf-8")
TemplateItemLabelForm::TemplateItemLabelForm(const QString &indexStr, const QString & objectname , QWidget *parent)
    : QWidget(parent)
    ,ui(new Ui::TemplateItemLabelForm)
    ,indexStr_(indexStr)
    ,objectName_(objectname)
{
    ui->setupUi(this);
    P_TemplateRegisterForm = new TemplateRegisterForm(true, indexStr_, "�༭����");
    P_templateIni_ = new QSettings(QDir::currentPath() + QString("/template/TemplateManager.ini")
        , QSettings::IniFormat);//��һ��ini

    P_TempInfoIni_ = new QSettings(QDir::currentPath()+QString("%1TempInfo.ini").arg(
        P_templateIni_->value(QString("pluginpath/%1").arg(objectName_)).toString())
        , QSettings::IniFormat);//�ڶ���ini

	pluginTempIni_ = new QSettings(QDir::currentPath() + QString("/config/plugins/plugin.ini")
		, QSettings::IniFormat);//������ص������ļ�
    connect(P_TemplateRegisterForm, SIGNAL(signalRefreshList()), this, SLOT(slotRefreshListItemLabel()));
	connect(P_TemplateRegisterForm, SIGNAL(signalRefreshPlgunIni(const QString &)), this, SLOT(slotRefreshPlgunIni(const QString &)));
	
}

TemplateItemLabelForm::~TemplateItemLabelForm()
{
}
void TemplateItemLabelForm::set_label_temname(const QString &str)//ģ������
{
    ui->label_temname->setText(str);
}
void TemplateItemLabelForm::set_textEdit_info(const QString &str)//����
{
    currentTemplateOper_ = str;
    ui->textEdit_info->setText(str);
}
void TemplateItemLabelForm::set_label_createtime(const QString &str)//����ʱ��
{
    ui->label_createtime->setText(str);
}
void TemplateItemLabelForm::set_label_changetime(const QString &str)//�޸�ʱ��
{
    ui->label_changetime->setText(str);
}
void TemplateItemLabelForm::SetCurrentImgShow(const QString &imgpath)//����Ԥ������
{
    /*QImage* img = new QImage;
    img->load(imgpath);
    QImage scaledimg;
    scaledimg = img->scaled(ui->label_image->width(), ui->label_image->height(), Qt::KeepAspectRatio);
    ui->label_image->setPixmap(QPixmap::fromImage(scaledimg));*/
    ui->widget_image->setLoadImage(QDir::currentPath()+imgpath);
}
void TemplateItemLabelForm::on_toolButton_doc_clicked()//��˵���ĵ�
{
    if (!QDesktopServices::openUrl(QUrl::fromLocalFile(QDir::currentPath()+ currentDocPath_))) {
        setmessage(HINT_FLAG::MRUN_ERROR, "�ĵ���ʧ��");
    }
}
//�������
void TemplateItemLabelForm::on_toolButton_export_clicked()
{
    QString topath = QFileDialog::getExistingDirectory(this, tr("��ѡ��Ҫ����Ŀ¼"), "./");
    if (topath.isEmpty())
    {
       // setmessage(HINT_FLAG::MRUN_HINT, "����ģ��ʧ��");
        return;
    }
    QString zippath = QDir::currentPath() + currentTemplatePath_;//����Ŀ¼+���·�� ѹ���ļ�����·��
    const std::string expoerpath = std::string((const char *)zippath.toLocal8Bit().simplified());
    const std::string zip = expoerpath + ".zip";//ѹ���ļ�����Ӻ�׺���������ɱ���
    //zipper::Unzipper unzipSolution(path);
    zipper::Zipper zippper(zip);//�½�����zip������Ҫ���ļ�����ѹ����
    QFileInfoList fileinfoList = GetFileList(QString::fromLocal8Bit(expoerpath.data()));//��ȡҪѹ�����ļ��㼶�б�
    
     for (int i = 0; i < fileinfoList.size(); i++)
    {
        QString filepath = fileinfoList.at(i).filePath();//�ļ�·��
        std::string strFilePath = std::string(std::string((const char *)filepath.toLocal8Bit()));//תstd����string
       
        QString strRelative = filepath.replace(zippath, "").simplified();//��ȡ��õ�ǰ�����ļ���Ŀ¼
        std::string strRelativePath = std::string(std::string((const char *)strRelative.toLocal8Bit()));//���ѹ����·��
        std::ifstream readFile(strFilePath, std::ios::binary);//��ȡ�ļ�����
        zippper.add(readFile, strRelativePath);//��ȡ������д�����·��
        readFile.close();
    }
     zippper.close();//ѹ���������������
    if (copyFileToPath(QString::fromLocal8Bit(zip.data()), topath, true)) {
        QFileInfo info;
        info.setFile(QString::fromLocal8Bit(zip.data()));
        zipper::Unzipper unzip(std::string((const char *)topath.toLocal8Bit().simplified())+"/"+
            std::string((const char *)info.fileName().toLocal8Bit().simplified()));
        QDir dirtopath;
        dirtopath.setPath(topath +"/"+ info.baseName());
        QString STR = topath + "/" + info.baseName();
        dirtopath.removeRecursively();//ɾ���ļ���
        if (!dirtopath.exists()) {
            dirtopath.mkdir(topath + "/" + info.baseName());
        }
        if (!unzip.extract(std::string((const char *)(topath + "/" + info.baseName()).toLocal8Bit().simplified())))
        {
            unzip.close();
        }
        
        unzip.close();
        QFile file(QString::fromLocal8Bit(zip.data()));
        if (!file.remove())
        {
            file.close();
            return;
        }
        file.setFileName(topath.simplified() + "/" + info.fileName().simplified());
        if (!file.remove())
        {
            file.close();
            return;
        }
        file.close();
        setmessage(HINT_FLAG::MRUN_HINT, "����ģ��ɹ�");
        return;
    }
    //setmessage(HINT_FLAG::MRUN_HINT, "����ģ��ʧ��");
}
void TemplateItemLabelForm::on_toolButton_edit_clicked()//�༭
{
    QString currentPath = QDir::currentPath();
    P_TemplateRegisterForm->set_lineEdit_name(currentTemplateName_);//ģ������
    P_TemplateRegisterForm->set_textEdit_oper(currentTemplateOper_);//ģ������
    P_TemplateRegisterForm->set_lineEdit_demofile(currentPath + currentTemplateDemoFilePath_);//��ǰģ������ļ�
    P_TemplateRegisterForm->set_lineEdit_functionDesc(currentPath + currentDocPath_);//�ĵ�˵��
    P_TemplateRegisterForm->set_lineEdit_previewimg(currentPath + currentImgPath_);//Ԥ��ͼ
    P_TemplateRegisterForm->set_lineEdit_author(currentTemplateAuthor_);//����
    P_TemplateRegisterForm->SetCurrentTemIniKey(objectName_);//��ǰtabҳ����
    P_TemplateRegisterForm->SetCurrentTemplatePath(currentPath + currentTemplatePath_);//��ǰģ��·��
    P_TemplateRegisterForm->show();
}
void TemplateItemLabelForm::on_toolButton_del_clicked()//ɾ��
{ 
    if (messageopertips("ȷ��ɾ����ģ��?")) {
        QString del_file = QString(QDir::currentPath()+ currentTemplatePath_);//ɾ����ǰģ��
        QDir oldDir;
        oldDir.setPath(del_file);
        oldDir.removeRecursively();
        P_TempInfoIni_->remove(QString("/%1").arg(indexStr_));
		QStringList iniKeyList = pluginTempIni_->childGroups();//ɾ����������
		if (iniKeyList.contains(indexStr_)) {
			pluginTempIni_->remove(QString("/%1").arg(indexStr_));
		}
        emit signalRefreshListItemLabel();
    }
    
}
//�����ļ��У�
bool TemplateItemLabelForm::copyDirectoryFiles(const QString & fromDir, const QString & toDir, bool coverFileIfExist)
{
    QDir sourceDir(fromDir);
    int index = fromDir.lastIndexOf("/"); //�Ӻ������"/"λ��
    QString dirname = fromDir.right(fromDir.length() - index - 1); //���ұ߽�ȡ
    QString newToDir = toDir + "/" + dirname;
    QDir targetDir(newToDir);
    if (!targetDir.exists()) {    /**< ���Ŀ��Ŀ¼�����ڣ�����д��� */
        if (!targetDir.mkdir(targetDir.absolutePath()))
            return false;
    }
    QFileInfoList fileInfoList = sourceDir.entryInfoList();
    foreach(QFileInfo fileInfo, fileInfoList) {
        if (fileInfo.fileName() == "." || fileInfo.fileName() == "..")
            continue;
        if (fileInfo.isDir()) {    /**< ��ΪĿ¼ʱ���ݹ�Ľ���copy */
            if (!copyDirectoryFiles(fileInfo.filePath(),
                targetDir.filePath(fileInfo.fileName()),
                coverFileIfExist))
                return false;
        }
        else {            /**< �������ǲ���ʱ�������ļ�����ɾ������ */
            if (coverFileIfExist && targetDir.exists(fileInfo.fileName())) {
                targetDir.remove(fileInfo.fileName());
            }
            /// �����ļ�copy
            if (!QFile::copy(fileInfo.filePath(),
                targetDir.filePath(fileInfo.fileName()))) {
                return false;

            }

        }

    }

    return true;
}
//�����ļ���
bool TemplateItemLabelForm::copyFileToPath(QString sourceDir, QString toDir, bool coverFileIfExist)
{
    toDir.replace("\\", "/");
    if (sourceDir == toDir) {

        return true;

    }

    if (!QFile::exists(sourceDir)) {

        return false;

    }

    QDir *createfile = new QDir;

    QFileInfo fileInfo(sourceDir);
    QString filename = fileInfo.fileName();
    toDir = toDir + "/" + filename;

    bool exist = createfile->exists(toDir);

    if (exist) {

        if (coverFileIfExist) {
            createfile->remove(toDir);
        }

    }
    if (!QFile::copy(sourceDir, toDir))

    {

        return false;

    }

    return true;

}
////ˢ�±༭�������
void TemplateItemLabelForm::slotRefreshListItemLabel()
{
    emit signalRefreshListItemLabel();
}
void TemplateItemLabelForm::wheelEvent(QWheelEvent *event)
{

}
//�ݹ�����ļ���
QFileInfoList TemplateItemLabelForm::GetFileList(QString path)
{
    QDir dir(path);

    //�г�dir(path)Ŀ¼�ļ��������ļ���Ŀ¼��Ϣ���洢��file_list����
    QFileInfoList file_list = dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    //�г�dir(path)Ŀ¼���������ļ���
    QFileInfoList folder_list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    //�������ļ���folder_list�ݹ�����������ݴ���file_list����
    for (int i = 0; i != folder_list.size(); i++)
    {
        QString name = folder_list.at(i).absoluteFilePath();
        QFileInfoList child_file_list = GetFileList(name);
        file_list.append(child_file_list);
    }
    return file_list;
}
//���ز��
void TemplateItemLabelForm::on_checkBox_load_clicked(bool checked)
{
	if (checked) {
		if (!messageopertips("ȷ�ϼ��ز�������в�����ؽ�����������Ч")) {
			ui->checkBox_load->setChecked(!checked);
			return;
		}
	}
	else
	{
		if (!messageopertips("ȷ��ȡ�����ز�������в��ȡ��������������Ч")) {
			ui->checkBox_load->setChecked(!checked);
			return;
		}
	}
	
    int index = currentTemplateDemoFilePath_.lastIndexOf("/"); //�Ӻ������"/"λ��
	QString dllname = currentTemplateDemoFilePath_.right(currentTemplateDemoFilePath_.length() - index - 1); //���ұ߽�ȡ
	if (checked) {
		
		QStringList iniKeyList = pluginTempIni_->childGroups();
		if (iniKeyList.contains(indexStr_)) {
			pluginTempIni_->remove(QString("/%1").arg(indexStr_));
		}
		pluginTempIni_->setValue(QString("%1/modulename").arg(indexStr_), currentTemplateName_);
		pluginTempIni_->setValue(QString("%1/dllname").arg(indexStr_), dllname);
		pluginTempIni_->setValue(QString("%1/dllpath").arg(indexStr_), currentTemplatePath_);
		pluginTempIni_->setValue(QString("%1/dllicon").arg(indexStr_), "icons/ic_nav_setting");
		pluginTempIni_->setValue(QString("%1/showtype").arg(indexStr_), 0);
		pluginTempIni_->setValue(QString("%1/dllindex_row").arg(indexStr_), 0);
		pluginTempIni_->setValue(QString("%1/dllindex_col").arg(indexStr_), 0);
		P_TempInfoIni_->setValue(QString("%1/checkedstatus").arg(indexStr_), true);
	}
	else
	{
		pluginTempIni_->remove(QString("/%1").arg(indexStr_));
		P_TempInfoIni_->setValue(QString("%1/checkedstatus").arg(indexStr_), false);
	}
}
//����ini
void TemplateItemLabelForm::slotRefreshPlgunIni(const QString &path)
{
	int index = currentTemplateDemoFilePath_.lastIndexOf("/"); //�Ӻ������"/"λ��
	QString dllname = currentTemplateDemoFilePath_.right(currentTemplateDemoFilePath_.length() - index - 1); //���ұ߽�ȡ
	if (ui->checkBox_load->isChecked()) {

		QStringList iniKeyList = pluginTempIni_->childGroups();
		if (iniKeyList.contains(indexStr_)) {
			pluginTempIni_->remove(QString("/%1").arg(indexStr_));
		}
		pluginTempIni_->setValue(QString("%1/modulename").arg(indexStr_), currentTemplateName_);
		pluginTempIni_->setValue(QString("%1/dllname").arg(indexStr_), dllname);
		pluginTempIni_->setValue(QString("%1/dllpath").arg(indexStr_), path);
		pluginTempIni_->setValue(QString("%1/dllicon").arg(indexStr_), "icons/ic_nav_setting");
		pluginTempIni_->setValue(QString("%1/showtype").arg(indexStr_), 0);
		pluginTempIni_->setValue(QString("%1/dllindex_row").arg(indexStr_), 0);
		pluginTempIni_->setValue(QString("%1/dllindex_col").arg(indexStr_), 0);
		P_TempInfoIni_->setValue(QString("%1/checkedstatus").arg(indexStr_), true);
	}
}
//��ǰ����Ƿ����
void TemplateItemLabelForm::SetCheckedLoadStatus(bool checked)
{
	ui->checkBox_load->setChecked(checked);
}
void TemplateItemLabelForm::setEnabledOthers(bool checked)
{
	ui->toolButton_del->setEnabled(checked);
	ui->toolButton_edit->setEnabled(checked);
	ui->toolButton_export->setEnabled(checked);
}