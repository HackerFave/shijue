#pragma once

#include <QWidget>
#include <QMouseEvent>
#include <QButtonGroup>
#include <QColorDialog>
#include <QComboBox>
#include <QMenuBar>
#include <QMenu>
#include "publicClass.h"
#include "ui_btnDrogForm.h"
#pragma execution_character_set("utf-8")
class btnDrogForm : public QWidget
{
    Q_OBJECT

public:
    btnDrogForm(QWidget *parent = Q_NULLPTR);
    ~btnDrogForm();
    void setBtnChecked();
    int getCursorSize();
    bool getBtnChecked(int index);
    void addMoreTypeAction(const QList<QVariantList> & listType);
    void setVisibleForm(bool checked);
protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
signals:
    void signalBtnId(int);
    void signalValue(int);
    void signalLineColor(QColor);
private slots:
    void on_btnMouse_clicked();//选择
    void on_btnDrawLine_clicked();//画笔
    void on_btnEras_clicked();//擦除
    void on_btnClear_clicked();//清除
    void on_btnApply_clicked();//应用
    void slotValueChange(int);
    void slotClose();
    void on_toolButton_color_clicked();
    void slotShowColor(QColor color);
    void slotSetColor(QColor color);
    void slotSetColor_1();
    void on_toolButton_menu_clicked();
    void onMenuTriggered(QAction*);
    void slotBtnDrogMoreType(QList<QVariantList>);
private:
    void clearStyle(QToolButton *btn);
    QString converRGB2HexStr(QColor _rgb);
private:
    Ui::btnDrogForm ui;
    bool isPressedWidget = false;
    QPoint m_lastPos;
    QButtonGroup * _btnGroup = Q_NULLPTR;

    QColor m_color;//记录线条初始颜色
    QColor m_selectedColor;//记录按OK键后选中的颜色
    QMenu *_menu = Q_NULLPTR;
    publicClass * _publicClass = Q_NULLPTR;
    QList<QVariantList> _listData;//获取多类别数据
};
