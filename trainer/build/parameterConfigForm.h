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
    //保存训练配置文件
    bool TrainCfgSave();
    //设置yaml
    void SetYamlString(std::string arg1, std::string arg2, YAML::Node & config, QStringList& list);
    //加载训练配置文件
    bool TrainCfgLoad();
    //获取yaml字符串
    QString GetYamlString(std::string arg1, std::string arg2, YAML::Node & config);
    //修改训练配置文件中的工作目录
    bool TrainCfgSetRootPath();
    //加载测试配置文件
    void TestCfgLoad();
    //修改测试配置文件中的工作目录
    bool TestCfgSetRootPath();
    YAML::Node getTestconfig();
    YAML::Node getTrainconfig();
private slots:
    void on_btnParamDefault_clicked();
    void on_btnParamApply_clicked();
signals:
    void signal_btnParamDefault();//默认
    void signal_btnParamApply();//应用
private:
    Ui::parameterConfigForm ui;
    //算法参数中的翻转映射表
    std::vector<QCheckBox*> transformlist_;
    QMap<int, QComboBox*> _mapComboBox;

    YAML::Node trainconfig_; // 训练配置
    YAML::Node testconfig_;//测试配置
};
