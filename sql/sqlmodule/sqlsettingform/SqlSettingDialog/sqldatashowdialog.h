#pragma once

#include <QDialog>
#include "ui_sqldatashowdialog.h"
enum SQL_INFO
{
	Page,
	Record,
	Info
};
class SqlDataShowDialog : public QDialog
{
    Q_OBJECT
public:
    SqlDataShowDialog(QWidget *parent = Q_NULLPTR);
    ~SqlDataShowDialog();
    void initshowform(const QStringList list, const QString &tablename);
    void setsqldatatomodel(const QMap<QString, QString> &data, const QStringList &keyList);
    void cleartabledata();
    int getspinboxvalue();
    void setmaxvalue(int value);
	void setlabelinfo(SQL_INFO flag, const QString &strinfo);
signals:
    void signalItemText(const QString &);
    void signalChangeValue(int value);
private:
    Ui::SqlDataShowDialog ui;
};
