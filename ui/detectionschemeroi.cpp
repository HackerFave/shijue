#include "detectionschemeroi.h"
#include "qmessagebox.h"


DetectionSchemeRoi::DetectionSchemeRoi(QWidget *parent)
	: QWidget(parent)
{
	this->setAttribute(Qt::WA_StyledBackground, true);
	//this->setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);
	pix = NULL;
	tempPix = NULL;
	showPic = NULL;
	srcPic = NULL;
	isDrawing = false;
	isSetingRoi = false;
	isMoveing = false;
	isDrawed = true;
	isDraging = false;
	s_rect[0] = QRect(0, 0, 5, 5);
	m_rect = QRect(0, 0, 0, 0);
	g_rect = QRect(0, 0, 0, 0);
	mx = 0;
	my = 0;
	x = 0;
	y = 0;
	w = 0;
	y = 0;

}

DetectionSchemeRoi::~DetectionSchemeRoi()
{
	if (pix != NULL)
	{
		delete pix;
		pix = NULL;
	}
	if (tempPix != NULL)
	{
		delete tempPix;
		tempPix = NULL;
	}
	if (showPic != NULL)
	{
		delete showPic;
		showPic = NULL;
	}
	if (srcPic != NULL)
	{
		delete srcPic;
		srcPic = NULL;
	}
	
}

void DetectionSchemeRoi::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	if (showPic)
	{
		if (!isSetingRoi)	//是否设置过ROI，如果没有则显示原图，有则显示设置过的图像
		{
			if (pix)
			{
				painter.drawPixmap(0, 0, *pix);
				return;
			}
			else
			{
				painter.drawPixmap(0, 0, *showPic);
				return;
			}
		}
	
	}
    if (!pix) {
        return;
    }

	if (!isDrawed)
	{
		x = lastPoint.x();
		y = lastPoint.y();

		w = endPoint.x() - x;
		h = endPoint.y() - y;
		

	}


	if (isDrawing) //如果正在绘图，就在辅助画布上绘制
	{

			
			*tempPix = *pix;
			QPainter pp(tempPix);
			pp.setPen(Qt::red);
			pp.drawRect(x, y, w, h);
			painter.drawPixmap(0, 0, *tempPix);
			m_rect = QRect(x, y, w, h);
			g_rect = m_rect;
	}
	else if (isMoveing)
	{

		mx = endPointMoving.x() - lastPointMoving.x();
		my = endPointMoving.y() - lastPointMoving.y();
		*tempPix = *pix;
		QPainter pp(tempPix);
		pp.setPen(Qt::red);
		QRect d_rect(m_rect.x() + mx, m_rect.y() + my, m_rect.width(), m_rect.height());
		pp.drawRect(d_rect);
		painter.drawPixmap(0, 0, *tempPix);
		g_rect = d_rect;
		
		
	}
	else if (isDraging)
	{
		QRect d_rect(m_rect),t_rect;
		QRect four_rect(0, 0, 0, 0);
		*tempPix = *pix;
		QPainter pp(tempPix);
		pp.setPen(Qt::red);
		QPoint tl, br;
		switch (f_rect)
		{
		case leftTopRect:
			//右下为定点
			br = QPoint(d_rect.x() + d_rect.width(), d_rect.y() + d_rect.height());
			tl = QPoint(endPointDraging.x(), endPointDraging.y());
			
			break;
		case leftBottomRect:
			
			//右上为定点 
			br = QPoint(d_rect.x() + d_rect.width(), endPointDraging.y());
			tl = QPoint(endPointDraging.x(), d_rect.y());
			
			break;
		case rightTopRect:
			//左下为定点
			br = QPoint(endPointDraging.x(), d_rect.y()+ d_rect.height());
			tl = QPoint(d_rect.x(), endPointDraging.y());

			break;
		case rightBottomRect:
			//左上为定点
			br = QPoint(endPointDraging.x(), endPointDraging.y());
			tl = QPoint(d_rect.x(), d_rect.y());
			break;
		default:
			break;

		}
		t_rect = QRect(tl, br);
		pp.drawRect(t_rect);
		painter.drawPixmap(0, 0, *tempPix);
		g_rect = t_rect;
	}
	else 
	{
			QPainter pp(pix);
			pp.setPen(Qt::red);
			QRect d_rect(g_rect);
			pp.drawRect(d_rect);
			QRect ss_rect(s_rect[0]);
			QBrush brush(QColor(0, 0, 255), Qt::SolidPattern);
			pp.setBrush(brush);
			
			ss_rect.moveCenter(d_rect.topLeft());		//左上小矩形
			pp.drawRect(ss_rect);
			s_rect[0] = ss_rect;

			ss_rect.moveCenter(d_rect.bottomLeft());	//左下小矩形
			pp.drawRect(ss_rect);
			s_rect[1] = ss_rect;

			ss_rect.moveCenter(d_rect.topRight());	//右上小矩形
			pp.drawRect(ss_rect);
			s_rect[2] = ss_rect;

			ss_rect.moveCenter(d_rect.bottomRight());	//右下小矩形
			pp.drawRect(ss_rect);
			s_rect[3] = ss_rect;
			painter.drawPixmap(0, 0, *pix);
			
	}
}

	


void DetectionSchemeRoi::mousePressEvent(QMouseEvent *event)
{

	//鼠标左键
	if (event->button()==Qt::LeftButton)
	{
		if (!isDrawed)
		{
			lastPoint = event->pos();
			isDrawing = true;

		}
		//移动图像
		else if (isRectContains(event->pos(),g_rect))
		{
            if (showPic == NULL) {
                return;
            }
			QRect b_rect(g_rect);
			m_rect = g_rect;
			lastPointMoving = event->pos();
			*pix = *showPic;
			isMoveing = true;
		}
		//拖拽
		else if (isRectContains(event->pos(), s_rect[0]))
		{
			//左上
			isDraging = true;
			f_rect = FourRect(leftTopRect);
			*pix = *showPic;
			m_rect = g_rect;
			lastPointDraging = event->pos();
		}
		else if (isRectContains(event->pos(), s_rect[1]))
		{
			//左下
			isDraging = true;
			f_rect = FourRect(leftBottomRect);
			*pix = *showPic;
			m_rect = g_rect;
			lastPointDraging = event->pos();
		}
		else if (isRectContains(event->pos(), s_rect[2]))
		{
			//右上
			isDraging = true;
			f_rect = FourRect(rightTopRect);
			*pix = *showPic;
			m_rect = g_rect;
			lastPointDraging = event->pos();
		}
		else if (isRectContains(event->pos(), s_rect[3]))
		{
			//右下
			isDraging = true;
			f_rect = FourRect(rightBottomRect);
			*pix = *showPic;
			m_rect = g_rect;
			lastPointDraging = event->pos();
		}
	}

}
void DetectionSchemeRoi::mouseMoveEvent(QMouseEvent *event)
{

	if (event->buttons() & Qt::LeftButton & Qt::LeftButton)
	{
		if (!isDrawed)
		{
			endPoint = event->pos();
		
		}
		//移动图像或者缩放
		else
		{
			endPointMoving = event->pos();
			endPointDraging = event->pos();
		}
		update();
	}
	else if (isRectContains(event->pos(), g_rect))
	{
		//在矩形区域内
		setCursor(Qt::OpenHandCursor);

	}
	else if (isRectContains(event->pos(), s_rect[0]) || isRectContains(event->pos(), s_rect[3]))
	{
		//扩大
		setCursor(Qt::SizeFDiagCursor);
	}
	else if (isRectContains(event->pos(), s_rect[1]) || isRectContains(event->pos(), s_rect[2]))
	{
		setCursor(Qt::SizeBDiagCursor);
	}
	else
		setCursor(Qt::CrossCursor);

}
void DetectionSchemeRoi::mouseReleaseEvent(QMouseEvent *event)
{

	if (event->button() == Qt::LeftButton)
	{
		if (!isDrawed)
		{
			endPoint = event->pos();
			isDrawing = false;
			isDrawed = true;
			
		}
		//移动图像
		else if(isMoveing)
		{
			endPointMoving = event->pos();
			isMoveing = false;
		}
		else if (isDraging)
		{
			endPointDraging = event->pos();
			isDraging = false;
		}
		update();
		emit SendRoiRect();
	}
}


void DetectionSchemeRoi::ShowPic(QString path)
{
	QSize test1 = QSize(this->size());
	std::string imagepath = path.toLocal8Bit();
	this->setFocus();
	cv::Mat srcImage = cv::imread(imagepath);

	m_oldpix = srcImage.size();

	cv::cvtColor(srcImage, srcImage, cv::COLOR_BGR2RGB);
	QImage disImage = QImage((const unsigned char*)(srcImage.data), srcImage.cols, srcImage.rows, srcImage.cols*srcImage.channels(), QImage::Format_RGB888);
	QPixmap pix = QPixmap::fromImage(disImage.scaled(this->size(), Qt::IgnoreAspectRatio));
	
	

	if (!showPic)
		showPic = new QPixmap(pix);
	if (!srcPic)
		srcPic = new QPixmap(pix);

	*showPic = pix.copy();
	*srcPic = pix.copy();
	if (this->pix == NULL)
	{
		this->pix = new QPixmap(*showPic);
		tempPix = new QPixmap(*showPic);
	}
	else
	{
		*this->pix = *showPic;
	}
	update();

}


void DetectionSchemeRoi::ShowPic(cv::Mat& srcImage)
{
    
	QSize test1 = QSize(this->size());

	//cv::Mat srcImage = cv::imread("D:\\labelinspector\\data\\1562297763874.PNG");
	cv::cvtColor(srcImage, srcImage, cv::COLOR_BGR2RGB);
	QImage disImage = QImage((const unsigned char*)(srcImage.data), srcImage.cols, srcImage.rows, srcImage.cols*srcImage.channels(), QImage::Format_RGB888);
	QPixmap pix = QPixmap::fromImage(disImage.scaled(this->size(), Qt::IgnoreAspectRatio));

	if (!showPic)
		showPic = new QPixmap(pix);
	if (!srcPic)
		srcPic = new QPixmap(pix);

	*showPic = pix.copy();
	*srcPic = pix.copy();
	if (this->pix == NULL)
	{
		this->pix = new QPixmap(*showPic);
		tempPix = new QPixmap(*showPic);
	}
	else
	{
		*this->pix = *showPic;
	}
    update();
}
void DetectionSchemeRoi::DrawRoi()
{
	isSetingRoi = true;
	isDrawed = false;
	if (!showPic)
		return;
	*pix = *showPic;
	
}

void DetectionSchemeRoi::DelRoi()
{
	if (showPic && pix)
	{
		*pix = *showPic;
		if (isSetingRoi)
		{
			isSetingRoi = false;
			lastPoint = QPoint(0, 0);
			endPoint = QPoint(0, 0);
			isDrawing = false;
			isDrawed = true;
		}
		
	}
	g_rect = QRect(0, 0, 0, 0);
	repaint();
	//DrawRoi();
}



QString DetectionSchemeRoi::GetRoiRect()
{
	int h, w;
	float tx, ty , bx, by, gh, gw;
	QPointF temptl = QPoint();
	QPointF tempbr = QPoint();
	QString str; 
	h = size().height();
	w = size().width();
	if(w&&h)
	{ 
		tx = g_rect.topLeft().x();
		ty = g_rect.topLeft().y();
		
		bx = g_rect.bottomRight().x();
		by = g_rect.bottomRight().y();
	
		gh = g_rect.height();
		gw = g_rect.width();
		
		if (gw <0 && gh <0)
		{
			temptl.setX(bx);
			temptl.setY(by);
			tempbr.setX(tx);
			tempbr.setY(ty);
		}
		else if (gw < 0)
		{
			temptl.setX(bx);
			temptl.setY(ty);
			tempbr.setX(tx);
			tempbr.setY(by);

		}
		else if (gh < 0)
		{
			temptl.setX(tx);
			temptl.setY(by);
			tempbr.setX(bx);
			tempbr.setY(ty);

		}
		else
		{
			temptl.setX(tx);
			temptl.setY(ty);
			tempbr.setX(bx);
			tempbr.setY(by);
		}
		//tx, ty, bx, by
		tx = temptl.x() / (float)w > 1.0 ? 1.0 : temptl.x() / (float)w;
		ty = temptl.y() / (float)h > 1.0 ? 1.0 : temptl.y() / (float)h;
		bx = tempbr.x() / (float)w > 1.0 ? 1.0 : tempbr.x() / (float)w;
		by = tempbr.y() / (float)h > 1.0 ? 1.0 : tempbr.y() / (float)h;

		tx = tx < 0.0 ? 0.0 : tx;
		ty = ty < 0.0 ? 0.0 : ty;
		bx = bx < 0.0 ? 0.0 : bx;
		by = by < 0.0 ? 0.0 : by;

		str = QString::number(tx) + ',' + QString::number(ty) + ','
			+ QString::number(bx) + ',' + QString::number(by);

		if (str == "0,0,0,0")
			return QString();

		m_oldrect.x = temptl.x();
		m_oldrect.y = temptl.y();
		m_oldrect.width = tempbr.x() - temptl.x() ;
		m_oldrect.height = tempbr.y() - temptl.y() ;
		return str;
	}
	return QString();
}
//当前实际显示转原始
cv::Rect DetectionSchemeRoi::GetOldRect()
{

	if(this->GetRoiRect().isEmpty())
		return cv::Rect();

	h = size().height();
	w = size().width();

	if (h&&w)
	{
		if ((m_oldpix.width == 0) || (m_oldpix.height == 0))
			return cv::Rect();
		//高放大倍数
		float fh = (float)h / (float)m_oldpix.height ;
		//宽放大倍数
		float fw =  (float)w / (float)m_oldpix.width ;



		float oldx = m_oldrect.x / fw;
		float oldy = m_oldrect.y / fh;

		float oldw = m_oldrect.width / fw;
		float oldh = m_oldrect.height / fh;

		return cv::Rect((int)oldx,(int)oldy,(int)oldw,(int)oldh);
	}
	
	return cv::Rect();

}
//原始转当前实际显示
cv::Rect DetectionSchemeRoi::GetCurrenRect(cv::Rect & gcrect)
{
	DelRoi();
	isSetingRoi = true;
	int ch = size().height();
	int cw = size().width();
	if (cw&&ch)
	{
		if ((m_oldpix.width == 0) || (m_oldpix.height == 0))
			return  cv::Rect(0, 0, 0, 0);
		//高放大倍数
		float fh = (float)ch / (float)m_oldpix.height;
		//宽放大倍数
		float fw = (float)cw / (float)m_oldpix.width;
	
		float cx = gcrect.x * fw;
		float cy = gcrect.y * fh;

		float cw = gcrect.width * fw;
		float ch = gcrect.height * fh;

		g_rect = QRect((int)cx, (int)cy, (int)cw, (int)ch);
		repaint();
		return cv::Rect((int)cx, (int)cy, (int)cw, (int)ch);
	}
	return cv::Rect(0,0,0,0);
}


void DetectionSchemeRoi::set_grect(float tx, float ty, float bx, float by)
{
	QPoint topleft, bottomright;
	int x, y , h, w;
	h = size().height();
	w = size().width();
	x = tx * w;
	y = ty * h;
	topleft.setX(x);
	topleft.setY(y);

	x = bx * w;
	y = by * h;
	bottomright.setX(x);
	bottomright.setY(y);

	g_rect = QRect(topleft, bottomright);
	update();
}

bool DetectionSchemeRoi::isRectContains(QPoint point,QRect isRect)
{
	//QRect tempRect(isRect.x() - 5, isRect.y() - 5, isRect.width() - 10, isRect.height() - 10);
	//矩形光标
	if (isDrawed && isSetingRoi)
	{
		if (isRect.contains(point))
			return true;
	}
	return false;
}

void DetectionSchemeRoi::closeEvent(QCloseEvent * event)
{
	delete(this);
}

