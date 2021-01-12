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
	void ReNameSignal();		//�����ļ��������ź�
protected:
	virtual void focusInEvent(QFocusEvent *e);
	virtual void focusOutEvent(QFocusEvent *e);
};
