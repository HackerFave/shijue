#include "btnDrogForm.h"
#include "publicClass.h"
#include <QDesktopWidget>
#include <QVBoxLayout >
#include <qgridlayout.h>
#include <QPushButton>
#include <QMenu>
btnDrogForm::btnDrogForm(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    ui.spinBox->setFocusPolicy(Qt::NoFocus);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Widget| Qt::WindowStaysOnTopHint);
    this->show();
    QDesktopWidget *deskdop = QApplication::desktop();
    this->move((deskdop->width() - this->width()*11) , (deskdop->height() - this->height()-50));

    ui.toolButton_min->setIcon(QIcon(tr("%1/icons/min.png").arg(publicClass::instance()->getAppPath())));
    ui.toolButton_close->setIcon(QIcon(tr("%1/icons/closeW.png").arg(publicClass::instance()->getAppPath())));

    ui.btnMouse->setIconSize(QSize(40, 40));
    ui.btnMouse->setIcon(QIcon(tr("%1/icons/sel.png").arg(publicClass::instance()->getAppPath())));

    ui.btnDrawLine->setIconSize(QSize(40, 40));
    ui.btnDrawLine->setIcon(QIcon(tr("%1/icons/pen.png").arg(publicClass::instance()->getAppPath())));

    ui.btnEras->setIconSize(QSize(40, 40));
    ui.btnEras->setIcon(QIcon(tr("%1/icons/btnEras.png").arg(publicClass::instance()->getAppPath())));

    ui.btnClear->setIconSize(QSize(40, 40));
    ui.btnClear->setIcon(QIcon(tr("%1/icons/clear.png").arg(publicClass::instance()->getAppPath())));
    ui.btnApply->setIconSize(QSize(40, 40));
    ui.btnApply->setIcon(QIcon(tr("%1/icons/btnApply.png").arg(publicClass::instance()->getAppPath())));

    ui.toolButton_menu->setIconSize(QSize(40, 40));
    ui.toolButton_menu->setIcon(QIcon(tr("%1/icons/moretype.png").arg(publicClass::instance()->getAppPath())));
   // ui.toolButton_color->setIconSize(QSize(40, 40));
    //ui.toolButton_color->setIcon(QIcon(tr("%1/icons/color.png").arg(publicClass::instance()->getAppPath())));
    connect(ui.toolButton_min, SIGNAL(clicked()), this, SLOT(showMinimized()));
    connect(ui.toolButton_close, SIGNAL(clicked()), this, SLOT(slotClose()));
    connect(ui.spinBox, SIGNAL(valueChanged(int)), this, SLOT(slotValueChange(int)));
    ui.line->setFixedHeight(3);
    this->setWindowOpacity(1);
   // this->setAttribute(Qt::WA_TranslucentBackground);
    this->setFixedWidth(70);
    ui.frame_2->setVisible(false);
    _btnGroup = new QButtonGroup(this);
    _btnGroup->addButton(ui.btnMouse, 0);
    _btnGroup->addButton(ui.btnDrawLine, 1);
    _btnGroup->addButton(ui.btnEras, 2);
    _btnGroup->addButton(ui.btnClear, 3);//清除
    _btnGroup->addButton(ui.btnApply, 4);//应用
    _btnGroup->setExclusive(true);
    QPalette p = ui.line->palette();
    QBrush brush = p.background();
    m_color = brush.color();
    _publicClass = publicClass::instance();
}

btnDrogForm::~btnDrogForm()
{
}
void btnDrogForm::mousePressEvent(QMouseEvent *event)
{
    if (ui.frame->geometry().contains(this->mapFromGlobal(QCursor::pos()))) {
        m_lastPos = event->globalPos();
        isPressedWidget = true; // 当前鼠标按下的即是QWidget而非界面上布局的其它控件

        
    }
    if (event->button() == Qt::RightButton) {

    }

}

void btnDrogForm::mouseMoveEvent(QMouseEvent *event)
{
    if (isPressedWidget) {
        this->move(this->x() + (event->globalX() - m_lastPos.x()),
            this->y() + (event->globalY() - m_lastPos.y()));
        m_lastPos = event->globalPos();
    }
}
void btnDrogForm::mouseReleaseEvent(QMouseEvent *event)
{
    // 其实这里的mouseReleaseEvent函数可以不用重写
    m_lastPos = event->globalPos();
    isPressedWidget = false; // 鼠标松开时，置为false
}
void btnDrogForm::on_btnMouse_clicked()
{
    emit signalBtnId(1);
    for (int i = 0; i < _btnGroup->buttons().size(); i++) {
        if (i != 0) {
            clearStyle(dynamic_cast<QToolButton*>(_btnGroup->buttons().at(i)));
        }
    }
}
void btnDrogForm::on_btnDrawLine_clicked()
{
    emit signalBtnId(2);
    for (int i = 0; i < _btnGroup->buttons().size(); i++) {
        if (i != 1) {
            clearStyle(dynamic_cast<QToolButton*>(_btnGroup->buttons().at(i)));
        }
    }
}
void btnDrogForm::on_btnEras_clicked()
{
    emit signalBtnId(3);
    for (int i = 0; i < _btnGroup->buttons().size(); i++) {
        if (i != 2) {
            clearStyle(dynamic_cast<QToolButton*>(_btnGroup->buttons().at(i)));
        }
    }
}
void btnDrogForm::on_btnClear_clicked()
{
    emit signalBtnId(4);
    for (int i = 0; i < _btnGroup->buttons().size(); i++) {
        if (i != 3) {
            clearStyle(dynamic_cast<QToolButton*>(_btnGroup->buttons().at(i)));
        }
    }
}
void btnDrogForm::on_btnApply_clicked()
{
    emit signalBtnId(5);
    for (int i = 0; i < _btnGroup->buttons().size(); i++) {
        if (i != 4) {
            clearStyle(dynamic_cast<QToolButton*>(_btnGroup->buttons().at(i)));
        }
    }
}
void btnDrogForm::slotValueChange(int value)
{
    ui.line->setFixedHeight(value);
    emit signalValue(value);
}
void btnDrogForm::slotClose()
{
    this->setVisible(false);
}
void btnDrogForm::clearStyle(QToolButton *btn)
{
   /* btn->setStyleSheet("QToolButton{color: rgb(255, 255, 255);border - radius:2px;background:transparent;border: 1px solid rgb(111, 111, 111);}"
        "QToolButton:checked{background - color: rgb(111, 111, 111);}"
        "QToolButton:hover{background - color:rgb(111, 111, 111);}");*/
    //
    //btn->styleSheet().clear();
    btn->setChecked(false);

}
void btnDrogForm::setBtnChecked()
{
    for (int i = 0; i < _btnGroup->buttons().size(); i++){ 

        //dynamic_cast<QToolButton*>(_btnGroup->buttons().at(i))->setCheckable(true);
        if (dynamic_cast<QToolButton*>(_btnGroup->buttons().at(i))->isChecked()) {
            dynamic_cast<QToolButton*>(_btnGroup->buttons().at(i))->setChecked(false);
        }      
    }
}
int btnDrogForm::getCursorSize()
{
    return ui.spinBox->value();
}
bool btnDrogForm::getBtnChecked(int index)
{
    switch (index)
    {
    case 0: {
        return ui.btnDrawLine->isChecked();
    }
    case 1: {
        return ui.btnEras->isChecked();
    }
    default:
        return false;
    }
}
void btnDrogForm::on_toolButton_color_clicked()
{
    QColorDialog *m_pColor = new QColorDialog();
    m_pColor->setCurrentColor(m_color);
    m_pColor->show();
    connect(m_pColor, SIGNAL(currentColorChanged(QColor)), this, SLOT(slotShowColor(QColor)));//显示当前选中颜色的效果
    connect(m_pColor, SIGNAL(colorSelected(QColor)), this, SLOT(slotSetColor(QColor)));//OK信号连接
    //rejected信号会在QColorDialog框关闭或按Cancel按钮时发出，可通过绑定该信号来进行Cancel信号过滤
    connect(m_pColor, SIGNAL(rejected()), this, SLOT(slotSetColor_1()));
}
void btnDrogForm::slotShowColor(QColor color)
{
    QPalette palette;
    palette = ui.line->palette();
    palette.setBrush(QPalette::Window, QBrush(color));
    ui.line->setAutoFillBackground(true);
    ui.line->setPalette(palette);
}

void btnDrogForm::slotSetColor(QColor color)
{
    if (color.isValid())
    {
        qDebug() << "SetColor";
        QPalette palette;
        palette = ui.line->palette();
        palette.setBrush(QPalette::Window, QBrush(color));
        ui.line->setAutoFillBackground(true);
        ui.line->setPalette(palette);
        m_selectedColor = color;
        ui.line->setStyleSheet(QString("background-color:rgb(%1,%2,%3);").arg(m_selectedColor.red())
            .arg(m_selectedColor.green())
            .arg(m_selectedColor.blue()));
        emit signalLineColor(m_selectedColor);
    }
}

void btnDrogForm::slotSetColor_1()
{

    if (m_selectedColor.isValid())
    {
        qDebug() << "SetColor_1";
        QPalette palette;
        palette = ui.line->palette();
        palette.setBrush(QPalette::Window, QBrush(m_selectedColor));
        ui.line->setAutoFillBackground(true);
        ui.line->setPalette(palette);
        m_color = m_selectedColor;
      /*  ui.line->setStyleSheet(QString("background-color:QColor(%1,%2,%3);").arg(m_color.red())
            .arg(m_color.green())
            .arg(m_color.blue()));*/
    }
    else
    {
        qDebug() << "SetColor_2";
        QPalette palette;
        palette = ui.line->palette();
        palette.setBrush(QPalette::Window, QBrush(m_color));
        ui.line->setAutoFillBackground(true);
        ui.line->setPalette(palette);
      /*  ui.line->setStyleSheet(QString("background-color:QColor(%1,%2,%3);").arg(m_color.red())
            .arg(m_color.green())
            .arg(m_color.blue()));*/
    }
}
QString btnDrogForm::converRGB2HexStr(QColor _rgb)

{

    QString redStr = QString("%1").arg(_rgb.red(), 2, 16, QChar('0'));

    QString greenStr = QString("%1").arg(_rgb.green(), 2, 16, QChar('0'));

    QString blueStr = QString("%1").arg(_rgb.blue(), 2, 16, QChar('0'));

    QString hexStr = "#" + redStr + greenStr ;

    return hexStr;

}
void btnDrogForm::addMoreTypeAction(const QList<QVariantList> & listType)
{
    if (_menu) {
        _menu->deleteLater();
    }
    _menu = new QMenu();

    _menu->setStyleSheet("QMenu::item:selected{background-color:rgb(89,87,87);border: 0px solid rgb(235,110,36);}");
    for (size_t i = 0; i < listType.size(); i++)
    {
        QAction * action = new QAction(tr("%1:%2").arg(listType.at(i).at(2).toString()).arg(listType.at(i).at(0).toString()));
        QPixmap pixmap(tr("%1/icons/type.png").arg(publicClass::instance()->getAppPath()));
       // QPixmap scaledPixmap = pixmap.scaled(btnDrog->getCursorSize() + 2, btnDrog->getCursorSize() + 2, Qt::KeepAspectRatio);
        QStringList list = publicClass::instance()->hexToRgb(listType.at(i).at(1).toString());
        qreal r;
        qreal g;
        qreal b;
        if (!list.isEmpty()) {
             r = list.at(0).toInt();
             g = list.at(1).toInt();
             b = list.at(2).toInt();
             pixmap.fill(QColor(r,g,b));
        }
        action->setIcon(QIcon(pixmap));
        
        _menu->addAction(action);
        _menu->addSeparator();
        action->setCheckable(true);
        action->setChecked(false);
       // if (i == 0) {
         //   onMenuTriggered(action);
        //}
       
    }
    ui.toolButton_menu->setToolButtonStyle(Qt::ToolButtonIconOnly);
    ui.toolButton_menu->setPopupMode(QToolButton::InstantPopup);
    ui.toolButton_menu->setMenu(_menu);
  
   connect(_menu, SIGNAL(triggered(QAction*)), this, SLOT(onMenuTriggered(QAction*)));
}
void btnDrogForm::on_toolButton_menu_clicked()
{

   
}
void btnDrogForm::onMenuTriggered(QAction* action)
{
   
    for (int i = 0; i < _menu->actions().size(); i++) {
        _menu->actions().at(i)->setChecked(false);
    }
    //qDebug() << _listData.at(action->text().simplified().split(":").at(1).toInt());
    QString color = _publicClass->readIniFile(QString::number(action->text().simplified().split(":").at(1).toInt())).toString().split(":").at(1);
    qreal r = color.split(",").at(0).toInt();
    qreal g = color.split(",").at(1).toInt();
    qreal b = color.split(",").at(2).toInt();
    slotSetColor(QColor(r, g, b));
    action->setChecked(true);
}
void btnDrogForm::slotBtnDrogMoreType(QList<QVariantList> data)
{
    _listData.clear();
    _listData = data;
    if (data.isEmpty()) {
        slotSetColor(QColor(128, 0, 0));
    }
    addMoreTypeAction(data);
}
void btnDrogForm::setVisibleForm(bool checked)
{
    if (checked) {
        int moreTypeNum = _publicClass->readIniFile("moreTypeNum").toUInt();
        if (moreTypeNum > 0) {


            _listData.clear();
            for (int i = 1; i < moreTypeNum + 1; i++)
            {
                QVariantList list;

                QString color = _publicClass->readIniFile(QString::number(i)).toString().split(":").at(1);
                qreal r = color.split(",").at(0).toInt();
                qreal g = color.split(",").at(1).toInt();
                qreal b = color.split(",").at(2).toInt();
                list << i << QColor(r, g, b) << _publicClass->readIniFile(QString::number(i)).toString().split(":").at(0);
                _listData << list;
            }

            addMoreTypeAction(_listData);
        }
    }
    this->setVisible(checked);
}