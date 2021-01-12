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
	//��ǰʵ����ʾתԭʼ
	cv::Rect GetOldRect();
	//ԭʼת��ǰʵ����ʾ
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
	//����
	QPoint  endPoint;
	QPoint  lastPoint;
	//�ƶ�
	QPoint  endPointMoving;
	QPoint  lastPointMoving;
	//��ק
	QPoint  endPointDraging;
	QPoint  lastPointDraging;

	QSize dlg_size;

	bool isDrawing;
	bool isSetingRoi;
	bool isMoveing;
	bool isDrawed;	//�Ƿ񻭹�roi��true ����
	bool isDraging; //��ק
	QRect m_rect;
	QRect g_rect;
	QRect s_rect[4];//�ĸ�С������ק
	FourRect f_rect;
	PicOperate pic_op;

	//tool������
	cv::Rect2f m_oldrect;//ԭʼm_oldrect
	cv::Size m_oldpix;	//ԭʼͼ��rect

	bool isRectContains(QPoint point, QRect isRect);	//�ж�����Ƿ����Ѿ����õľ���������.
	void closeEvent(QCloseEvent *event);


public:signals:
	void SendRoiRect();
};
