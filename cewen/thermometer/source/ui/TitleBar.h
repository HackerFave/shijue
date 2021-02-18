#pragma once

#include <QWidget>
#include <QObject>
#include <QMouseEvent>
class TitleBar : public QWidget
{
	Q_OBJECT

public:
	TitleBar(QWidget *parent);
	~TitleBar();
	void setParentWidget(QWidget *parentWidget);
protected:
	bool mousePress = false;
	QPoint movePoint = QPoint(0, 0);
	QWidget *parentWidget = nullptr;
	void mousePressEvent(QMouseEvent *);
	void mouseReleaseEvent(QMouseEvent *);
	void mouseMoveEvent(QMouseEvent *);
};
