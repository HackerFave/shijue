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
    void set_label_temname(const QString &str);//ģ������
    void SetCurrentTemplateName(const QString &templatename) {//��������
        currentTemplateName_ = templatename;
    }
    void set_textEdit_info(const QString &str);//����
    void set_label_createtime(const QString &str);//����ʱ��
    void set_label_changetime(const QString &str);//�޸�ʱ��
    void SetCurrentDocPath(const QString &docpath) {
        currentDocPath_ = docpath;
    }
    void SetCurrentImgPath(const QString &imgpath) {//Ԥ��ͼ
        currentImgPath_ = imgpath;
    }
    void SetCurrentImgShow(const QString &imgpath);//����Ԥ������
    void SetCurrentTemplatePath(const QString &templatepath) {//ģ��·��
        currentTemplatePath_ = templatepath;
    }
    void SetCurrentTemplateAuthor(const QString &templateauthor) {//������
        currentTemplateAuthor_ = templateauthor;
    }
    void SetCurrentTemplateDemoFilePath(const QString &templatedemofilepath) {//�����ļ�·��
        currentTemplateDemoFilePath_ = templatedemofilepath;
    }
	void SetCheckedLoadStatus(bool checked);//��ǰ����Ƿ����
    //�����ļ���
    bool copyDirectoryFiles(const QString & fromDir, const QString & toDir, bool coverFileIfExist);
    bool copyFileToPath(QString sourceDir, QString toDir, bool coverFileIfExist);//�����ļ�
    QFileInfoList GetFileList(QString path);//�ع�����ļ���
	void setEnabledOthers(bool checked);
protected:
    void wheelEvent(QWheelEvent *event) override;
private slots:
    void on_toolButton_doc_clicked();//��˵���ĵ�
    void on_toolButton_export_clicked();//����
    void on_toolButton_edit_clicked();//�༭
    void on_toolButton_del_clicked();//ɾ��
    void slotRefreshListItemLabel();//ˢ�±༭�������
	void on_checkBox_load_clicked(bool checked);//�Ƿ���ز��
	void slotRefreshPlgunIni(const QString &);//ˢ�±༭�����Ĳ������ini
signals:
    void signalRefreshListItemLabel();//ˢ�±༭�������
private:
    Ui::TemplateItemLabelForm* ui;
    QString currentDocPath_;//˵���ĵ�
    QString currentImgPath_;//Ԥ��ͼ
    QString currentTemplatePath_;//��ǰģ��·��
    QString currentTemplateName_;//��ǰģ������
    QString currentTemplateDemoFilePath_;//��ǰģ������ļ�·��
    QString currentTemplateOper_;//��ǰģ������
    QString currentTemplateAuthor_;//��ǰģ�崴����
    TemplateRegisterForm *P_TemplateRegisterForm = Q_NULLPTR;//�༭����
    QString indexStr_;//ģ��ҳ����
    QString objectName_;//��ǰҳ������
    QSettings *P_templateIni_ = Q_NULLPTR;//��һ��ini��ȡ��ǰ��������������Լ�·��
    QSettings *P_TempInfoIni_ = Q_NULLPTR;//����ÿһ��ģ��·��
	QSettings * pluginTempIni_ = Q_NULLPTR;//���ز����ini
};
