#pragma once

#include <QDialog>
#include <qicon.h>
#include "ui_messageDialog.h"

class messageDialog : public QDialog
{
	Q_OBJECT

public:
	messageDialog(QWidget *parent = Q_NULLPTR);
	~messageDialog();
	void setMessageHint(QString &icon,QString &strHead, QString &strHint);
	void setMessageHint(QString &strHead, QString &strHint);
	bool getCurrentIndex();
	void setFontSize(int size , int indexLabel);
	void setFontColor(QColor color, int indexLabel);
	void setIconSize(int width, int height);

private slots:
	void slotOk()
	{
		//emit signalIndexBtn(true);
		this->close();
		_index = true;
	}
	void slotCancel()
	{
		//emit signalIndexBtn(false);
		this->close();
		_index = false;
	}
signals:
	void signalIndexBtn(bool);
public:
signals:
	void SendMesSignal(QString shead, QString msg);
private:
	Ui::messageDialog ui;

	bool _index;
};
