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
    void on_btnMouse_clicked();//ѡ��
    void on_btnDrawLine_clicked();//����
    void on_btnEras_clicked();//����
    void on_btnClear_clicked();//���
    void on_btnApply_clicked();//Ӧ��
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

    QColor m_color;//��¼������ʼ��ɫ
    QColor m_selectedColor;//��¼��OK����ѡ�е���ɫ
    QMenu *_menu = Q_NULLPTR;
    publicClass * _publicClass = Q_NULLPTR;
    QList<QVariantList> _listData;//��ȡ���������
};
