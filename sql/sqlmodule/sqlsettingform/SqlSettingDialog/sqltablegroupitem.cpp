#include "sqltablegroupitem.h"
#include "ui_sqltablegroupitem.h"
SqlTableGroupItem::SqlTableGroupItem(QWidget *parent)
    : QDialog(parent)
    ,ui(new Ui::SqlTableGroupItem)
{
    ui->setupUi(this);
    ui->groupBox_sqltable->setCheckable(true);
    connect(ui->groupBox_sqltable, SIGNAL(toggled(bool)), this, SLOT(SlotGroupChecked(bool)));

}

SqlTableGroupItem::~SqlTableGroupItem()
{
}
void SqlTableGroupItem::SetGroupBoxTitle(const QString &title)
{
    ui->groupBox_sqltable->setTitle(title);
}
void SqlTableGroupItem::SetIdItemList(const QStringList &listitem)
{
    int row;
    map_id_label_.clear();
    if (listitem.size() % 3 == 0) {
        row = listitem.size() / 3;
     }
    else
    {
        row = listitem.size() / 3+1;
    }
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < 3; j++) {
            if ((i * 3 + j) == listitem.size()) {
                return;
            }
            SqlIdItemLabel *label = new SqlIdItemLabel;
            label->SetCheckBoxText(listitem.at(i * 3 + j).split("|").at(0));
            label->SetLineeditPlaceholder(listitem.at(i * 3 + j).split("|").at(1));//linedit输入框提示信息
            label->SetCheckBoxChecked(ui->groupBox_sqltable->isChecked());
            ui->gridLayout_addsqlitem->addWidget(label, i, j, 1, 1);
            map_id_label_.insert(label->GetCheckedBoxText(), label);
        }
    }

    
}
void SqlTableGroupItem::SetGroupBoxChecked(bool checked)
{
    ui->groupBox_sqltable->setChecked(checked);
}
void SqlTableGroupItem::SlotGroupChecked(bool checked)
{
    
    QMap<QString, SqlIdItemLabel*>::iterator it;
    for (it = map_id_label_.begin(); it != map_id_label_.end(); it++) {
        it.value()->SetCheckBoxChecked(checked);
    }
   
}
void SqlTableGroupItem::GetLayoutItems(QLayout *layout, bool checked)
{
    QLayoutItem *child;
    while ((child = layout->takeAt(0)) != nullptr)
    {
        ///setParent为NULL，防止删除之后界面不消失
        if (child->widget())
        {
            child->widget()->setParent(nullptr);
           // qobject_cast<SqlIdItemLabel*>(child->widget())->SetCheckBoxChecked(checked);
        }
        else if (child->layout()) {
            GetLayoutItems(child->layout(), checked);
        }
        //child->widget()->setVisible(checked);
        qobject_cast<SqlIdItemLabel*>(child->widget())->SetCheckBoxChecked(checked);
    }
}