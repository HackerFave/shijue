#include "widgetpic.h"
#include "qpainter.h"
#include "qdebug.h"
#include "qdatetime.h"
#include "qdesktopwidget.h"
#include "qapplication.h"
//WidgetPic::WidgetPic(QWidget *parent)
//	: QOpenGLWidget(parent)
WidgetPic::WidgetPic(QWidget *parent)
	: QWidget(parent)
{
	m_img = NULL;
	m_ismax = false;
	m_bestaion = -1;
	m_bewindow = -1;
	this->setAttribute(Qt::WA_StyledBackground, true);
	this->setStyleSheet("background-color: rgb(0,0, 0)");
	
}

WidgetPic::~WidgetPic()
{
	if(m_img!=NULL)
	{ 
		delete m_img;
		m_img = NULL;
	}
}

void WidgetPic::mouseDoubleClickEvent(QMouseEvent * event)
{
	if (m_ismax)
	{
		m_ismax = false;
		setWindowFlags(Qt::SubWindow);
		showNormal();
		
	}
	else
	{
		QDesktopWidget* desktop = QApplication::desktop();
		m_ismax = true;
		setWindowFlags(Qt::Window);
		showFullScreen();
		this->setGeometry(desktop->screenGeometry(m_bewindow));
		
	}
	if(m_img)
		SetQimage(m_oldimg);
	repaint();
}

void WidgetPic::paintEvent(QPaintEvent * e)
{
	if(m_img!=NULL)
	{
		QPainter painter;
		painter.begin(this);

		mutex.lock();
		int w = width() - m_img->width();
		int h = height()- m_img->height();
		w = w > 0 ? w / 2 : 0;
		h = h > 0 ? h / 2 : 0;

		painter.drawImage(QPoint(w, h), *m_img);
		mutex.unlock();

		painter.end();
	}
	update();
}

void WidgetPic::SetQimage(QImage img)
{
	//QTime time;
	//time.start();
	
	QImage result = img.scaled(width(), height(),Qt::KeepAspectRatio);
	//qDebug() << time.elapsed() / 1000.0 << "s";
	if (m_img == NULL)
	{
		m_img = new QImage(result);
	}
	else
	{
		mutex.lock();
		m_oldimg = img.copy();
		*m_img = result.copy();
		mutex.unlock();
	}
}


