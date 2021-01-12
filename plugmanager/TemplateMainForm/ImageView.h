#pragma once
#ifndef IMAGE_VIEW_H
#define IMAGE_VIEW_H

#include <QObject>
#include <QWidget>
#include <QImage>
#include <qevent.h>
#include <QPaintEvent>
class ImageView : public QWidget
{
	Q_OBJECT

public:
	ImageView(QWidget *parent);
	~ImageView();
protected:
	void contextMenuEvent(QContextMenuEvent *event) override;
	void paintEvent(QPaintEvent *event) override;
	void wheelEvent(QWheelEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
public:
	void setLoadImage(const QString &imagefilePath);
private:
	QImage m_Image;
	qreal m_ZoomValue = 1.0;
	int m_XPtInterval = 0;
	int m_YPtInterval = 0;
	QPoint m_OldPos;
	bool m_Pressed = false;
    bool ctrlIsPress_ = false;

private slots:
	void onLoadImage(void);
	void onZoomInImage(void);
	void onZoomOutImage(void);
	void onPresetImage(void);
};
#endif
