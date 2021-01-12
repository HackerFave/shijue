#include "qualityStandardItem.h"

qualityStandardItem::qualityStandardItem(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
}

qualityStandardItem::~qualityStandardItem()
{
}
void qualityStandardItem::setTitleRange(int id, const QString &title, const QString &range)
{
    switch (id)
    {
    case 0: {
        ui.label_first_title->setText(title);
        ui.label_first_range->setText(range);
    }
            break;
    case 1: {
        ui.label_second_title->setText(title);
        ui.label_second_range->setText(range);
    }
            break;
    default:
        break;
    }
}
void qualityStandardItem::setVisibleFrame(int id, bool checked)
{
    switch (id)
    {
    case 0: {
        ui.frame_first->setVisible(checked);
    }
            break;
    case 1: {
        ui.frame_second->setVisible(checked);
    }
            break;
    default:
        break;
    }
}
void qualityStandardItem::setLabelFixSize(int id, QSize size)
{
    switch (id)
    {
    case 0: {
        ui.label_first_title->setFixedSize(size);
        ui.label_first_range->setFixedSize(QSize(80,60));
    }
            break;
    case 1: {
        ui.label_second_title->setFixedSize(size);
        ui.label_second_range->setFixedSize(size);
    }
            break;
    default:
        break;
    }
}
void qualityStandardItem::setSpinBoxValue(const QString & value1, const QString & value2)
{
    
    double value_1 = value1.toDouble();
    double value_2 = value2.toDouble();
    ui.spinBox_first->setValue(value_1);
    ui.spinBox_second->setValue(value_2);
}
double qualityStandardItem::getSpinBoxValue(int index)
{
    switch (index)
    {
    case 0: {
        return ui.spinBox_first->value();
    }
          //  break;
    case 1: {
        return ui.spinBox_second->value();
    }
           // break;
    default:
        break;
    }
}