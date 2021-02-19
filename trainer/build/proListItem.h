#pragma once

#include <QWidget>
#include <QMouseEvent>
#include "ui_proListItem.h"

class proListItem : public QWidget
{
    Q_OBJECT

public:
    proListItem(QWidget *parent = Q_NULLPTR,QString id ="");
    ~proListItem();
    void setProName(const QString &textProName);
protected:
    //void mouseReleaseEvent(QMouseEvent * ev);
    void mouseDoubleClickEvent(QMouseEvent *event);
private slots:
    void on_toolButton_status_clicked(bool);
    void on_toolButton_select_clicked(bool);
signals:
    void signalSelect(bool,QString,QString);
    void signalStatus(bool,QString,QString);
private:
    Ui::proListItem ui;
    QString _id;
};
