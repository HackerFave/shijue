#pragma once

#include <QWidget>
#include <qevent.h>
#include <qpainter.h>
#include <opencv2/opencv.hpp>
#include <mutex>
enum FourRect {
	leftTopRect,
	leftBottomRect,
	rightTopRect,
	rightBottomRect,
};
enum PicOperate {
	drawRoi,
	moveRoi,
	dragRoi,
};

class DetectionSchemeRoi : public QWidget
{
	Q_OBJECT

public:
	DetectionSchemeRoi(QWidget *parent);
	~DetectionSchemeRoi();

	void paintEvent(QPaintEvent *);

	void mousePressEvent(QMouseEvent * event);

	void mouseMoveEvent(QMouseEvent * event);

	void mouseReleaseEvent(QMouseEvent * event);

	void ShowPic(cv::Mat& srcImage);

	void ShowPic(QString path);
	
	void DrawRoi();

	void DelRoi();

	QString GetRoiRect();
	//当前实际显示转原始
	cv::Rect GetOldRect();
	//原始转当前实际显示
	cv::Rect GetCurrenRect(cv::Rect & gcrect);

	void set_grect(float tx, float ty, float bx, float by);

	bool get_issetingroi(){ return isSetingRoi; }
	
	void set_issetingroi(bool setingroi) { isSetingRoi = setingroi; update(); }

private:
	int x, y, w, h,mx,my;
	QPixmap *pix;
	QPixmap *showPic;
	QPixmap *srcPic;
	QPixmap *tempPix;
	//矩形
	QPoint  endPoint;
	QPoint  lastPoint;
	//移动
	QPoint  endPointMoving;
	QPoint  lastPointMoving;
	//拖拽
	QPoint  endPointDraging;
	QPoint  lastPointDraging;

	QSize dlg_size;

	bool isDrawing;
	bool isSetingRoi;
	bool isMoveing;
	bool isDrawed;	//是否画过roi，true 画过
	bool isDraging; //拖拽
	QRect m_rect;
	QRect g_rect;
	QRect s_rect[4];//四个小矩形拖拽
	FourRect f_rect;
	PicOperate pic_op;

	//tool工具用
	cv::Rect2f m_oldrect;//原始m_oldrect
	cv::Size m_oldpix;	//原始图像rect

	bool isRectContains(QPoint point, QRect isRect);	//判断鼠标是否在已经画好的矩形区域内.
	void closeEvent(QCloseEvent *event);


public:signals:
	void SendRoiRect();
};
