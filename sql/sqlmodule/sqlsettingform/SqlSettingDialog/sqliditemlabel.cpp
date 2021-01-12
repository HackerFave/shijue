#include "sqliditemlabel.h"
#include "ui_sqliditemlabel.h"

SqlIdItemLabel::SqlIdItemLabel(QWidget *parent)
    : QWidget(parent)
    ,ui(new Ui::SqlIdItemLabel)
{
    ui->setupUi(this);
    ui->label_title->setVisible(false);
}

SqlIdItemLabel::~SqlIdItemLabel()
{

}
void SqlIdItemLabel::SetCheckBoxChecked(bool checked)
{
    ui->checkBox_sqlid->setChecked(checked);
}
void SqlIdItemLabel::SetCheckBoxText(const QString &text)
{
    ui->checkBox_sqlid->setText(text);
}
bool SqlIdItemLabel::GetCheckBoxCheckedStatus()
{
    return ui->checkBox_sqlid->isChecked();
}
void SqlIdItemLabel::SetLineeditSqlData(const QString &data)
{
    ui->lineEdit_sqldata->setText(data);
}
void SqlIdItemLabel::SetLineeditPlaceholder(const QString &data)
{
    ui->lineEdit_sqldata->setPlaceholderText(data);
}
QString SqlIdItemLabel::GetLineeditSqlData()
{
    return ui->lineEdit_sqldata->text();
}
void SqlIdItemLabel::SetLabelSqlData(const QString &data)
{
    ui->label_title->setText(data);
}
QString SqlIdItemLabel::GetCheckedBoxText()
{
    return ui->checkBox_sqlid->text().simplified();
}