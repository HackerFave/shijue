#pragma once

#include <QWidget>
#include "ui_TemplateRegisterForm.h"
#include <qsettings.h>
#include <QFileDialog>
class TemplateRegisterForm : public QWidget
{
    Q_OBJECT

public:
    TemplateRegisterForm(bool isedit,QString indexinikey, const QString title="", QWidget *parent = Q_NULLPTR);
    ~TemplateRegisterForm();
    bool copyFileToPath(QString sourceDir, QString toDir, bool coverFileIfExist);//拷贝文件
    bool copyDirectoryFiles(const QString &fromDir, const QString &toDir, bool coverFileIfExist);//拷贝文件夹
    QString ReadIni(const QString &key);
    void SetCurrentTemIniKey(const QString &key) {
        ini_key_path_ = key;
    }
    void SetCurrentTemplatePath(const QString &templatepath) {//当前模板路径
        currentTemplatePath_ = templatepath;
    }
    void ClearData();
    void set_lineEdit_name(const QString &str);//模板名称
    void set_textEdit_oper(const QString &str);//描述信息
    void set_lineEdit_demofile(const QString &str);//程序文件
    void set_lineEdit_functionDesc(const QString &str);//功能说明
    void set_lineEdit_previewimg(const QString &str);//预览画面
    void set_lineEdit_author(const QString &str);//创建人
private slots:
    void on_toolButton_exportdemofile_clicked();//导入程序文件
    void on_toolButton_exportfuncdesc_clicked();//导入功能说明
    void on_toolButton_exportimg_clicked();//上传预览画面
    void on_toolButton_save_clicked();//保存配置;
    void on_toolButton_cancel_clicked();//取消配置
signals:
    void signalRefreshList();
	void signalRefreshPlgunIni(const QString &);//更新编辑后的插件加载ini
private:
    Ui::TemplateRegisterForm ui;
    QSettings *P_templateIni_= Q_NULLPTR;//第一级ini获取当前操作界面的索引以及路径
    QSettings *P_TempInfoIni_ = Q_NULLPTR;//管理每一个模板路径
    QString  ini_key_path_;//当前操作ini key值
    bool isEdit_;//是否是编辑状态
    QString indexIniKey_;//当前模板界面索引
    QString title_;//标题
    QString currentTemplatePath_;//当前模板路径
};
