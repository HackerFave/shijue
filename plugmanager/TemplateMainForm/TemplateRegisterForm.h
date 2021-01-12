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
    bool copyFileToPath(QString sourceDir, QString toDir, bool coverFileIfExist);//�����ļ�
    bool copyDirectoryFiles(const QString &fromDir, const QString &toDir, bool coverFileIfExist);//�����ļ���
    QString ReadIni(const QString &key);
    void SetCurrentTemIniKey(const QString &key) {
        ini_key_path_ = key;
    }
    void SetCurrentTemplatePath(const QString &templatepath) {//��ǰģ��·��
        currentTemplatePath_ = templatepath;
    }
    void ClearData();
    void set_lineEdit_name(const QString &str);//ģ������
    void set_textEdit_oper(const QString &str);//������Ϣ
    void set_lineEdit_demofile(const QString &str);//�����ļ�
    void set_lineEdit_functionDesc(const QString &str);//����˵��
    void set_lineEdit_previewimg(const QString &str);//Ԥ������
    void set_lineEdit_author(const QString &str);//������
private slots:
    void on_toolButton_exportdemofile_clicked();//��������ļ�
    void on_toolButton_exportfuncdesc_clicked();//���빦��˵��
    void on_toolButton_exportimg_clicked();//�ϴ�Ԥ������
    void on_toolButton_save_clicked();//��������;
    void on_toolButton_cancel_clicked();//ȡ������
signals:
    void signalRefreshList();
	void signalRefreshPlgunIni(const QString &);//���±༭��Ĳ������ini
private:
    Ui::TemplateRegisterForm ui;
    QSettings *P_templateIni_= Q_NULLPTR;//��һ��ini��ȡ��ǰ��������������Լ�·��
    QSettings *P_TempInfoIni_ = Q_NULLPTR;//����ÿһ��ģ��·��
    QString  ini_key_path_;//��ǰ����ini keyֵ
    bool isEdit_;//�Ƿ��Ǳ༭״̬
    QString indexIniKey_;//��ǰģ���������
    QString title_;//����
    QString currentTemplatePath_;//��ǰģ��·��
};
