#include "proCategoryItem.h"

proCategoryItem::proCategoryItem(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
}
proCategoryItem::~proCategoryItem()
{
}
QStringList proCategoryItem::getProType()
{
    QStringList list;
    if (ui.checkBox_ppbense->isChecked()) {
        list.push_back(ui.checkBox_ppbense->text().simplified());
    }
    if (ui.checkBox_ppBlack->isChecked()) {
        list.push_back(ui.checkBox_ppBlack->text().simplified());
    }
    if (ui.checkBox_pe->isChecked()) {
        list.push_back(ui.checkBox_pe->text().simplified());
    }
    list.push_back(ui.lineEdit_other->text().simplified());
    return list;
}