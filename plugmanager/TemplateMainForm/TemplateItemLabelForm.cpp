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
    P_TemplateRegisterForm = new TemplateRegisterForm(true, indexStr_, "编辑窗口");
    P_templateIni_ = new QSettings(QDir::currentPath() + QString("/template/TemplateManager.ini")
        , QSettings::IniFormat);//第一级ini

    P_TempInfoIni_ = new QSettings(QDir::currentPath()+QString("%1TempInfo.ini").arg(
        P_templateIni_->value(QString("pluginpath/%1").arg(objectName_)).toString())
        , QSettings::IniFormat);//第二级ini

	pluginTempIni_ = new QSettings(QDir::currentPath() + QString("/config/plugins/plugin.ini")
		, QSettings::IniFormat);//插件加载的配置文件
    connect(P_TemplateRegisterForm, SIGNAL(signalRefreshList()), this, SLOT(slotRefreshListItemLabel()));
	connect(P_TemplateRegisterForm, SIGNAL(signalRefreshPlgunIni(const QString &)), this, SLOT(slotRefreshPlgunIni(const QString &)));
	
}

TemplateItemLabelForm::~TemplateItemLabelForm()
{
}
void TemplateItemLabelForm::set_label_temname(const QString &str)//模板名称
{
    ui->label_temname->setText(str);
}
void TemplateItemLabelForm::set_textEdit_info(const QString &str)//描述
{
    currentTemplateOper_ = str;
    ui->textEdit_info->setText(str);
}
void TemplateItemLabelForm::set_label_createtime(const QString &str)//创建时间
{
    ui->label_createtime->setText(str);
}
void TemplateItemLabelForm::set_label_changetime(const QString &str)//修改时间
{
    ui->label_changetime->setText(str);
}
void TemplateItemLabelForm::SetCurrentImgShow(const QString &imgpath)//设置预览画面
{
    /*QImage* img = new QImage;
    img->load(imgpath);
    QImage scaledimg;
    scaledimg = img->scaled(ui->label_image->width(), ui->label_image->height(), Qt::KeepAspectRatio);
    ui->label_image->setPixmap(QPixmap::fromImage(scaledimg));*/
    ui->widget_image->setLoadImage(QDir::currentPath()+imgpath);
}
void TemplateItemLabelForm::on_toolButton_doc_clicked()//打开说明文档
{
    if (!QDesktopServices::openUrl(QUrl::fromLocalFile(QDir::currentPath()+ currentDocPath_))) {
        setmessage(HINT_FLAG::MRUN_ERROR, "文档打开失败");
    }
}
//导出插件
void TemplateItemLabelForm::on_toolButton_export_clicked()
{
    QString topath = QFileDialog::getExistingDirectory(this, tr("请选择要导出目录"), "./");
    if (topath.isEmpty())
    {
       // setmessage(HINT_FLAG::MRUN_HINT, "导出模板失败");
        return;
    }
    QString zippath = QDir::currentPath() + currentTemplatePath_;//工作目录+相对路径 压缩文件生成路径
    const std::string expoerpath = std::string((const char *)zippath.toLocal8Bit().simplified());
    const std::string zip = expoerpath + ".zip";//压缩文件必须加后缀，否则生成崩溃
    //zipper::Unzipper unzipSolution(path);
    zipper::Zipper zippper(zip);//新建生成zip，将需要的文件放入压缩包
    QFileInfoList fileinfoList = GetFileList(QString::fromLocal8Bit(expoerpath.data()));//获取要压缩的文件层级列表
    
     for (int i = 0; i < fileinfoList.size(); i++)
    {
        QString filepath = fileinfoList.at(i).filePath();//文件路径
        std::string strFilePath = std::string(std::string((const char *)filepath.toLocal8Bit()));//转std：：string
       
        QString strRelative = filepath.replace(zippath, "").simplified();//截取获得当前操作文件夹目录
        std::string strRelativePath = std::string(std::string((const char *)strRelative.toLocal8Bit()));//相对压缩包路径
        std::ifstream readFile(strFilePath, std::ios::binary);//读取文件数据
        zippper.add(readFile, strRelativePath);//读取的数据写入相对路径
        readFile.close();
    }
     zippper.close();//压缩包生成数据完成
    if (copyFileToPath(QString::fromLocal8Bit(zip.data()), topath, true)) {
        QFileInfo info;
        info.setFile(QString::fromLocal8Bit(zip.data()));
        zipper::Unzipper unzip(std::string((const char *)topath.toLocal8Bit().simplified())+"/"+
            std::string((const char *)info.fileName().toLocal8Bit().simplified()));
        QDir dirtopath;
        dirtopath.setPath(topath +"/"+ info.baseName());
        QString STR = topath + "/" + info.baseName();
        dirtopath.removeRecursively();//删除文件夹
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
        setmessage(HINT_FLAG::MRUN_HINT, "导出模板成功");
        return;
    }
    //setmessage(HINT_FLAG::MRUN_HINT, "导出模板失败");
}
void TemplateItemLabelForm::on_toolButton_edit_clicked()//编辑
{
    QString currentPath = QDir::currentPath();
    P_TemplateRegisterForm->set_lineEdit_name(currentTemplateName_);//模板名称
    P_TemplateRegisterForm->set_textEdit_oper(currentTemplateOper_);//模板描述
    P_TemplateRegisterForm->set_lineEdit_demofile(currentPath + currentTemplateDemoFilePath_);//当前模板程序文件
    P_TemplateRegisterForm->set_lineEdit_functionDesc(currentPath + currentDocPath_);//文档说明
    P_TemplateRegisterForm->set_lineEdit_previewimg(currentPath + currentImgPath_);//预览图
    P_TemplateRegisterForm->set_lineEdit_author(currentTemplateAuthor_);//作者
    P_TemplateRegisterForm->SetCurrentTemIniKey(objectName_);//当前tab页属性
    P_TemplateRegisterForm->SetCurrentTemplatePath(currentPath + currentTemplatePath_);//当前模板路径
    P_TemplateRegisterForm->show();
}
void TemplateItemLabelForm::on_toolButton_del_clicked()//删除
{ 
    if (messageopertips("确定删除该模板?")) {
        QString del_file = QString(QDir::currentPath()+ currentTemplatePath_);//删除当前模板
        QDir oldDir;
        oldDir.setPath(del_file);
        oldDir.removeRecursively();
        P_TempInfoIni_->remove(QString("/%1").arg(indexStr_));
		QStringList iniKeyList = pluginTempIni_->childGroups();//删除加载配置
		if (iniKeyList.contains(indexStr_)) {
			pluginTempIni_->remove(QString("/%1").arg(indexStr_));
		}
        emit signalRefreshListItemLabel();
    }
    
}
//拷贝文件夹：
bool TemplateItemLabelForm::copyDirectoryFiles(const QString & fromDir, const QString & toDir, bool coverFileIfExist)
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
        else {            /**< 当允许覆盖操作时，将旧文件进行删除操作 */
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
//拷贝文件：
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
////刷新编辑后的数据
void TemplateItemLabelForm::slotRefreshListItemLabel()
{
    emit signalRefreshListItemLabel();
}
void TemplateItemLabelForm::wheelEvent(QWheelEvent *event)
{

}
//递归遍历文件夹
QFileInfoList TemplateItemLabelForm::GetFileList(QString path)
{
    QDir dir(path);

    //列出dir(path)目录文件下所有文件和目录信息，存储到file_list容器
    QFileInfoList file_list = dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    //列出dir(path)目录下所有子文件夹
    QFileInfoList folder_list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    //进行子文件夹folder_list递归遍历，将内容存入file_list容器
    for (int i = 0; i != folder_list.size(); i++)
    {
        QString name = folder_list.at(i).absoluteFilePath();
        QFileInfoList child_file_list = GetFileList(name);
        file_list.append(child_file_list);
    }
    return file_list;
}
//加载插件
void TemplateItemLabelForm::on_checkBox_load_clicked(bool checked)
{
	if (checked) {
		if (!messageopertips("确认加载插件，所有插件加载将在重启后生效")) {
			ui->checkBox_load->setChecked(!checked);
			return;
		}
	}
	else
	{
		if (!messageopertips("确认取消加载插件，所有插件取消将在重启后生效")) {
			ui->checkBox_load->setChecked(!checked);
			return;
		}
	}
	
    int index = currentTemplateDemoFilePath_.lastIndexOf("/"); //从后面查找"/"位置
	QString dllname = currentTemplateDemoFilePath_.right(currentTemplateDemoFilePath_.length() - index - 1); //从右边截取
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
//更新ini
void TemplateItemLabelForm::slotRefreshPlgunIni(const QString &path)
{
	int index = currentTemplateDemoFilePath_.lastIndexOf("/"); //从后面查找"/"位置
	QString dllname = currentTemplateDemoFilePath_.right(currentTemplateDemoFilePath_.length() - index - 1); //从右边截取
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
//当前插件是否加载
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