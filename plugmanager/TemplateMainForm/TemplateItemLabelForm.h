#pragma once

#include <QWidget>
#include "ui_TemplateItemLabelForm.h"
#include "TemplateRegisterForm.h"
#include <QKeyEvent>
#include <QWheelEvent>
namespace ui {
    class TemplateItemLabelForm;
}
class TemplateItemLabelForm : public QWidget
{
    Q_OBJECT

public:
    TemplateItemLabelForm(const QString &index="",const QString & objectname="",QWidget *parent = Q_NULLPTR);
    ~TemplateItemLabelForm();
    void set_label_temname(const QString &str);//模板名称
    void SetCurrentTemplateName(const QString &templatename) {//创建名称
        currentTemplateName_ = templatename;
    }
    void set_textEdit_info(const QString &str);//描述
    void set_label_createtime(const QString &str);//创建时间
    void set_label_changetime(const QString &str);//修改时间
    void SetCurrentDocPath(const QString &docpath) {
        currentDocPath_ = docpath;
    }
    void SetCurrentImgPath(const QString &imgpath) {//预览图
        currentImgPath_ = imgpath;
    }
    void SetCurrentImgShow(const QString &imgpath);//设置预览画面
    void SetCurrentTemplatePath(const QString &templatepath) {//模板路径
        currentTemplatePath_ = templatepath;
    }
    void SetCurrentTemplateAuthor(const QString &templateauthor) {//创建人
        currentTemplateAuthor_ = templateauthor;
    }
    void SetCurrentTemplateDemoFilePath(const QString &templatedemofilepath) {//程序文件路径
        currentTemplateDemoFilePath_ = templatedemofilepath;
    }
	void SetCheckedLoadStatus(bool checked);//当前插件是否加载
    //拷贝文件夹
    bool copyDirectoryFiles(const QString & fromDir, const QString & toDir, bool coverFileIfExist);
    bool copyFileToPath(QString sourceDir, QString toDir, bool coverFileIfExist);//拷贝文件
    QFileInfoList GetFileList(QString path);//地柜遍历文件夹
	void setEnabledOthers(bool checked);
protected:
    void wheelEvent(QWheelEvent *event) override;
private slots:
    void on_toolButton_doc_clicked();//打开说明文档
    void on_toolButton_export_clicked();//导出
    void on_toolButton_edit_clicked();//编辑
    void on_toolButton_del_clicked();//删除
    void slotRefreshListItemLabel();//刷新编辑后的数据
	void on_checkBox_load_clicked(bool checked);//是否加载插件
	void slotRefreshPlgunIni(const QString &);//刷新编辑保存后的插件加载ini
signals:
    void signalRefreshListItemLabel();//刷新编辑后的数据
private:
    Ui::TemplateItemLabelForm* ui;
    QString currentDocPath_;//说明文档
    QString currentImgPath_;//预览图
    QString currentTemplatePath_;//当前模板路径
    QString currentTemplateName_;//当前模板名称
    QString currentTemplateDemoFilePath_;//当前模板程序文件路径
    QString currentTemplateOper_;//当前模板描述
    QString currentTemplateAuthor_;//当前模板创建人
    TemplateRegisterForm *P_TemplateRegisterForm = Q_NULLPTR;//编辑界面
    QString indexStr_;//模板页属性
    QString objectName_;//当前页属性名
    QSettings *P_templateIni_ = Q_NULLPTR;//第一级ini获取当前操作界面的索引以及路径
    QSettings *P_TempInfoIni_ = Q_NULLPTR;//管理每一个模板路径
	QSettings * pluginTempIni_ = Q_NULLPTR;//加载插件的ini
};
