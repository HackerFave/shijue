#include "dataMatrixGroup.h"

dataMatrixGroup::dataMatrixGroup(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    initItem();

}

dataMatrixGroup::~dataMatrixGroup()
{
}
void dataMatrixGroup::initItem()
{
    _mapLabelItem.insert(0, ui.label_item1);
    _mapLabelItem.insert(1, ui.label_item2);
    _mapLabelItem.insert(2, ui.label_item3);
    _mapLabelItem.insert(3, ui.label_item4);
    _mapLabelItem.insert(4, ui.label_item5);
    _mapLabelItem.insert(5, ui.label_item6);
    _mapLabelItem.insert(6, ui.label_item7);
    _mapLabelItem.insert(7, ui.label_item8);

}
void dataMatrixGroup::initDataItem(QMap<int, int> map)
{
    for (int i = 0; i < _mapLabelItem.size(); ++i) {
        _mapLabelItem.value(i)->setText(QString::number(map.value(i)));
    }
}
void dataMatrixGroup::dataClear()
{
    for (size_t i = 0; i < _mapLabelItem.size(); i++)
    {
        _mapLabelItem.value(i)->clear();
    }
}