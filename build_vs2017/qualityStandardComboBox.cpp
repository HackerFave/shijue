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
    //修改某项文本
    for (int i = 0; i < ui.comboBox_item->count(); i++)
    {
        pItemModel->item(i)->setForeground(QColor(255, 255, 255));            //修改某项文本颜色
        pItemModel->item(i)->setBackground(QColor(90, 90, 90));    //修改某项背景颜色    
        pItemModel->item(i)->setSizeHint(QSize(180, 30));
        pItemModel->item(i)->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);    //修改某项文本对齐方式
    }
}