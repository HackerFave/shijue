#include "tableViewEditItemForm.h"

tableViewEditItemForm::tableViewEditItemForm(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    
    P_baseWidget = new baseWidget;
    P_baseWidget->setWidget(this);
    P_baseWidget->setHintHeight(50);
    P_baseWidget->show();
    connect(P_baseWidget, SIGNAL(signalFormStatus(int, bool)), this, SLOT(slotFormStatus(int, bool)));
}

tableViewEditItemForm::~tableViewEditItemForm()
{
   
}
void tableViewEditItemForm::setTitle(const QString &text, const int &row, const int &column)
{
    ui.label_title->setText(text);
    _row = row;
    _column = column;
}
void tableViewEditItemForm::on_toolButton_ok_clicked()
{
    this->close();
    P_baseWidget->close();
    emit signalItemData(_row, _column, ui.lineEdit_item->text());     
}
void tableViewEditItemForm::on_toolButton_cancel_clicked()
{
    this->close();
    P_baseWidget->close();
    emit signalItemData(_row, _column, "");
}
void tableViewEditItemForm::slotFormStatus(int, bool)
{
    emit signalItemData(_row, _column, "");
}