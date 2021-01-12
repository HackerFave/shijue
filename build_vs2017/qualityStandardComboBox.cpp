#include "qualityStandardComboBox.h"

qualityStandardComboBox::qualityStandardComboBox(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
}

qualityStandardComboBox::~qualityStandardComboBox()
{
}
void qualityStandardComboBox::addItemRow(const QString &str)
{
    ui.comboBox_item->addItem(str);
    QStandardItemModel *pItemModel = qobject_cast<QStandardItemModel*>(ui.comboBox_item->model());
    //�޸�ĳ���ı�
    for (int i = 0; i < ui.comboBox_item->count(); i++)
    {
        pItemModel->item(i)->setForeground(QColor(255, 255, 255));            //�޸�ĳ���ı���ɫ
        pItemModel->item(i)->setBackground(QColor(90, 90, 90));    //�޸�ĳ�����ɫ    
        pItemModel->item(i)->setSizeHint(QSize(180, 30));
        pItemModel->item(i)->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);    //�޸�ĳ���ı����뷽ʽ
    }
}