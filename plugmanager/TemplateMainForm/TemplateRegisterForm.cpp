#include "TemplateRegisterForm.h"
#include <qfile.h>
#include <qdir.h>
#include <qdatetime.h>
#include <QFileInfo>
#include "mrun_messagebox.h"
#include <QTextCodec>
#pragma execution_character_set("utf-8")
TemplateRegisterForm::TemplateRegisterForm(bool isEdit, QString indexinikey, const QString title, QWidget *parent)
    : QWidget(parent)
    ,isEdit_(isEdit)
    , indexIniKey_(indexinikey)
    , title_(title)
{
    ui.setupUi(this);
    this->setAttribute(Qt::WA_ShowModal);
    this->setWindowTitle(title_);
    P_templateIni_ = new QSettings(QString("%1/template/TemplateManager.ini").arg(QCoreApplication::applicationDirPath())
        , QSettings::IniFormat);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    P_templateIni_->setIniCodec(QTextCodec::codecForName("UTF-8"));
    if (isEdit_)
    {
        ui.groupBox->setTitle("�༭�޸�ģ��");
    }
}

TemplateRegisterForm::~TemplateRegisterForm()
{
}
//�����ļ���
bool TemplateRegisterForm::copyFileToPath(QString sourceDir, QString toDir, bool coverFileIfExist)
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

	if (sourceDir == toDir) {
		return true;
	}
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
//�����ļ��У�
bool TemplateRegisterForm::copyDirectoryFiles(const QString & fromDir, const QString & toDir, bool coverFileIfExist)
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

        else {            
			if (fileInfo.filePath() == targetDir.filePath(fileInfo.fileName())) {
				continue;
			}
			/**< �������ǲ���ʱ�������ļ�����ɾ������ */
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

void TemplateRegisterForm::on_toolButton_exportdemofile_clicked()//��������ļ�
{
    ui.lineEdit_demofile->clear();
    QString sourcepath;
    if (ui.comboBox_demofile->currentIndex() == 0) {
        sourcepath = QFileDialog::getOpenFileName(this,
            tr("��ѡ��Ҫ������ļ�"),
            "./",///home/lhd/tongji/Raccoon/map_editor/map ·��
            "*");//�ļ����� JPEG Files(*.jpg);;PNG Files(*.png);;PGM Files(*.pgm)
    }
    else
    {
        sourcepath = QFileDialog::getExistingDirectory(this, tr("��ѡ��Ҫ������ļ�"), "./");
        if (sourcepath.isEmpty())
        {
            return;
        }
    }
    ui.lineEdit_demofile->setText(sourcepath);
    
}
void TemplateRegisterForm::on_toolButton_exportfuncdesc_clicked()//���빦��˵��
{
    ui.lineEdit_functionDesc->clear();
    QString sourcepath;
    if (ui.comboBox_functionDesc->currentIndex() == 0) {
        sourcepath = QFileDialog::getOpenFileName(this,
            tr("��ѡ��Ҫ����Ĺ���˵��"),
            "./",///home/lhd/tongji/Raccoon/map_editor/map ·��
            "*");//�ļ����� JPEG Files(*.jpg);;PNG Files(*.png);;PGM Files(*.pgm)
        
    }
    else
    {
        sourcepath = QFileDialog::getExistingDirectory(this, tr("��ѡ��Ҫ����Ĺ���˵��"), "./");
        if (sourcepath.isEmpty())
        {
            return;
        }
    }
    ui.lineEdit_functionDesc->setText(sourcepath);
}
void TemplateRegisterForm::on_toolButton_exportimg_clicked()//�ϴ�Ԥ������
{
    ui.lineEdit_previewimg->clear();

    QString sourcepath;
    if (ui.comboBox_previewimg->currentIndex() == 0) {
        sourcepath = QFileDialog::getOpenFileName(this,
            tr("��ѡ��Ҫ�ϴ���Ԥ������"),
            "./",///home/lhd/tongji/Raccoon/map_editor/map ·��
            "*");//�ļ����� JPEG Files(*.jpg);;PNG Files(*.png);;PGM Files(*.pgm)

    }
    else
    {
        sourcepath = QFileDialog::getExistingDirectory(this, tr("��ѡ��Ҫ�ϴ���Ԥ������"), "./");
        if (sourcepath.isEmpty())
        {
            return;
        }
    }
    ui.lineEdit_previewimg->setText(sourcepath);
}
QString TemplateRegisterForm::ReadIni(const QString &key)
{
    return P_templateIni_->value(QString("pluginpath/%1").arg(key)).toString();
}
void TemplateRegisterForm::ClearData()
{
    ui.lineEdit_author->clear();
    ui.lineEdit_name->clear();
    ui.lineEdit_demofile->clear();
    ui.lineEdit_functionDesc->clear();
    ui.lineEdit_previewimg->clear();
    ui.textEdit_oper->clear();
}
void TemplateRegisterForm::on_toolButton_save_clicked()//��������;
{
    if (ui.lineEdit_name->text().simplified().isEmpty()) {
        setmessage(HINT_FLAG::MRUN_HINT, QString::fromLocal8Bit("���Ʋ���Ϊ��"));
        return;
    }
    if (ui.textEdit_oper->toPlainText().simplified().isEmpty()) {
        setmessage(HINT_FLAG::MRUN_HINT, QString::fromLocal8Bit("���������"));
        return;
    }
    if (ui.lineEdit_demofile->text().simplified().isEmpty()) {
        setmessage(HINT_FLAG::MRUN_HINT, QString::fromLocal8Bit("��������ļ�����Ϊ��"));
        return;
    }
	else if (ui.lineEdit_demofile->text().simplified().contains(QRegExp("[\\x4e00-\\x9fa5]+"))){
		setmessage(HINT_FLAG::MRUN_HINT, QString::fromLocal8Bit("��������ļ��в��ܰ��������ַ�"));
		return;
	}
    if (ui.lineEdit_functionDesc->text().simplified().isEmpty()) {
        setmessage(HINT_FLAG::MRUN_HINT, QString::fromLocal8Bit("���빦��˵������Ϊ��"));
        return;
    }
	else if (ui.lineEdit_functionDesc->text().simplified().contains(QRegExp("[\\x4e00-\\x9fa5]+"))) {
		setmessage(HINT_FLAG::MRUN_HINT, QString::fromLocal8Bit("���빦��˵���в��ܰ��������ַ�"));
		return;
	}
    if (ui.lineEdit_previewimg->text().simplified().isEmpty()) {
        setmessage(HINT_FLAG::MRUN_HINT, QString::fromLocal8Bit("�ϴ�Ԥ�����治��Ϊ��"));
        return;
    }
	else if (ui.lineEdit_previewimg->text().simplified().contains(QRegExp("[\\x4e00-\\x9fa5]+"))) {
		setmessage(HINT_FLAG::MRUN_HINT, QString::fromLocal8Bit("�ϴ�Ԥ�������в��ܰ��������ַ�"));
		return;
	}
    if (ui.lineEdit_author->text().simplified().isEmpty()) {
        setmessage(HINT_FLAG::MRUN_HINT, QString::fromLocal8Bit("����봴��������"));
        return;
    }
    QString topath = ReadIni(ini_key_path_);
    QString currentWorkPath = QDir::currentPath();
    if (topath.isEmpty()) {
        setmessage(HINT_FLAG::MRUN_ERROR, QString::fromLocal8Bit("����ʧ��"));
    }

     QDateTime time = QDateTime::currentDateTime();
     QString timeStr = time.toString("yyyyMMddhhmmsszzz");//��Ϊkeyֵ
	 
	if (isEdit_) {
		topath = QString("%1%2").arg(topath).arg(indexIniKey_);
	}
	else
	{
		topath = QString("%1%2").arg(topath).arg(timeStr);
	}
     QDir dir;
    dir.setPath(QString(currentWorkPath+topath));
    if (!dir.exists()) {
        dir.mkpath(QString(currentWorkPath + topath));
    }
    if (ui.comboBox_demofile->currentIndex() == 0) {

        if (!copyFileToPath(ui.lineEdit_demofile->text().simplified(), QString(currentWorkPath + topath), true)) {
            setmessage(HINT_FLAG::MRUN_HINT, QString::fromLocal8Bit("��������ļ�ʧ��"));
			if (!isEdit_) {
				dir.removeRecursively();
			}
            ui.lineEdit_demofile->clear();
            return;
        }
    }
    else
    {
        if (!copyDirectoryFiles(ui.lineEdit_demofile->text().simplified(), QString(currentWorkPath + topath), true)) {
            setmessage(HINT_FLAG::MRUN_HINT, QString::fromLocal8Bit("��������ļ�ʧ��"));
			if (!isEdit_) {
				dir.removeRecursively();
			}
            ui.lineEdit_demofile->clear();
            return;
        }
    }
    if (ui.comboBox_functionDesc->currentIndex() == 0) {

        if (!copyFileToPath(ui.lineEdit_functionDesc->text().simplified(), QString(currentWorkPath + topath), true)) {
            setmessage(HINT_FLAG::MRUN_HINT, QString::fromLocal8Bit("���빦��˵��ʧ��"));
			if (!isEdit_) {
				dir.removeRecursively();
			}
            ui.lineEdit_functionDesc->clear();
            return;
        }
    }
    else
    {
        if (!copyDirectoryFiles(ui.lineEdit_functionDesc->text().simplified(), QString(currentWorkPath + topath), true)) {
            setmessage(HINT_FLAG::MRUN_HINT, QString::fromLocal8Bit("���빦��˵��ʧ��"));
			if (!isEdit_) {
				dir.removeRecursively();
			}
            ui.lineEdit_functionDesc->clear();
            return;
        }
    }
    if (ui.comboBox_previewimg->currentIndex() == 0) {
        if (!copyFileToPath(ui.lineEdit_previewimg->text().simplified(), QString(currentWorkPath + topath), true)) {
            setmessage(HINT_FLAG::MRUN_HINT, QString::fromLocal8Bit("�ϴ�Ԥ������ʧ��"));
			if (!isEdit_) {
				dir.removeRecursively();
			}
            ui.lineEdit_previewimg->clear();
            return;
        }
    }
    else
    {
        if (!copyDirectoryFiles(ui.lineEdit_previewimg->text().simplified(), QString(currentWorkPath + topath), true)) {
            setmessage(HINT_FLAG::MRUN_HINT, QString::fromLocal8Bit("�ϴ�Ԥ������ʧ��"));
			if (!isEdit_) {
				dir.removeRecursively();
			}
            ui.lineEdit_previewimg->clear();
            return;
        }
    }
    //QString currentPath = QDir::currentPath();
    P_TempInfoIni_ = new QSettings(currentWorkPath +QString("%1TempInfo.ini").arg(ReadIni(ini_key_path_))
      , QSettings::IniFormat);
   // int count;
    QString keyIni;//Ĭ��ʱ�����ɣ��༭״̬ʱͨ����������
    if (isEdit_) {//��ǰ�Ƿ��Ǳ༭״̬
        keyIni = indexIniKey_;
        P_TempInfoIni_->setValue(QString("%1/templatePath").arg(keyIni), topath);//ģ��·��

        QFileInfo infoDemoFile(ui.lineEdit_demofile->text().simplified());//ģ������ļ�·��
        P_TempInfoIni_->setValue(QString("%1/templateDemoFilePath").arg(keyIni), topath + "/" + infoDemoFile.fileName());//ģ������ļ�·��
        QFileInfo infoFDP(ui.lineEdit_functionDesc->text().simplified());//����˵���ĵ�
        P_TempInfoIni_->setValue(QString("%1/templateFunctionDescPath").arg(keyIni), topath + "/" + infoFDP.fileName());//����˵���ĵ�·��
        QFileInfo infoPIP(ui.lineEdit_previewimg->text().simplified());//Ԥ������
        P_TempInfoIni_->setValue(QString("%1/templatePreviewImgPath").arg(keyIni), topath + "/" + infoPIP.fileName());//Ԥ������ͼƬ·��
        P_TempInfoIni_->setValue(QString("%1/templateName").arg(keyIni), ui.lineEdit_name->text().simplified());//ģ������
        P_TempInfoIni_->setValue(QString("%1/templateAuthor").arg(keyIni), ui.lineEdit_author->text().simplified());//������
        P_TempInfoIni_->setValue(QString("%1/templateOper").arg(keyIni), ui.textEdit_oper->toPlainText().simplified());//����
        //P_TempInfoIni_->setValue(QString("%1/templateCreateTime").arg(count), time.toString("yyyy-MM-dd hh:mm:ss").simplified());//����ʱ��
        P_TempInfoIni_->setValue(QString("%1/templateChangeTime").arg(keyIni), time.toString("yyyy-MM-dd hh:mm:ss").simplified().simplified());//�޸�ʱ��
		emit signalRefreshPlgunIni(topath);
       /* QString del_file = QString(currentTemplatePath_);
        QDir oldDir;
        oldDir.setPath(del_file);
        oldDir.removeRecursively();*///ȡ��ɾ���ļ��У���Ϊ���ڱ༭���������µ��ļ���
    }
    else //ע��
    {
        //count = P_TempInfoIni_->childGroups().size();
        keyIni = timeStr;
        P_TempInfoIni_->setValue(QString("%1/templatePath").arg(keyIni), topath);//ģ��·��
        QFileInfo infoDemoFile(ui.lineEdit_demofile->text().simplified());//ģ������ļ�·��
        P_TempInfoIni_->setValue(QString("%1/templateDemoFilePath").arg(keyIni),topath + "/" + infoDemoFile.fileName());//ģ������ļ�·��
        QFileInfo infoFDP(ui.lineEdit_functionDesc->text().simplified());//����˵���ĵ�
        P_TempInfoIni_->setValue(QString("%1/templateFunctionDescPath").arg(keyIni), topath + "/" + infoFDP.fileName());//����˵���ĵ�·��
        QFileInfo infoPIP(ui.lineEdit_previewimg->text().simplified());//Ԥ������
        P_TempInfoIni_->setValue(QString("%1/templatePreviewImgPath").arg(keyIni), topath + "/" + infoPIP.fileName());//Ԥ������ͼƬ·��
        P_TempInfoIni_->setValue(QString("%1/templateName").arg(keyIni), ui.lineEdit_name->text().simplified());//ģ������
        P_TempInfoIni_->setValue(QString("%1/templateAuthor").arg(keyIni), ui.lineEdit_author->text().simplified());//������
        P_TempInfoIni_->setValue(QString("%1/templateOper").arg(keyIni), ui.textEdit_oper->toPlainText().simplified());//����
        P_TempInfoIni_->setValue(QString("%1/templateCreateTime").arg(keyIni), time.toString("yyyy-MM-dd hh:mm:ss").simplified());//����ʱ��
        P_TempInfoIni_->setValue(QString("%1/templateChangeTime").arg(keyIni), time.toString("yyyy-MM-dd hh:mm:ss").simplified().simplified());//�޸�ʱ��
    }
    
    emit signalRefreshList();
    ClearData();
    this->close();
   
}
void TemplateRegisterForm::on_toolButton_cancel_clicked()//ȡ������
{
    ClearData();
    this->close();
}
void TemplateRegisterForm::set_lineEdit_name(const QString &str)//����
{
    ui.lineEdit_name->setText(str);
}
void TemplateRegisterForm::set_textEdit_oper(const QString &str)//����
{
    ui.textEdit_oper->setText(str);
}
void TemplateRegisterForm::set_lineEdit_demofile(const QString &str)//�����ļ�
{
    QFileInfo info(str);
    if (info.isFile()) {
        ui.comboBox_demofile->setCurrentIndex(0);
    }
    else
    {
        ui.comboBox_demofile->setCurrentIndex(1);
    }
    ui.lineEdit_demofile->setText(str);
}
void TemplateRegisterForm::set_lineEdit_functionDesc(const QString &str)//����˵��
{
    QFileInfo info(str);
    if (info.isFile()) {
        ui.comboBox_functionDesc->setCurrentIndex(0);
    }
    else
    {
        ui.comboBox_functionDesc->setCurrentIndex(1);
    }
    ui.lineEdit_functionDesc->setText(str);
}
void TemplateRegisterForm::set_lineEdit_previewimg(const QString &str)//Ԥ������
{
    QFileInfo info(str);
    if (info.isFile()) {
        ui.comboBox_previewimg->setCurrentIndex(0);
    }
    else
    {
        ui.comboBox_previewimg->setCurrentIndex(1);
    }
    ui.lineEdit_previewimg->setText(str);
}
void TemplateRegisterForm::set_lineEdit_author(const QString &str)//������
{
    ui.lineEdit_author->setText(str);
}