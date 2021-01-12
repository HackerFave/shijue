#include "mylistwidget.h"
#include "qmessagebox.h"
#include "uicommon.h"
#include "qdebug.h"
MyListWidget::MyListWidget(QWidget *parent)
	: QListWidget(parent)
{
	is_rename = false;
}

MyListWidget::~MyListWidget()
{
}

void MyListWidget::focusInEvent(QFocusEvent * e)
{
	if(is_rename)
	{ 
		closePersistentEditor(this->currentItem());
		
		is_rename = false;
		emit ReNameSignal();
	}
	
}

void MyListWidget::focusOutEvent(QFocusEvent * e)
{
	
	
	
}
