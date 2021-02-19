#include "messageDialog.h"
#include <qdebug.h>
messageDialog::messageDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	this->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
	this->setModal(true);
	ui.label_3->adjustSize();
	//2. 让QLabel能够自动判断并换行显示：
	ui.label_3->setGeometry(QRect(328, 240, 329, 27 * 4)); //四倍行距
	ui.label_3->setWordWrap(true);
	ui.label_3->setAlignment(Qt::AlignTop);

	connect(ui.toolButton, SIGNAL(clicked()), this, SLOT(slotCancel()));
	connect(ui.pushButton_2, SIGNAL(clicked()), this, SLOT(slotOk()));
	connect(ui.pushCancel, SIGNAL(clicked()), this, SLOT(slotCancel()));
	_index = false;
}

messageDialog::~messageDialog()
{
}


void messageDialog::setMessageHint(QString &icon, QString &strHead, QString &strHint)
{
	
	QImage Image;
	Image.load(icon);
	QPixmap pixmap = QPixmap::fromImage(Image);
	int with = ui.label->width();
	int height = ui.label->height();
	QPixmap fitpixmap = pixmap.scaled(with, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);  // 按比例缩放
	ui.label->setAlignment(Qt::AlignCenter);
	ui.label->setPixmap(fitpixmap);

	ui.label_2->setText(strHead);
	ui.label_3->setAlignment(Qt::AlignCenter);
	ui.label_3->setText(strHint);
}
//strhint 为空：成功； ？ 警告； 其他：报警；
void messageDialog::setMessageHint(QString &strHead, QString &strHint)
{
	ui.pushCancel->hide();

	QPixmap pixmap;
	if (strHint.isEmpty())
		pixmap = QPixmap(":/resources/icons/ic_success.png");
	else if(QString::compare(strHint,"?")!=0)
		pixmap = QPixmap(":/resources/icons/ic_warning.png");
	else
	{
		strHint = "";
		pixmap = QPixmap(":/resources/icons/ic_warning.png");
		//pixmap = QPixmap(":/resources/icons/ic_solution_help.png");
		ui.pushCancel->show();
	}
	ui.label->setPixmap(pixmap);
	int with = ui.label->width();
	int height = ui.label->height();
	ui.label->setAlignment(Qt::AlignCenter);
	ui.label_2->setText(strHead);
	ui.label_3->setAlignment(Qt::AlignCenter);
	ui.label_3->setText(strHint);
}

bool messageDialog::getCurrentIndex()
{
	return _index;
}
void messageDialog::setFontSize(int size ,int indexLabel)
{
	switch (indexLabel)
	{
	case 0:
	{
		QFont ft;
		ft.setPointSize(size);
		ui.label_2->setFont(ft);
	}
	break;
	case 1:
	{
		QFont ft;
		ft.setPointSize(size);
		ui.label_3->setFont(ft);
	}
	break;
	default:
		break;
	}
}
void messageDialog::setFontColor(QColor color , int indexLabel)
{
	
	switch (indexLabel)
	{
	case 0:
	{
		QPalette pa;
		pa.setColor(QPalette::WindowText, color);
		ui.label_2->setPalette(pa);
	}
	break;
	case 1:
	{
		QPalette pa;
		pa.setColor(QPalette::WindowText, color);
		ui.label_3->setPalette(pa);
	}
	break;
	default:
		break;
	}
}
void messageDialog::setIconSize(int width, int height)
{
	ui.label->setFixedSize(QSize(width, height));
}