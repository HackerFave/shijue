#pragma once

#include <QOpenGLWidget>
#include "qmutex.h"
//class WidgetPic : public QOpenGLWidget
class WidgetPic : public QWidget
{
	Q_OBJECT

public:
	WidgetPic(QWidget *parent);
	~WidgetPic();
private:
	QImage *m_img;
	QImage m_oldimg;
	QMutex mutex;
	bool m_ismax;
	//������λ
	int m_bestaion;
	//������Ļ
	int m_bewindow;
public:
	int get_bestaion() { return m_bestaion; }
	void set_bestaion(int staion) { m_bestaion = staion; }
	int get_bewindow() { return m_bewindow; }
	void set_bewindow(int window) { m_bewindow = window; }

	void SetQimage(QImage img);
	void setIsMax(bool flag);
private:
	//���˫��
	void mouseDoubleClickEvent(QMouseEvent * event);
	void paintEvent(QPaintEvent *e);
};

