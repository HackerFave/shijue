#pragma once

#include <QWidget>

namespace Ui
{
    class SqlIdItemLabel;
}
class SqlIdItemLabel : public QWidget
{
    Q_OBJECT

public:
    SqlIdItemLabel(QWidget *parent = Q_NULLPTR);
    ~SqlIdItemLabel();
    void SetCheckBoxChecked(bool checked);
    void SetCheckBoxText(const QString &text);
    QString GetCheckedBoxText();
    bool GetCheckBoxCheckedStatus();
    void SetLineeditSqlData(const QString &data);
    void SetLineeditPlaceholder(const QString &data);//输入框提示信息
    QString GetLineeditSqlData();
    void SetLabelSqlData(const QString &data);
private:
    Ui::SqlIdItemLabel* ui;
};
