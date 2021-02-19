#include "parameterConfigForm.h"

parameterConfigForm::parameterConfigForm(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    initUi();
   
}

parameterConfigForm::~parameterConfigForm()
{
}
std::vector<QCheckBox*> parameterConfigForm::getCheckedBoxList()
{
    std::vector<QCheckBox*> list;
    list.push_back(ui.check_FLIP_LEFT_RIGHT);
    list.push_back(ui.check_FLIP_TOP_BOTTOM);
    list.push_back(ui.check_ROTATE_90);
    list.push_back(ui.check_ROTATE_180);
    list.push_back(ui.check_ROTATE_270);
    list.push_back(ui.check_TRANSPOSE);
    list.push_back(ui.check_TRANSVERSE);
    return list;
}
void parameterConfigForm::setCom_keeptraindataIndex(int value) 
{
    ui.com_keeptraindata->setCurrentIndex(value);
}
void parameterConfigForm::initComboBoxStyle(QMap<int, QComboBox *> comBoxMap)
{
    for (size_t j = 0; j < comBoxMap.size(); j++)
    {
        QStandardItemModel *pItemModel = qobject_cast<QStandardItemModel*>(comBoxMap.value(j)->model());
        //�޸�ĳ���ı�
        for (int i = 0; i < comBoxMap.value(j)->count(); i++)
        {
            pItemModel->item(i)->setForeground(QColor(255, 255, 255));            //�޸�ĳ���ı���ɫ
            pItemModel->item(i)->setBackground(QColor(90, 90, 90));    //�޸�ĳ�����ɫ    
            pItemModel->item(i)->setSizeHint(QSize(180, 30));
            pItemModel->item(i)->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);    //�޸�ĳ���ı����뷽ʽ
        }
    }

}
void parameterConfigForm::initUi()
{
    _mapComboBox.insert(0, ui.com_mode_supervised);
    _mapComboBox.insert(1, ui.com_defect_simulation);
    _mapComboBox.insert(2, ui.com_color);
    _mapComboBox.insert(3, ui.com_crop_mode);
    _mapComboBox.insert(4, ui.com_resize_mode);
    _mapComboBox.insert(5, ui.com_keeptraindata);
    initComboBoxStyle(_mapComboBox);
    transformlist_ = getCheckedBoxList();
}
int parameterConfigForm::getCom_defect_simulationCurrentIndex()
{
    return ui.com_defect_simulation->currentIndex();
}
int parameterConfigForm::getcom_resize_modeCurrentIndex()
{
    return ui.com_resize_mode->currentIndex();
}
int parameterConfigForm::getcom_keeptraindataCurrentIndex()
{
    return ui.com_keeptraindata->currentIndex();
}
QString parameterConfigForm::getEdit_crop_size_wText()
{
    return ui.edit_crop_size_w->text();
}
QString parameterConfigForm::getedit_crop_size_hText()
{
    return ui.edit_crop_size_h->text();
}
QString parameterConfigForm::getedit_range_wText()
{
    return ui.edit_range_w->text();
}
QString parameterConfigForm::getedit_range_hText()
{
    return ui.edit_range_h->text();
}
QString parameterConfigForm::getedit_rotate_rangeText()
{
    return ui.edit_rotate_range->text();
}
QString parameterConfigForm::getedit_brightText()
{
    return ui.edit_bright->text();
}
QString parameterConfigForm::getedit_contrastText()
{
    return ui.edit_contrast->text();
}
QString parameterConfigForm::getedit_color_2Text()
{
    return ui.edit_color_2->text();
}
QString parameterConfigForm::getedit_sharpText()
{
    return ui.edit_sharp->text();
}
QString parameterConfigForm::getedit_epochText()
{
    return ui.edit_epoch->text();
}
QString parameterConfigForm::getedit_train_ratioText()
{
    return ui.edit_train_ratio->text();
}
void parameterConfigForm::setEditText(const QString &key, QString dataStr)
{
    QString keyStr = key.simplified();
    if (keyStr == "edit_crop_size_w")
    {
        ui.edit_crop_size_w->setText(dataStr);
    }
    else if(keyStr == "edit_crop_size_h")
    {
        ui.edit_crop_size_h->setText(dataStr);
    }
    else if (keyStr == "edit_range_w")
    {
        ui.edit_range_w->setText(dataStr);
    }
    else if (keyStr == "edit_range_h")
    {
        ui.edit_range_h->setText(dataStr);
    }
    else if (keyStr == "edit_rotate_range")
    {
        ui.edit_rotate_range->setText(dataStr);
    }
    else if (keyStr == "edit_bright")
    {
        ui.edit_bright->setText(dataStr);
    }
    else if (keyStr == "edit_color_2")
    {
        ui.edit_color_2->setText(dataStr);
    }
    else if (keyStr == "edit_sharp")
    {
        ui.edit_sharp->setText(dataStr);
    }
    else if (keyStr == "edit_epoch")
    {
        ui.edit_epoch->setText(dataStr);
    }
    else if(keyStr=="edit_train_ratio")
    {
       
    }
}
void parameterConfigForm::setCurrentIndex(const QString &key, int index)
{
    QString keyStr = key.simplified();
    if (keyStr == "com_defect_simulation") {
        ui.com_defect_simulation->setCurrentIndex(index);
    }
    else if (keyStr == "com_resize_mode")
    {
        ui.com_resize_mode->setCurrentIndex(index);
    }
   
}
/*�㷨����*/
//�㷨������Ӧ�á���ť������ ��Ӧ�ã�
void parameterConfigForm::on_btnParamApply_clicked()
{
    emit signal_btnParamApply();
    TrainCfgSave();
}
//Ĭ��
void parameterConfigForm::on_btnParamDefault_clicked()
{
    emit signal_btnParamDefault();
    FileProjectManager::Instance()->ResetTrainConfig();
    TrainCfgLoad();
    TrainCfgSetRootPath();
}
//����ѵ�������ļ�
bool parameterConfigForm::TrainCfgSave()
{
    QString yamlfile = FileProjectManager::Instance()->ProjectGetTrainCfgAbsPath();
    QFile file(yamlfile);
    if (!file.exists())
    {
        //������
        return false;
    }
    std::string fs = yamlfile.toLocal8Bit();
    YAML::Node config = YAML::LoadFile(fs);
    QString data;
    QStringList datalist;

    config["DATASETS"]["DEFECTSIMULATE"]["ENABLE"] = bool(getCom_defect_simulationCurrentIndex());

    //�ü�GetYamlString("AUGMENT", "CROP_SIZE", config)

	datalist.append(getedit_crop_size_hText());
	datalist.append(getEdit_crop_size_wText());
    SetYamlString("AUGMENT", "CROP_SIZE", config, datalist);


    //��ת
    for (int i = 0, index = 0; i < transformlist_.size(); i++)
    {
        if (transformlist_[i]->isChecked())
        {
            config["DATASETS"]["AUGMENT"]["TRANSPOSE"][index++] = i;
        }
    }

    //����
    int ydata = getcom_resize_modeCurrentIndex();
    config["DATASETS"]["AUGMENT"]["RESIZE_MODE"] = ydata;

    //��
    data = getedit_range_wText();
    datalist = data.split(",");
    SetYamlString("AUGMENT", "RESIZE_RANGE_W", config, datalist);


    //��
    data = getedit_range_hText();
    datalist = data.split(",");
    SetYamlString("AUGMENT", "RESIZE_RANGE_H", config, datalist);


    //��ת
    data = getedit_rotate_rangeText();
    datalist = data.split(",");
    SetYamlString("AUGMENT", "ROTATE_RANGE", config, datalist);


    //����
    data = getedit_brightText();
    datalist = data.split(",");
    SetYamlString("AUGMENT", "BRIGHT_RANGE", config, datalist);



    //�Աȶ�
    data = getedit_contrastText();
    datalist = data.split(",");
    SetYamlString("AUGMENT", "CONTRAST_RANGE", config, datalist);


    //���Ͷ�
    data = getedit_color_2Text();
    datalist = data.split(",");
    SetYamlString("AUGMENT", "COLOR_RANGE", config, datalist);

    //���
    data = getedit_sharpText();
    datalist = data.split(",");
    SetYamlString("AUGMENT", "SHARP_RANGE", config, datalist);

    //END_EPOCH
    config["TRAIN"]["END_EPOCH"] = getedit_epochText().toInt();

    trainconfig_ = config;
    std::ofstream fout(fs);
    fout << config;
    fout.close();
    return true;
}
//����yaml
void parameterConfigForm::SetYamlString(std::string arg1, std::string arg2, YAML::Node &config, QStringList& list)
{
    for (int i = 0; i < list.count(); i++)
    {
        config["DATASETS"][arg1][arg2][i] = std::string(list.at(i).toLocal8Bit());
    }
}
//����ѵ�������ļ���ѵ��������
bool parameterConfigForm::TrainCfgLoad()
{
    QString yamlfile = FileProjectManager::Instance()->ProjectGetTrainCfgAbsPath();
    QFile file(yamlfile);
    if (!file.exists()) {
        //������
        return false;
    }
    std::string fs = yamlfile.toLocal8Bit();
    //UI����
    YAML::Node config = YAML::LoadFile(fs);
    trainconfig_ = config;

    //�ü�
    if (config["DATASETS"]["AUGMENT"]["CROP_SIZE"].size() > 1) {
        ui.edit_crop_size_w->setText(QString::fromStdString(config["DATASETS"]["AUGMENT"]["CROP_SIZE"][0].as<std::string>()));
        ui.edit_crop_size_h->setText( QString::fromStdString(config["DATASETS"]["AUGMENT"]["CROP_SIZE"][1].as<std::string>()));
    }

    for (auto it = config["DATASETS"]["AUGMENT"]["TRANSPOSE"].begin(); it != config["DATASETS"]["AUGMENT"]["TRANSPOSE"].end(); it++) {
        transformlist_[it->as<int>()]->setChecked(true);
    }

    ui.com_defect_simulation->setCurrentIndex(config["DATASETS"]["DEFECTSIMULATE"]["ENABLE"].as<bool>());

    //����
    int ydata;
    ydata = config["DATASETS"]["AUGMENT"]["RESIZE_MODE"].as<int>();
    ui.com_resize_mode->setCurrentIndex(ydata);

    //��
    ui.edit_range_w->setText(GetYamlString("AUGMENT", "RESIZE_RANGE_W", config));

    //��
    ui.edit_range_h->setText( GetYamlString("AUGMENT", "RESIZE_RANGE_H", config));

    //��ת
    ui.edit_rotate_range->setText( GetYamlString("AUGMENT", "ROTATE_RANGE", config));

    //����
    ui.edit_bright->setText(GetYamlString("AUGMENT", "BRIGHT_RANGE", config));

    //�Աȶ�
    ui.edit_contrast->setText( GetYamlString("AUGMENT", "CONTRAST_RANGE", config));

    //���Ͷ�
    ui.edit_color_2->setText( GetYamlString("AUGMENT", "COLOR_RANGE", config));

    //���
    ui.edit_sharp->setText(GetYamlString("AUGMENT", "SHARP_RANGE", config));

    //END_EPOCH
    ui.edit_epoch->setText(QString::number(config["TRAIN"]["END_EPOCH"].as<int>()));

    //RATIO

    return true;
}
//��ȡyaml�ַ���
QString parameterConfigForm::GetYamlString(std::string arg1, std::string arg2, YAML::Node & config)
{
    QString cdata;

    for (int i = 0; i < config["DATASETS"][arg1][arg2].size(); i++)
    {
        cdata.append(QString::fromStdString(config["DATASETS"][arg1][arg2][i].as<std::string>()));
        cdata.append(",");
    }
    cdata.chop(1);
    return cdata;
}
//�޸�ѵ�������ļ��еĹ���Ŀ¼
bool parameterConfigForm::TrainCfgSetRootPath()
{
    QString yamlfile = FileProjectManager::Instance()->ProjectGetTrainCfgAbsPath();
    QFile file(yamlfile);
    if (!file.exists())
    {
        //������
        return false;
    }
    std::string fs = yamlfile.toLocal8Bit();
    YAML::Node config = YAML::LoadFile(fs);
    QString data;
    QStringList datalist;

    config["DATASETS"]["ROOT"] = std::string(FileProjectManager::Instance()->ProjectGetTrainAbsPath().toLocal8Bit());


    //�ж��Ƿ����Ԥѵ��ģ��
    QString pretrainmodel;
    pretrainmodel = QString("%1/%2").arg(FileProjectManager::Instance()->ProjectGetTrainAbsPath()).arg(QString("F1_best_model.pth.tar"));
    file.setFileName(pretrainmodel);
    if (file.exists()) {
        config["TRAIN"]["FT"]["RESUME"] = true;
        config["TRAIN"]["FT"]["CHECKPATH"] = std::string(pretrainmodel.toLocal8Bit());
    }
    else {
        pretrainmodel = QString("%1/%2").arg(QCoreApplication::applicationDirPath()).arg(QString("F1_best_model.pth.tar"));
        file.setFileName(pretrainmodel);
        if (file.exists()) {
            config["TRAIN"]["FT"]["RESUME"] = true;
            config["TRAIN"]["FT"]["CHECKPATH"] = std::string(pretrainmodel.toLocal8Bit());
        }
        else {
            config["TRAIN"]["FT"]["RESUME"] = false;
        }
    }

    std::ofstream fout(fs);
    fout << config;
    fout.close();
    return true;
}
void parameterConfigForm::TestCfgLoad() {
    QString yamlfile = FileProjectManager::Instance()->ProjectGetTestCfgAbsPath();
    QFile file(yamlfile);
    if (!file.exists()) {
        //������
        return;
    }
    std::string fs = yamlfile.toLocal8Bit();
    testconfig_ = YAML::LoadFile(fs);
}

bool parameterConfigForm::TestCfgSetRootPath()
{
    QString yamlfile = FileProjectManager::Instance()->ProjectGetTestCfgAbsPath();
    QFile file(yamlfile);
    if (!file.exists())
    {
        //������
        return false;
    }
    std::string fs = yamlfile.toLocal8Bit();
    YAML::Node config = YAML::LoadFile(fs);
    QString data;
    QStringList datalist;

    config["TEST"]["ROOT"] = std::string(FileProjectManager::Instance()->ProjectGetTrainAbsPath().toLocal8Bit());

    std::ofstream fout(fs);
    fout << config;
    fout.close();
    return true;
}
YAML::Node parameterConfigForm::getTestconfig()
{
    return testconfig_;
}
YAML::Node parameterConfigForm::getTrainconfig()
{
    return trainconfig_;
}