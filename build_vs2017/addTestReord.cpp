#include "addTestReord.h"
#include <QDesktopWidget>
addTestReord::addTestReord(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    P_baseWidget = new baseWidget;
    P_baseWidget->setWidget(this);
    P_baseWidget->setHintHeight(50);
    QDesktopWidget *pDesk = QApplication::desktop();
    P_baseWidget->move((pDesk->width() - this->width()) / 2, (pDesk->height() - this->height()) / 2);
    P_baseWidget->show();
    connect(P_baseWidget, SIGNAL(signalFormStatus(int, bool)), this, SLOT(slotFormStatus(int,bool)));
}

addTestReord::~addTestReord()
{
}
void addTestReord::on_toolButton_ok_clicked()
{
    QMap<QString, QString> data = getEditData();
    this->close();
    P_baseWidget->close();
    emit signalRowData(data,true);
}
void addTestReord::on_toolButton_cancel_clicked()
{
    QMap<QString, QString> data;
    this->close();
    P_baseWidget->close();
    emit signalRowData(data,false);
}
void addTestReord::slotFormStatus(int, bool)
{
    QMap<QString, QString> data;
    emit signalRowData(data, false);
}
QMap<QString, QString> addTestReord::getEditData()
{
    QMap<QString, QString> data;
    data.insert("meters", ui.lineEdit_meters->text().simplified());//米数
    data.insert("thickness", ui.lineEdit_houdu->text().simplified());//厚度
    data.insert("position", ui.lineEdit_weizhi->text().simplified());//位置
    data.insert("length", ui.lineEdit_length->text().simplified());//长度
    data.insert("description", ui.lineEdit_miaoshu->text().simplified());//描述
    data.insert("defect", ui.lineEdit_quexian->text().simplified());//缺陷
    return data;
}