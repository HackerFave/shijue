#pragma once

#include <QWidget>
#include <QStandardItemModel>
#include "ui_qualityStandardComboBox.h"

class qualityStandardComboBox : public QWidget
{
    Q_OBJECT

public:
    qualityStandardComboBox(QWidget *parent = Q_NULLPTR);
    ~qualityStandardComboBox();
    void addItemRow(const QString &str);

private:
    Ui::qualityStandardComboBox ui;
};
