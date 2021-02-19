#pragma once

#include <QWidget>
#include <vector>
#include <QCheckBox>
#include <QMap>
#include <QComboBox>
#include <QStandardItemModel>
#include <QFile>
#include "yaml-cpp/yaml.h"
#include "fileprojectmanager.h"
#include "ui_parameterConfigForm.h"
using namespace std;
class parameterConfigForm : public QWidget
{
    Q_OBJECT

public:
    parameterConfigForm(QWidget *parent = Q_NULLPTR);
    ~parameterConfigForm();
    void initUi();
    std::vector<QCheckBox*> getCheckedBoxList();
    void setCom_keeptraindataIndex(int value);
    void initComboBoxStyle(QMap<int, QComboBox *> comBoxMap);
    int getCom_defect_simulationCurrentIndex();
    int getcom_resize_modeCurrentIndex();
    int getcom_keeptraindataCurrentIndex();   
    QString getEdit_crop_size_wText();
    QString getedit_crop_size_hText();
    QString getedit_range_wText();
    QString getedit_range_hText();
    QString getedit_rotate_rangeText();
    QString getedit_brightText();
    QString getedit_contrastText();
    QString getedit_color_2Text();
    QString getedit_sharpText();
    QString getedit_epochText();
    QString getedit_train_ratioText();
    void setEditText(const QString &key, QString dataStr);
    void setCurrentIndex(const QString &key, int index);
    //����ѵ�������ļ�
    bool TrainCfgSave();
    //����yaml
    void SetYamlString(std::string arg1, std::string arg2, YAML::Node & config, QStringList& list);
    //����ѵ�������ļ�
    bool TrainCfgLoad();
    //��ȡyaml�ַ���
    QString GetYamlString(std::string arg1, std::string arg2, YAML::Node & config);
    //�޸�ѵ�������ļ��еĹ���Ŀ¼
    bool TrainCfgSetRootPath();
    //���ز��������ļ�
    void TestCfgLoad();
    //�޸Ĳ��������ļ��еĹ���Ŀ¼
    bool TestCfgSetRootPath();
    YAML::Node getTestconfig();
    YAML::Node getTrainconfig();
private slots:
    void on_btnParamDefault_clicked();
    void on_btnParamApply_clicked();
signals:
    void signal_btnParamDefault();//Ĭ��
    void signal_btnParamApply();//Ӧ��
private:
    Ui::parameterConfigForm ui;
    //�㷨�����еķ�תӳ���
    std::vector<QCheckBox*> transformlist_;
    QMap<int, QComboBox*> _mapComboBox;

    YAML::Node trainconfig_; // ѵ������
    YAML::Node testconfig_;//��������
};
