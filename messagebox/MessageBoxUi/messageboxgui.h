#pragma execution_character_set("utf-8")
#pragma once

#include <QtCore/qglobal.h>
#include <QDialog>
#include <QMouseEvent>
#include <string>
#include "mrun_messagebox_define.h"
namespace Ui
{
    class MessageBoxGui;
}
class MessageBoxGui : public QDialog
{
    Q_OBJECT
 
public:
    MessageBoxGui(QDialog *parent = Q_NULLPTR);
    ~MessageBoxGui();
    void SetMessageHint(QString &icon, QString &strHead, QString &strHint);
    void SetMessageHint(HINT_FLAG flag, QString strHint);
    bool GetCurrentIndex();
    void SetFontSize(int size, int indexLabel);
    void SetFontColor(QColor color, int indexLabel);
    void SetIconSize(int width, int height);
	void MessageOperT(QString tips);
public slots:
    void SlotOk();
    void SlotCancel();
signals:
    void signalIndexBtn(bool);
    void SendMesSignal(std::string shead, std::string msg);
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
private:
    Ui::MessageBoxGui * ui;
    bool ok_cancel_flag_;
    bool ispressedwidget_ = false;
    QPoint lastpos_;
};
