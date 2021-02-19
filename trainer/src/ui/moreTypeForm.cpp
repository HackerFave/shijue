#include "moreTypeForm.h"
#include "publicClass.h"
#include <QColorDialog>

moreTypeForm::moreTypeForm(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    ui.toolButton_changeColor->setIconSize(QSize(40, 30));
    //ui.toolButton_changeColor->setIcon(QIcon(tr("%1/icons/color.png").arg(publicClass::instance()->getAppPath())));
    ui.toolButton_changeColor->setToolTip(tr("修改类别颜色"));
    //QPalette p = ui.label_color->palette();
    //QBrush brush = p.background();
      //brush.color();
}

moreTypeForm::~moreTypeForm()
{
}
void moreTypeForm::on_toolButton_changeColor_clicked()
{
    QColorDialog *m_pColor = new QColorDialog();
    m_pColor->setCurrentColor(m_color);
    m_pColor->setAttribute(Qt::WA_ShowModal);
    m_pColor->show();
   // connect(m_pColor, SIGNAL(currentColorChanged(QColor)), this, SLOT(slotShowColor(QColor)));//显示当前选中颜色的效果
    connect(m_pColor, SIGNAL(colorSelected(QColor)), this, SLOT(slotSetColor(QColor)));//OK信号连接
    //rejected信号会在QColorDialog框关闭或按Cancel按钮时发出，可通过绑定该信号来进行Cancel信号过滤
    connect(m_pColor, SIGNAL(rejected()), this, SLOT(slotSetColor_1()));
}
void moreTypeForm::slotShowColor(QColor color)
{
    QPalette palette;
    palette = ui.label_color->palette();
    palette.setBrush(QPalette::Window, QBrush(color));
    ui.label_color->setAutoFillBackground(true);
    ui.label_color->setPalette(palette);
}

void moreTypeForm::slotSetColor(QColor color)
{
    if (color.isValid())
    {
        qDebug() << "SetColor";
        QPalette palette;
        palette = ui.label_color->palette();
        palette.setBrush(QPalette::Window, QBrush(color));
        ui.label_color->setAutoFillBackground(true);
        ui.label_color->setPalette(palette);
        m_selectedColor = color;
        ui.label_color->setStyleSheet(QString("border-radius:10px;background-color:rgb(%1,%2,%3);").arg(m_selectedColor.red())
            .arg(m_selectedColor.green())
            .arg(m_selectedColor.blue()));
        emit signalLabelColor(m_selectedColor);
    }
}

void moreTypeForm::slotSetColor_1()
{

    if (m_selectedColor.isValid())
    {
      /*  qDebug() << "SetColor_1";
        QPalette palette;
        palette = ui.label_color->palette();
        palette.setBrush(QPalette::Window, QBrush(m_selectedColor));
        ui.label_color->setAutoFillBackground(true);
        ui.label_color->setPalette(palette);
        m_color = m_selectedColor;
        ui.label_color->setStyleSheet(QString("border-radius:10px;background-color:QColor(%1,%2,%3);").arg(m_color.red())
              .arg(m_color.green())
              .arg(m_color.blue()));*/
    }
    else
    {
        /*qDebug() << "SetColor_2";
        QPalette palette;
        palette = ui.label_color->palette();
        palette.setBrush(QPalette::Window, QBrush(m_color));
        ui.label_color->setAutoFillBackground(true);
        ui.label_color->setPalette(palette);
        ui.label_color->setStyleSheet(QString("border-radius:10px;background-color:QColor(%1,%2,%3);").arg(m_color.red())
              .arg(m_color.green())
              .arg(m_color.blue()));*/
    }
}
void moreTypeForm::setLabelColor(QColor color)
{
    ui.label_color->setStyleSheet(QString("border-radius:10px;background-color:rgb(%1,%2,%3);").arg(color.red())
        .arg(color.green())
        .arg(color.blue()));
    m_selectedColor = color;
    m_color = color;
    emit signalLabelColor(m_selectedColor);
}
QColor moreTypeForm::getLabelColor()
{
    return m_selectedColor;
}
bool moreTypeForm::getChecked()
{
    bool checked = ui.checkBox->isChecked();
    return checked;
}
QString moreTypeForm::getCheckeBoxStr()
{
    QString str = ui.checkBox->text().simplified();
    return str;
}
void moreTypeForm::setChecked(bool checked)
{
    ui.checkBox->setChecked(checked);
}
void moreTypeForm::setCheckBoxStr(const QString &str)
{
    ui.checkBox->setText(str);
}
void moreTypeForm::setLineEditTypeStr(const QString &str)
{
    ui.lineEdit_name->setText(str.simplified());
}
void moreTypeForm::getTypeStr()
{
    _TypeStr = ui.lineEdit_name->text().simplified();
}
QString moreTypeForm::getTypeText()
{
    getTypeStr();
    return _TypeStr;
}