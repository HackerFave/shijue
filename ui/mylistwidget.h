#pragma once

#include <QListWidget>

class MyListWidget : public QListWidget
{
	Q_OBJECT
public:
	bool is_rename;
public:
	MyListWidget(QWidget *parent);
	~MyListWidget();
signals:
	void ReNameSignal();		//配置文件重命名信号
protected:
	virtual void focusInEvent(QFocusEvent *e);
	virtual void focusOutEvent(QFocusEvent *e);
};
