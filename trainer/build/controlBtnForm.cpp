#include "controlBtnForm.h"

controlBtnForm::controlBtnForm(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    connect(ui.btnHide, SIGNAL(clicked()), this, SLOT(slotbtnHide()));
    connect(ui.btnHideP, SIGNAL(clicked()), this, SLOT(slotbtnHideP()));
    connect(ui.btnTrain, SIGNAL(clicked()), this, SLOT(slotbtnTrain()));
    connect(ui.btnTest, SIGNAL(clicked()), this, SLOT(slotbtnTest()));
    connect(ui.btnConvert, SIGNAL(clicked()), this, SLOT(slotbtnConvert()));
    connect(ui.btnThreshold, SIGNAL(clicked()), this, SLOT(slotbtnThreshold()));
    initUi();

}

controlBtnForm::~controlBtnForm()
{
}
void controlBtnForm::initUi()
{
     _bistrainopen= false;
     _bistestopen = false;
    _btnGroup = new QButtonGroup;
    _btnGroup->addButton(ui.btnTrain, 0);//ѵ��
    _btnGroup->addButton(ui.btnTest, 1);//����
    _btnGroup->addButton(ui.btnConvert, 2);//����
    _btnGroup->addButton(ui.btnThreshold, 3);//Ӧ��

    _mapComboBox.insert(0, ui.comboBox);
    initComboBoxStyle(_mapComboBox);
}
/*��ע����*/
// �ո񰴼����������л���ע��ʾ/����,
void controlBtnForm::slotbtnHide()
{
    emit signalShowLabel();
}
// �ո񰴼����������л�Ԥ������ʾ/����,
void controlBtnForm::slotbtnHideP()
{
    emit signalShowPredict();
}
void controlBtnForm::slotbtnTrain()
{
    emit signalbtnTrain();
}
void controlBtnForm::slotbtnTest()
{
    emit signalbtnTest();
}
void controlBtnForm::slotbtnConvert()
{
    emit signalbtnConvert();
}
void controlBtnForm::slotbtnThreshold()
{
    emit signalbtnThreshold();
}
QString controlBtnForm::geteditThresholdText()
{
    return ui.editThreshold->text();
}
//�������ֵ��Сֵ
int controlBtnForm::getcomboBoxCurrentIndex()
{
    return ui.comboBox->currentIndex();
}
//��ť�û�
void controlBtnForm::btnGroupEnabled(int id, bool checked)
{
    for (size_t i = 0; i < _btnGroup->buttons().size(); i++)
    {
        if (id != i) {
            _btnGroup->button(i)->setEnabled(checked);
        }
    }
}
int controlBtnForm::getprogressBarValue()
{
    return ui.progressBar->value();
    
}
void controlBtnForm::setprogressBarValue(int value)
{
    ui.progressBar->setValue(value);
}
void controlBtnForm::setprogressBarMaxValue(int value)
{
    ui.progressBar->setMaximum(value);
}
void controlBtnForm::setbtnTestText(QString text)
{
    ui.btnTest->setText(text);
}
void controlBtnForm::setbtnTrainText(QString text)
{
    ui.btnTrain->setText(text);
}
float controlBtnForm::geteditThresholdValue()
{
    return ui.editThreshold->value();
}
void controlBtnForm::initComboBoxStyle(QMap<int, QComboBox *> comBoxMap)
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
