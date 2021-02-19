#pragma once

#include <QWidget>
#include "ui_moreTypeForm.h"
#pragma execution_character_set("utf-8")
class moreTypeForm : public QWidget
{
    Q_OBJECT

public:
    moreTypeForm(QWidget *parent = Q_NULLPTR);
    ~moreTypeForm();
    bool getChecked();
    QString getCheckeBoxStr();
    void setChecked(bool checked);
    void setCheckBoxStr(const QString &str);
    void setLineEditTypeStr(const QString &str);
    QString getTypeText();
    void getTypeStr();
    void setLabelColor(QColor color);
    QColor getLabelColor();
private slots:
    void on_toolButton_changeColor_clicked();
    void slotShowColor(QColor color);
    void slotSetColor(QColor color);
    void slotSetColor_1();
signals:
    void signalLabelColor(QColor);
private:
    Ui::moreTypeForm ui;
    QColor m_color;//记录线条初始颜色
    QColor m_selectedColor;//记录按OK键后选中的颜色
    QString _TypeStr;
};
