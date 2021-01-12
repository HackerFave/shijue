#include "yellowDirtItem.h"

yellowDirtItem::yellowDirtItem(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    ui.horizontalSlider_color_value->setMinimum(0);
    ui.horizontalSlider_color_value->setMaximum(255);
    connect(ui.horizontalSlider_color_value, SIGNAL(valueChanged(int)), this, SLOT(slotColorBtn(int)));
    //隐藏
    this->setVisible(false);
}

yellowDirtItem::~yellowDirtItem()
{
}
void yellowDirtItem::slotColorBtn(int colorValue)
{
    ui.toolButton_color->setStyleSheet(tr("border-radius:15px;background-color: rgb(255, 255, %1);").arg(255-colorValue));

    QPalette pal = ui.toolButton_color->palette();
    QBrush brush = pal.background();
    QColor col = brush.color();
    QString colorStr = tr("%1,%2,%3").arg(col.red()).arg(col.green()).arg(col.blue());
    m_selectedColor = col;
    emit signalLabelColor(m_selectedColor);
   // qreal r = color.split(",").at(0).toInt();
  //  qreal g = color.split(",").at(1).toInt();
   // qreal b = color.split(",").at(2).toInt();
    //colormap_[i + 1] = QColor(r, g, b);
}
void yellowDirtItem::on_toolButton_color_clicked()
{
    QPalette pal = ui.toolButton_color->palette();
    QBrush brush = pal.background();
    QColor col = brush.color();
    m_color = col;
    QColorDialog *m_pColor = new QColorDialog();
    m_pColor->setCurrentColor(m_color);
    m_pColor->setAttribute(Qt::WA_ShowModal);
    m_pColor->show();
    // connect(m_pColor, SIGNAL(currentColorChanged(QColor)), this, SLOT(slotShowColor(QColor)));//显示当前选中颜色的效果
    connect(m_pColor, SIGNAL(colorSelected(QColor)), this, SLOT(slotSetColor(QColor)));//OK信号连接
    //rejected信号会在QColorDialog框关闭或按Cancel按钮时发出，可通过绑定该信号来进行Cancel信号过滤
    connect(m_pColor, SIGNAL(rejected()), this, SLOT(slotSetColor_1()));
}
void yellowDirtItem::slotShowColor(QColor color)
{
    QPalette palette;
    palette = ui.toolButton_color->palette();
    palette.setBrush(QPalette::Window, QBrush(color));
    ui.toolButton_color->setAutoFillBackground(true);
    ui.toolButton_color->setPalette(palette);
}

void yellowDirtItem::slotSetColor(QColor color)
{
    if (color.isValid())
    {
        QPalette palette;
        palette = ui.toolButton_color->palette();
        palette.setBrush(QPalette::Window, QBrush(color));
        ui.toolButton_color->setAutoFillBackground(true);
        ui.toolButton_color->setPalette(palette);
        m_selectedColor = color;
        ui.toolButton_color->setStyleSheet(QString("border-radius:15px;background-color:rgb(%1,%2,%3);").arg(m_selectedColor.red())
            .arg(m_selectedColor.green())
            .arg(m_selectedColor.blue()));
        emit signalLabelColor(m_selectedColor);
    }
}

void yellowDirtItem::slotSetColor_1()
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
void yellowDirtItem::setLabelColor(QColor color)
{
    ui.toolButton_color->setStyleSheet(QString("border-radius:15px;background-color:rgb(%1,%2,%3);").arg(color.red())
        .arg(color.green())
        .arg(color.blue()));
    m_selectedColor = color;
    m_color = color;
    ui.horizontalSlider_color_value->setValue(color.blue());
    emit signalLabelColor(m_selectedColor);
}
QColor yellowDirtItem::getLabelColor()
{
    return m_selectedColor;
}