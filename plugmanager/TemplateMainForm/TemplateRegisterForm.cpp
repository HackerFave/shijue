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
        ui.groupBox->setTitle("编辑修改模板");
    }
}

TemplateRegisterForm::~TemplateRegisterForm()
{
}
//拷贝文件：
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
//拷贝文件夹：
bool TemplateRegisterForm::copyDirectoryFiles(const QString & fromDir, const QString & toDir, bool coverFileIfExist)
{
    QDir sourceDir(fromDir);
    int index = fromDir.lastIndexOf("/"); //从后面查找"/"位置
    QString dirname = fromDir.right(fromDir.length() - index - 1); //从右边截取
    QString newToDir = toDir + "/" + dirname;
    QDir targetDir(newToDir);
    if (!targetDir.exists()) {    /**< 如果目标目录不存在，则进行创建 */

        if (!targetDir.mkdir(targetDir.absolutePath()))

            return false;

    }

    QFileInfoList fileInfoList = sourceDir.entryInfoList();
    foreach(QFileInfo fileInfo, fileInfoList) {

        if (fileInfo.fileName() == "." || fileInfo.fileName() == "..")

            continue;

        if (fileInfo.isDir()) {    /**< 当为目录时，递归的进行copy */

            if (!copyDirectoryFiles(fileInfo.filePath(),

                targetDir.filePath(fileInfo.fileName()),

                coverFileIfExist))

                return false;

        }

        else {            
			if (fileInfo.filePath() == targetDir.filePath(fileInfo.fileName())) {
				continue;
			}
			/**< 当允许覆盖操作时，将旧文件进行删除操作 */
			if (coverFileIfExist && targetDir.exists(fileInfo.fileName())) {

				targetDir.remove(fileInfo.fileName());

			}
            /// 进行文件copy
            if (!QFile::copy(fileInfo.filePath(),

                targetDir.filePath(fileInfo.fileName()))) {

                return false;

            }

        }

    }

    return true;
}

void TemplateRegisterForm::on_toolButton_exportdemofile_clicked()//导入程序文件
{
    ui.lineEdit_demofile->clear();
    QString sourcepath;
    if (ui.comboBox_demofile->currentIndex() == 0) {
        sourcepath = QFileDialog::getOpenFileName(this,
            tr("请选择要导入的文件"),
            "./",///home/lhd/tongji/Raccoon/map_editor/map 路径
            "*");//文件过滤 JPEG Files(*.jpg);;PNG Files(*.png);;PGM Files(*.pgm)
    }
    else
    {
        sourcepath = QFileDialog::getExistingDirectory(this, tr("请选择要导入的文件"), "./");
        if (sourcepath.isEmpty())
        {
            return;
        }
    }
    ui.lineEdit_demofile->setText(sourcepath);
    
}
void TemplateRegisterForm::on_toolButton_exportfuncdesc_clicked()//导入功能说明
{
    ui.lineEdit_functionDesc->clear();
    QString sourcepath;
    if (ui.comboBox_functionDesc->currentIndex() == 0) {
        sourcepath = QFileDialog::getOpenFileName(this,
            tr("请选择要导入的功能说明"),
            "./",///home/lhd/tongji/Raccoon/map_editor/map 路径
            "*");//文件过滤 JPEG Files(*.jpg);;PNG Files(*.png);;PGM Files(*.pgm)
        
    }
    else
    {
        sourcepath = QFileDialog::getExistingDirectory(this, tr("请选择要导入的功能说明"), "./");
        if (sourcepath.isEmpty())
        {
            return;
        }
    }
    ui.lineEdit_functionDesc->setText(sourcepath);
}
void TemplateRegisterForm::on_toolButton_exportimg_clicked()//上传预览画面
{
    ui.lineEdit_previewimg->clear();

    QString sourcepath;
    if (ui.comboBox_previewimg->currentIndex() == 0) {
        sourcepath = QFileDialog::getOpenFileName(this,
            tr("请选择要上传的预览画面"),
            "./",///home/lhd/tongji/Raccoon/map_editor/map 路径
            "*");//文件过滤 JPEG Files(*.jpg);;PNG Files(*.png);;PGM Files(*.pgm)

    }
    else
    {
        sourcepath = QFileDialog::getExistingDirectory(this, tr("请选择要上传的预览画面"), "./");
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
void TemplateRegisterForm::on_toolButton_save_clicked()//保存配置;
{
    if (ui.lineEdit_name->text().simplified().isEmpty()) {
        setmessage(HINT_FLAG::MRUN_HINT, QString::fromLocal8Bit("名称不能为空"));
        return;
    }
    if (ui.textEdit_oper->toPlainText().simplified().isEmpty()) {
        setmessage(HINT_FLAG::MRUN_HINT, QString::fromLocal8Bit("请添加描述"));
        return;
    }
    if (ui.lineEdit_demofile->text().simplified().isEmpty()) {
        setmessage(HINT_FLAG::MRUN_HINT, QString::fromLocal8Bit("导入程序文件不能为空"));
        return;
    }
	else if (ui.lineEdit_demofile->text().simplified().contains(QRegExp("[\\x4e00-\\x9fa5]+"))){
		setmessage(HINT_FLAG::MRUN_HINT, QString::fromLocal8Bit("导入程序文件中不能包含中文字符"));
		return;
	}
    if (ui.lineEdit_functionDesc->text().simplified().isEmpty()) {
        setmessage(HINT_FLAG::MRUN_HINT, QString::fromLocal8Bit("导入功能说明不能为空"));
        return;
    }
	else if (ui.lineEdit_functionDesc->text().simplified().contains(QRegExp("[\\x4e00-\\x9fa5]+"))) {
		setmessage(HINT_FLAG::MRUN_HINT, QString::fromLocal8Bit("导入功能说明中不能包含中文字符"));
		return;
	}
    if (ui.lineEdit_previewimg->text().simplified().isEmpty()) {
        setmessage(HINT_FLAG::MRUN_HINT, QString::fromLocal8Bit("上传预览画面不能为空"));
        return;
    }
	else if (ui.lineEdit_previewimg->text().simplified().contains(QRegExp("[\\x4e00-\\x9fa5]+"))) {
		setmessage(HINT_FLAG::MRUN_HINT, QString::fromLocal8Bit("上传预览画面中不能包含中文字符"));
		return;
	}
    if (ui.lineEdit_author->text().simplified().isEmpty()) {
        setmessage(HINT_FLAG::MRUN_HINT, QString::fromLocal8Bit("请键入创建人名称"));
        return;
    }
    QString topath = ReadIni(ini_key_path_);
    QString currentWorkPath = QDir::currentPath();
    if (topath.isEmpty()) {
        setmessage(HINT_FLAG::MRUN_ERROR, QString::fromLocal8Bit("保存失败"));
    }

     QDateTime time = QDateTime::currentDateTime();
     QString timeStr = time.toString("yyyyMMddhhmmsszzz");//作为key值
	 
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
            setmessage(HINT_FLAG::MRUN_HINT, QString::fromLocal8Bit("导入程序文件失败"));
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
            setmessage(HINT_FLAG::MRUN_HINT, QString::fromLocal8Bit("导入程序文件失败"));
			if (!isEdit_) {
				dir.removeRecursively();
			}
            ui.lineEdit_demofile->clear();
            return;
        }
    }
    if (ui.comboBox_functionDesc->currentIndex() == 0) {

        if (!copyFileToPath(ui.lineEdit_functionDesc->text().simplified(), QString(currentWorkPath + topath), true)) {
            setmessage(HINT_FLAG::MRUN_HINT, QString::fromLocal8Bit("导入功能说明失败"));
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
            setmessage(HINT_FLAG::MRUN_HINT, QString::fromLocal8Bit("导入功能说明失败"));
			if (!isEdit_) {
				dir.removeRecursively();
			}
            ui.lineEdit_functionDesc->clear();
            return;
        }
    }
    if (ui.comboBox_previewimg->currentIndex() == 0) {
        if (!copyFileToPath(ui.lineEdit_previewimg->text().simplified(), QString(currentWorkPath + topath), true)) {
            setmessage(HINT_FLAG::MRUN_HINT, QString::fromLocal8Bit("上传预览画面失败"));
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
            setmessage(HINT_FLAG::MRUN_HINT, QString::fromLocal8Bit("上传预览画面失败"));
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
    QString keyIni;//默认时间生成，编辑状态时通过参数传递
    if (isEdit_) {//当前是否是编辑状态
        keyIni = indexIniKey_;
        P_TempInfoIni_->setValue(QString("%1/templatePath").arg(keyIni), topath);//模板路径

        QFileInfo infoDemoFile(ui.lineEdit_demofile->text().simplified());//模板程序文件路径
        P_TempInfoIni_->setValue(QString("%1/templateDemoFilePath").arg(keyIni), topath + "/" + infoDemoFile.fileName());//模板程序文件路径
        QFileInfo infoFDP(ui.lineEdit_functionDesc->text().simplified());//功能说明文档
        P_TempInfoIni_->setValue(QString("%1/templateFunctionDescPath").arg(keyIni), topath + "/" + infoFDP.fileName());//功能说明文档路径
        QFileInfo infoPIP(ui.lineEdit_previewimg->text().simplified());//预览画面
        P_TempInfoIni_->setValue(QString("%1/templatePreviewImgPath").arg(keyIni), topath + "/" + infoPIP.fileName());//预览画面图片路径
        P_TempInfoIni_->setValue(QString("%1/templateName").arg(keyIni), ui.lineEdit_name->text().simplified());//模板名称
        P_TempInfoIni_->setValue(QString("%1/templateAuthor").arg(keyIni), ui.lineEdit_author->text().simplified());//创建人
        P_TempInfoIni_->setValue(QString("%1/templateOper").arg(keyIni), ui.textEdit_oper->toPlainText().simplified());//描述
        //P_TempInfoIni_->setValue(QString("%1/templateCreateTime").arg(count), time.toString("yyyy-MM-dd hh:mm:ss").simplified());//创建时间
        P_TempInfoIni_->setValue(QString("%1/templateChangeTime").arg(keyIni), time.toString("yyyy-MM-dd hh:mm:ss").simplified().simplified());//修改时间
		emit signalRefreshPlgunIni(topath);
       /* QString del_file = QString(currentTemplatePath_);
        QDir oldDir;
        oldDir.setPath(del_file);
        oldDir.removeRecursively();*///取消删除文件夹，因为现在编辑不会生成新的文件夹
    }
    else //注册
    {
        //count = P_TempInfoIni_->childGroups().size();
        keyIni = timeStr;
        P_TempInfoIni_->setValue(QString("%1/templatePath").arg(keyIni), topath);//模板路径
        QFileInfo infoDemoFile(ui.lineEdit_demofile->text().simplified());//模板程序文件路径
        P_TempInfoIni_->setValue(QString("%1/templateDemoFilePath").arg(keyIni),topath + "/" + infoDemoFile.fileName());//模板程序文件路径
        QFileInfo infoFDP(ui.lineEdit_functionDesc->text().simplified());//功能说明文档
        P_TempInfoIni_->setValue(QString("%1/templateFunctionDescPath").arg(keyIni), topath + "/" + infoFDP.fileName());//功能说明文档路径
        QFileInfo infoPIP(ui.lineEdit_previewimg->text().simplified());//预览画面
        P_TempInfoIni_->setValue(QString("%1/templatePreviewImgPath").arg(keyIni), topath + "/" + infoPIP.fileName());//预览画面图片路径
        P_TempInfoIni_->setValue(QString("%1/templateName").arg(keyIni), ui.lineEdit_name->text().simplified());//模板名称
        P_TempInfoIni_->setValue(QString("%1/templateAuthor").arg(keyIni), ui.lineEdit_author->text().simplified());//创建人
        P_TempInfoIni_->setValue(QString("%1/templateOper").arg(keyIni), ui.textEdit_oper->toPlainText().simplified());//描述
        P_TempInfoIni_->setValue(QString("%1/templateCreateTime").arg(keyIni), time.toString("yyyy-MM-dd hh:mm:ss").simplified());//创建时间
        P_TempInfoIni_->setValue(QString("%1/templateChangeTime").arg(keyIni), time.toString("yyyy-MM-dd hh:mm:ss").simplified().simplified());//修改时间
    }
    
    emit signalRefreshList();
    ClearData();
    this->close();
   
}
void TemplateRegisterForm::on_toolButton_cancel_clicked()//取消配置
{
    ClearData();
    this->close();
}
void TemplateRegisterForm::set_lineEdit_name(const QString &str)//名称
{
    ui.lineEdit_name->setText(str);
}
void TemplateRegisterForm::set_textEdit_oper(const QString &str)//描述
{
    ui.textEdit_oper->setText(str);
}
void TemplateRegisterForm::set_lineEdit_demofile(const QString &str)//程序文件
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
void TemplateRegisterForm::set_lineEdit_functionDesc(const QString &str)//功能说明
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
void TemplateRegisterForm::set_lineEdit_previewimg(const QString &str)//预览画面
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
void TemplateRegisterForm::set_lineEdit_author(const QString &str)//创建人
{
    ui.lineEdit_author->setText(str);
}