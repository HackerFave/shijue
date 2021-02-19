#pragma once

#include <QWidget>
#include <QButtonGroup>
#include <QComboBox>
#include <QMap>
#include <QStandardItemModel>
#include "ui_controlBtnForm.h"

class controlBtnForm : public QWidget
{
    Q_OBJECT

public:
    controlBtnForm(QWidget *parent = Q_NULLPTR);
    ~controlBtnForm();
    void initUi();
    QString geteditThresholdText();
    int getcomboBoxCurrentIndex();
    void btnGroupEnabled(int id, bool checked);
    int getprogressBarValue();
    void setprogressBarValue(int value);
    void setprogressBarMaxValue(int value);
    void setbtnTestText(QString text);
    void setbtnTrainText(QString text);
    void initComboBoxStyle(QMap<int, QComboBox *> comBoxMap);
    float geteditThresholdValue();
private slots:
    void slotbtnHide();
    void slotbtnHideP();

    void slotbtnTrain();//ѵ��
    void slotbtnTest();//����
    void slotbtnConvert();//����
    //��ֵ
    void slotbtnThreshold();
   
signals:
    void signalShowLabel();
    void signalShowPredict();  
    void signalbtnTrain();
    void signalbtnTest();
    void signalbtnConvert();
    void signalbtnThreshold();
private:
    Ui::controlBtnForm ui;
    QButtonGroup * _btnGroup = Q_NULLPTR;//��ť��
    QMap<int, QComboBox*> _mapComboBox;
    //ѵ����ť״̬
    bool _bistrainopen;
    //���԰�ť״̬
    bool _bistestopen;
};
