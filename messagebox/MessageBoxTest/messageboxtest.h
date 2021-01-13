#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_messageboxtest.h"
//#include "../testDll/testDll.h"
class MessageBoxTest : public QMainWindow
{
    Q_OBJECT

public:
    MessageBoxTest(QWidget *parent = Q_NULLPTR);
public slots:
    void on_toolButton_clicked();
private:
    Ui::MessageBoxTestClass ui;
};
