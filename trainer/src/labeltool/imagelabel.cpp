#include "imagelabel.h"
#include "qtmat.h"
#include "qimagereader.h"
#include "qaction.h"
#include "qmenu.h"
#include "qapplication.h"
#include "ciwamsgdialog.h"
#include <QTime>

ImageLabel::ImageLabel(bool blabelregion, bool isTextShow,QWidget *parent)
    : QWidget(parent), _isBlabelregion(blabelregion),_isTextShow(isTextShow)
{
    penoldtype_ = COUNT_;
    pentype_ = COUNT_;//��������
    pencolor_ = QColor(Qt::red);//������ɫ
    pensize_ = 10;//���ʴ�С
    blabelregion = false;//�Ƿ��Ǳ�ע����
    bshowlabel_ = true;//�Ƿ���ʾ��עͼ
    bshowpredict = true;//�Ƿ���ʾԤ��ͼ
    bshowpredictpixinfo_ = true;//�Ƿ���ʾԤ����Ϣ
    bkeepaspectratio_ = false;//�Ƿ񱣳ֳ����
    mousecheck_.state = false; //�����״̬
    imagemenu_ = ImageMenu{ NULL,NULL,NULL,NULL,NULL };//����Ҽ�
    m_pointlist.clear();
    ainnoimage_ = NULL;//����ͼƬ��Ϣ
    InitMenu();
    x_ = 0;
    y_ = 0;
    zoomy_ = 1;
    zoomx_ = 1;
    zoomoldy_ = 0.000001;
    zoomoldx_ = 0.000001;
    mousepressed_ = false;
    scoretype_ = -1;
    predicttypepen_.setColor(Qt::red);
    score_.clear();
    colormap_.clear();
    maskgraycolormap_.clear();
}

ImageLabel::~ImageLabel()
{
    if (ainnoimage_ != NULL)
    {
        delete ainnoimage_;
        ainnoimage_ = NULL;
    }
}

//�˵�����ɾ��
void ImageLabel::triget_menu_delete()
{
    emit signalDelImage(blabelregion);
}

//�˵�����������ȱ��
void ImageLabel::triget_menu_nolabel()
{
    labelpix_.fill(Qt::transparent);
    maskpix_.fill(Qt::black);
    repaint();
    //�����ź�
    emit signalMenuOperate(blabelregion,(int)MenuOperate::O_NOLABEL,(void*)this);
}

void ImageLabel::MenuTrainAlways(bool is)
{
    QAction* temp;
    if (is){    //ǿ���޸�
        if (imagemenu_.menu_train_always != NULL) {
            temp = (QAction*)imagemenu_.menu_train_always;

            temp->setChecked(true);
            imagemenu_.b_train_always = true;
            ainnoimage_->trainpolicy = ainno_train_policy::AINNO_TRAIN_ALWAYS;
        }

        if (imagemenu_.menu_train_auto != NULL) {
            temp = (QAction*)imagemenu_.menu_train_auto;
            temp->setChecked(false);
            imagemenu_.b_train_auto = false;
        }
        if (imagemenu_.menu_train_never != NULL) {
            temp = (QAction*)imagemenu_.menu_train_never;
            temp->setChecked(false);
            imagemenu_.b_train_never = false;
        }
    }
    else{
        triget_menu_train_auto();
    }
}

//�˵�����ѵ����������
void ImageLabel::triget_menu_train_always()
{
    MenuTrainAlways(!imagemenu_.b_train_always);
    //�����ź�
    emit signalMenuOperate(blabelregion, (int)MenuOperate::O_ALWAYS, (void*)this);

}

void ImageLabel::MenuTrainNever(bool is)
{
    QAction* temp;
    if (is) {    //ǿ���޸�
        if (imagemenu_.menu_train_never != NULL) {
            temp = (QAction*)imagemenu_.menu_train_never;


            temp->setChecked(true);
            imagemenu_.b_train_never = true;
            ainnoimage_->trainpolicy = ainno_train_policy::AINNO_TRAIN_NEVER;
        }
        if (imagemenu_.menu_train_auto != NULL) {
            temp = (QAction*)imagemenu_.menu_train_auto;
            temp->setChecked(false);
            imagemenu_.b_train_auto = false;
        }
        if (imagemenu_.menu_train_always != NULL) {
            temp = (QAction*)imagemenu_.menu_train_always;
            temp->setChecked(false);
            imagemenu_.b_train_always = false;
        }
    
    }
    else {
        triget_menu_train_auto();
    }
}


//�˵�����ѵ�������Ӳ�
void ImageLabel::triget_menu_train_never()
{
    MenuTrainNever(!imagemenu_.b_train_never);
    emit signalMenuOperate(blabelregion, (int)MenuOperate::O_NEVER, (void*)this);
}

void ImageLabel::MenuTrainAuto(bool is)
{
    QAction* temp;
    if (imagemenu_.menu_train_auto != NULL) {
        temp = (QAction*)imagemenu_.menu_train_auto;
        temp->setChecked(true);
        imagemenu_.b_train_auto = true;
        ainnoimage_->trainpolicy = ainno_train_policy::AINNO_TRAIN_AUTO;
    }
    if (imagemenu_.menu_train_always != NULL)
    {
        temp = (QAction*)imagemenu_.menu_train_always;
        temp->setChecked(false);
        imagemenu_.b_train_always = false;
    }
    if (imagemenu_.menu_train_never != NULL)
    {
        temp = (QAction*)imagemenu_.menu_train_never;
        temp->setChecked(false);
        imagemenu_.b_train_never = false;
    }
   
}

//�˵�����ѵ�������Զ�
void ImageLabel::triget_menu_train_auto()
{
    MenuTrainAuto();
    emit signalMenuOperate(blabelregion, (int)MenuOperate::O_AUTO, (void*)this);
}   
//�رձ�ע����
void ImageLabel::triget_menu_close_auto()
{
    emit signalMenuOperate(blabelregion, (int)MenuOperate::O_CLOSE, (void*)this);
     
}
//�򿪱�ע����
void ImageLabel::triget_menu_open_auto()
{
    emit signalMenuOperate(blabelregion, (int)MenuOperate::O_OPEN, (void*)this);
}
//����
void ImageLabel::triget_menu_test()
{
    emit signalMenuOperate(blabelregion, (int)MenuOperate::O_TEST, (void*)this);
}
void ImageLabel::paintEvent(QPaintEvent * e)
{
    QPainter paint(this);
    /*if (_isTextShow) {
         QFont font("����", 15, QFont::Bold, true);
    paint.setFont(font);
    paint.setPen(Qt::white);
    paint.drawText(100,40, QString(ainnoimage_->filename.c_str()));
    return;
    }*/
    if (blabelregion) {
        if(zoomx_>zoomoldx_){
            
        }
        else {
            ResetZoom();
        }
        // ƽ��
        paint.translate(xptinterval_, yptinterval_);
        //����
    }
    paint.scale(zoomx_, zoomy_);
  //  paint.scale((zoomx_ >=zoomy_ )? zoomy_ : zoomx_, (zoomx_ >= zoomy_ )? zoomy_ : zoomx_);
    //��ʾԭͼ
    paint.drawPixmap(0, 0, srcpix_);
    
    //��ʾ��עͼ
    if (bshowlabel_){
        paint.drawPixmap(0, 0, labelpix_);
        //paint.drawPixmap(0, 0, labeltypepix_);
    }

    //��ʾԤ��ͼ
    if (bshowpredict){

        paint.drawPixmap(0, 0, predictpix_);
        //paint.drawPixmap(0, 0, predicttypepix_);
    }
    QPainter paintp(this);
    //��ʾ��עͼ
    if (bshowlabel_) {
        paintp.drawPixmap(0, 0, labeltypepix_);
    }

    //��ʾԤ��ͼ
    if (bshowpredict) {
        paintp.drawPixmap(0, 0, predicttypepix_);
    }


    //��ʾͼƬ��Ϣ
    paintp.drawPixmap(0, 0, infopix_);
    
    //��ʾͼ��Ԥ����Ϣ
    if(bshowpredictpixinfo_&&blabelregion){
        
        paintp.drawPixmap(x_+10, y_, predictpixinfo_);
    }

    //ѡ��״̬��ֻ�����б��в���ʾ
    if (!blabelregion) {
        if (mousecheck_.state) {
            paintp.drawImage(0,0,mousecheck_.pix);
        }
    }
}
bool ImageLabel::ReadFileImage(QString projectabspath ,ainno_image* curimg)
{
    ResetZoom();
    QString fileabspath = QString("%1/%2")
        .arg(projectabspath).arg(QString::fromLocal8Bit(curimg->filename.c_str()));
    //����ͼ��
    QImageReader reader;
    reader.setFileName(fileabspath);
    reader.setDecideFormatFromContent(true);
    if (reader.canRead()) {
        if (reader.read(&srcimage_)) {
            //qDebug() << "open is successful....";
        }
        else {
            CiWaMsgDialog::MessageSignal("�ļ�", "���ļ�ʧ��:" + reader.errorString());
            return false;
        }
    }
    else {
        CiWaMsgDialog::MessageSignal("�ļ�", "���ļ�ʧ��:" + reader.errorString());
        return false;
    }
    if (ainnoimage_ != NULL) {
        delete ainnoimage_;
        ainnoimage_ = NULL;
    }
    ainnoimage_ = new ainno_image(*curimg);
    return true;
}

bool ImageLabel::Open(QString projectabspath, ainno_image curimg)
{
    if (!ReadFileImage(projectabspath, &curimg)) {
        return false;
    }
    bool res = ShowImage();
    if (res) {
        scalex_ = 1;
        scaley_ = 1;
        DrawLabel();
        DrawPredict();
        ShowInfo();
        SetMenu();
        //update();
    }
    return res;
}

bool ImageLabel::ShowImage()
{
    srcpix_ = QPixmap::fromImage(srcimage_);
    labeltypepix_ = QPixmap(width(), height());
    labeltypepix_.fill(Qt::transparent);
    predicttypepix_ = QPixmap(width(), height());
    predicttypepix_.fill(Qt::transparent);
    predictpixinfo_ = QPixmap(200, 300);
    predictpixinfo_.fill(Qt::transparent);
    predictpix_ = QPixmap(srcpix_.width(), srcpix_.height());
    predictpix_.fill(Qt::transparent);

    //��ʾͼƬ��Ϣ�������ļ���
    if (blabelregion) {
        infopix_ = QPixmap(srcpix_.width(), 50);
    }
    else {
        infopix_ = QPixmap(200, 50);
    }
    m_pointlist.clear();
    return true;
}

void ImageLabel::SetMenu(bool isset) {
    //if(!isset){
    //    if (!blabelregion) {
    //        return;
    //    }
    //}
    ClearMenu();
    QAction *temp;
    if (imagemenu_.menu_nolabel != NULL) {
        temp = (QAction*)imagemenu_.menu_nolabel;
        if (ainnoimage_->labeled && ainnoimage_->labeltype == AINNO_LABEL_GOOD) {
            temp->setChecked(true);
        }
        else {
            temp->setChecked(false);
        }
    }
    switch (ainnoimage_->trainpolicy)
    {
    case ainno_train_policy::AINNO_TRAIN_ALWAYS:
        temp = (QAction*)imagemenu_.menu_train_always;
        temp->setChecked(true);
        imagemenu_.b_train_always = true;
        break;
    case ainno_train_policy::AINNO_TRAIN_AUTO:
        temp = (QAction*)imagemenu_.menu_train_auto;
        temp->setChecked(true);
        imagemenu_.b_train_auto = true;
        break;
    case ainno_train_policy::AINNO_TRAIN_NEVER:
        temp = (QAction*)imagemenu_.menu_train_never;
        temp->setChecked(true);
        imagemenu_.b_train_never = true;
        break;
    }
}
//trainer����boolֵ���Ƿ������ʾ
void ImageLabel::isFill(bool checked)
{
    _isFill = checked;
}
//��ͼ��ʾ��ע
void ImageLabel::DrawLabel() {
    maskpix_ = QPixmap(srcimage_.width(), srcimage_.height());
    maskpix_.fill(Qt::black);
    if (ainnoimage_->labeled) {
        if (ainnoimage_->labeltype == ainno_label_type::AINNO_LABEL_BAD) {
            //��ʾ��עͼ
            float scalex, scaley;
            QPen pen;
            labelpix_ = QPixmap(srcimage_.width(), srcimage_.height());
            labelpix_.fill(Qt::transparent);

            for (int i = 0; i < ainnoimage_->labeldefects.size(); i++)
            {
                QPointF* points = new QPointF[ainnoimage_->labeldefects[i].size()];
                int index = 0;
                for (auto it = ainnoimage_->labeldefects[i].begin();
                    it != ainnoimage_->labeldefects[i].end(); it++, index++)
                {
                    points[index] = QPointF((float)(it->x), (float)(it->y));
                }
                pen.setColor(colormap_[ainnoimage_->labeldefectsinfo[i].label]);
                DrawPoint(&labelpix_, points, ainnoimage_->labeldefects[i].size(), pen, _isFill);//��ʾ�Ƿ����
                //ʹ�ûҶ�colormap_�����ұ���Ҫ���!!!! ����֮����contour��������
                //pen.setColor(QColor(255, 255, 255));
                pen.setColor(maskgraycolormap_[ainnoimage_->labeldefectsinfo[i].label]);
                DrawPoint(&maskpix_, points, ainnoimage_->labeldefects[i].size(), pen, true);
                delete[] points;
            }
        }
        else {
            //Ϊ�˲���Ƶ��ˢ�£����Կ���ԭͼ
            labelpix_ = srcpix_.copy();
        }
    }
    else
    {
        //Ϊ�˲���Ƶ��ˢ�£����Կ���ԭͼ
        labelpix_ = srcpix_.copy();
    }
}

//��ʾԤ�� scoretype 1:scoreavg 0:scoremax,3scoremin
void ImageLabel::DrawPredictEx(std::vector<float> ng_thrs, int scoretype)
{
    QPen pen;
    //ͨ����ֵ�жϸı������ɫ
    bool bluecolor = false;

    //������ֵ�ж�
    if (ainnoimage_ != NULL) {
        double is = -1;
        if (scoretype < 0) {
            //ȫ��
            if (ainnoimage_->predictdefects.size() > 0) {
                float scalex, scaley;
                QPen pen;
                pen.setColor(QColor(0, 0, 255));
                predictpix_ = QPixmap(srcpix_.width(), srcpix_.height());
                predictpix_.fill(Qt::transparent);
                for (int i = 0; i < ainnoimage_->predictdefects.size(); i++)
                {
                    QPointF* points = new QPointF[ainnoimage_->predictdefects[i].size()];
                    int index = 0;
                    for (auto it = ainnoimage_->predictdefects[i].begin();
                        it != ainnoimage_->predictdefects[i].end(); it++, index++)
                    {
                        points[index] = QPointF(it->x, it->y);
                    }
                    DrawPoint(&predictpix_, points, ainnoimage_->predictdefects[i].size(), pen, false);
                    delete[] points;
                }
            }
        }
        else {
            predictpix_.fill(Qt::transparent);
            for (int i = 0; i < ainnoimage_->predictdefects.size(); i++)
            {
                //��ȡlabel
                int label = ainnoimage_->predictdefectsinfo[i].label;
                //��ȡ�÷�
                float score = 0;
                if (scoretype == 1) {
                    score = ainnoimage_->predictdefectsinfo[i].scoreavg;
                }
                else if (scoretype == 0) {
                    score = ainnoimage_->predictdefectsinfo[i].scoremax;
                }
                else {
                    score = ainnoimage_->predictdefectsinfo[i].scoremin;
                }

                //�ж���ֵ�͵÷�
                if (ng_thrs[label] < score) {
                    //��ɫ
                    pen.setColor(QColor(0, 0, 255));
                    bluecolor = true;
                }
                else {
                    //��ɫ
                    pen.setColor(QColor(0, 255, 0));
                }
                //������
                QPointF* points = new QPointF[ainnoimage_->predictdefects[i].size()];
                int index = 0;
                for (auto it = ainnoimage_->predictdefects[i].begin();
                    it != ainnoimage_->predictdefects[i].end(); it++, index++)
                {
                    //points[index] = QPointF((float)(it->x) * scalex, (float)(it->y) * scaley);
                    points[index] = QPointF(it->x, it->y);
                }
                DrawPoint(&predictpix_, points, ainnoimage_->predictdefects[i].size(), pen, false);
                delete[] points;
            }
            //�ı������ɫ
            if (bluecolor) {
                predicttypepen_.setColor(Qt::red);
            }
            else {
                predicttypepen_.setColor(Qt::green);
            }
            if ((scalex_ != 1) && (scaley_ != 1)) {
                SetPredictType(predicttypepen_);
            }
        }
    }
    score_ = ng_thrs;
    scoretype_ = scoretype;
    //repaint();
}
  



void ImageLabel::DrawPredict()
{
    ////Ԥ��
    //if (ainnoimage_->predictdefects.size() > 0) {
    //    cv::Mat predictdefects_img(srcimage_.height(), srcimage_.width(), CV_8UC3, cv::Scalar(0, 0, 0));
    //    //��������ж�
    //    cv::drawContours(predictdefects_img, ainnoimage_->predictdefects, -1, cv::Scalar(255, 0, 0), 5);
    //    predictpix_ = CVS::cvMatToQPixmap(predictdefects_img);
    //    predictpix_.setMask(predictpix_.createMaskFromColor(QColor(0, 0, 0), Qt::MaskInColor));
    //    

    //}
    //else {
    //    predictpix_ = QPixmap(srcpix_.width(), srcpix_.height());
    //    predictpix_.fill(Qt::transparent);
    //}

    DrawPredictEx(score_, scoretype_);
}

void ImageLabel::UpdateImageInfo(ainno_image ainnoimage) {
    *ainnoimage_ = ainnoimage;
}

void ImageLabel::ResizePredictdefects(int w, int h)
{
    if (ainnoimage_) {
        if (ainnoimage_->predictdefects.size()) {
            //bad
            SetPredictType(predicttypepen_);
        }
        else {
            //good
            SetPredictType(LabelRectFlag::NONE_);
        }
    }
    update();
}

//����
void ImageLabel::DrawLine(QPainter& p)
{
    //�����ߣ���ʼ�㵽������
    for (int i = 1; i < m_pointlist.size(); i++) {
        //�����ߣ���ʼ�㵽������
        p.drawLine(QLineF((float)(m_pointlist[i - 1].x()-xptinterval_)/ zoomx_, (float)(m_pointlist[i - 1].y()-yptinterval_)/ zoomy_,
            (float)(m_pointlist[i].x() - xptinterval_)/zoomx_, (float)(m_pointlist[i].y() - yptinterval_)/ zoomy_));
    }
}

//������ֵ����(�Ƿ����)
void ImageLabel::DrawPoint(QPixmap* pix, const QPointF* point, int pointsize, QPen pen, bool fill)
{
    QPainter paint(pix);
    if (fill) {
        pen.setWidthF(0.001);
        paint.setBrush(pen.color());//���
    }
    else {
        if (pix == &maskpix_) {
            pen.setWidthF(1);
        }
        else { //������ʾ���߿�
            pen.setWidth(_lineSizeWidth);
        }
    }
    paint.setPen(pen);
    paint.drawPolygon(point, pointsize);
}

//����ֵ��Ϣ
void ImageLabel::DrawPredictInfo(QPoint rpos)
{
    QPointF pos;
    pos.setX(((float)(rpos.x()-xptinterval_ )/ zoomx_));
    pos.setY(((float)(rpos.y() - yptinterval_ )/zoomy_ ));
    if (ainnoimage_ != NULL) {
        double is = -1;
        int idx = -1;
        for (int i = 0; i < ainnoimage_->predictdefects.size(); i++)
        {
            cv::Point2f srcPt(pos.x(), pos.y());
            is = cv::pointPolygonTest(ainnoimage_->predictdefects[i], srcPt, false);
            if (is >= 0) {
                idx = i;
                break;
            }
        }
        if (is >= 0) {//����Ϣ  
            predictpixinfo_.fill(Qt::transparent);
            QPainter painter(&predictpixinfo_);
            QFont font("����", 8, QFont::Bold, true);
            font.setItalic(false);
            painter.setPen(Qt::blue);
            painter.setFont(font);
            x_ = rpos.x();
            y_ = rpos.y();
            QString text;
            if(scoretype_ == 1){
                text = QString::number(ainnoimage_->predictdefectsinfo[idx].label) + ":"
                    + QString::number(ainnoimage_->predictdefectsinfo[idx].scoreavg, 'f', 2);
            }
            else {
                text = QString::number(ainnoimage_->predictdefectsinfo[idx].label) + ":"
                    + QString::number(ainnoimage_->predictdefectsinfo[idx].scoremax, 'f', 2);
            }
            painter.drawText(0, 10, text);
            repaint();
        }

    }
}

void ImageLabel::DrawMoveZoom(QPoint pos)
{
    int xPtInterval = pos.x() - mouseoldpos_.x();
    int yPtInterval = pos.y() - mouseoldpos_.y();

    xptinterval_ += xPtInterval;
    yptinterval_ += yPtInterval;

    mouseoldpos_ = pos;
    this->update();
}

//����ע�����߲�����ע
void ImageLabel::Draw()
{
    QPen pen; //����
    QPen maskpen;
    pen.setWidth(pensize_);
    maskpen.setWidth(pensize_);
    switch (pentype_)
    {
    case PEN:
        pen.setColor(pencolor_);
        maskpen.setColor(maskpencolor_);
        break;
    case ERAS:
        pen.setBrush(srcpix_);
        maskpen.setBrush(Qt::black);
        break;
    default:
        return;
    }

    //�������Ӵ�
    maskpen.setCapStyle(Qt::RoundCap); //������ʽ
    maskpen.setJoinStyle(Qt::RoundJoin);//���Ӵ���ʽ
    QPainter pa(&maskpix_);
    //���ÿ����
    //if(pentype_!=PenType::ERAS){
    //    pa.setRenderHint(QPainter::Antialiasing, true);
    //}
    pa.setPen(maskpen);
    DrawLine(pa);

    //�������Ӵ�
    pen.setCapStyle(Qt::RoundCap); //������ʽ
    pen.setJoinStyle(Qt::RoundJoin);//���Ӵ���ʽ
    QPainter pe(&labelpix_);
    //���ÿ����
    //if (pentype_ != PenType::ERAS) {
    //    pe.setRenderHint(QPainter::Antialiasing, true);
    //}
    pe.setPen(pen);
    DrawLine(pe);

    update();
}

void ImageLabel::ResetZoom()
{
    zoomy_ = zoomoldy_;
    zoomx_ = zoomoldx_;
    xptinterval_ = 0;
    yptinterval_ = 0;
    mousepressed_ = false;
}

void ImageLabel::ZoomInImage(void)
{
    zoomx_ += 0.2;
    zoomy_ += 0.2;
    this->update();
}

void ImageLabel::ZoomOutImage(void)
{
    zoomx_ -= 0.2;
    zoomy_ -= 0.2;
    if (zoomx_ <= 0 || zoomy_ <= 0)
    {
        zoomx_ += 0.2;
        zoomy_ += 0.2;
        return;
    }

    this->update();
}

void ImageLabel::wheelEvent(QWheelEvent * event)
{
    if (blabelregion){
        int value = event->delta();
        if (value > 0)
            ZoomInImage();
        else
            ZoomOutImage();
        this->update();
    }
}


//������꺯��
void ImageLabel::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {        //������
        if (blabelregion) {
            if (pentype_ == PenType::COUNT_) {
                mousepressed_ = true;
                mouseoldpos_ = e->pos();
                return QWidget::mouseMoveEvent(e);
            }
            else {
                m_pointlist.clear();
                m_pointlist.push_back(QPoint(e->pos()));
                Draw();
            }
        }
        else{
            SendImageClicked(true);
            //emit signalLeftBtnClicked(e->globalPos());//�������
        }
    }
    else if(e->button() == Qt::RightButton) {   //����Ҽ�
        if (!blabelregion) {
            SendImageClicked(false);
            //emit signalSendPressPos(e->globalPos());
        }
    }
    else if (e->button() == Qt::MidButton) {    //����м�
        if (blabelregion) {
            ResetZoom();
            repaint();
        }

    }
}
void ImageLabel::keyPressEvent(QKeyEvent *event) 
{


    if (event->key() == Qt::Key_Up) {
        

    }
    if (event->key() == Qt::Key_Down) {
      
    }
  
}
void ImageLabel::mouseMoveEvent(QMouseEvent *e)
{
    if (blabelregion) {
        if (e->buttons() & Qt::LeftButton & Qt::LeftButton) {
            if (mousepressed_) {
                setCursor(Qt::SizeAllCursor);
                DrawMoveZoom(e->pos());
            }
            else {
                m_pointlist.push_back(QPoint(e->pos()));
                Draw();
            }
        }
        else {
            DrawPredictInfo(e->pos());
        }
    }
}

void ImageLabel::mouseReleaseEvent(QMouseEvent *event)
{
    mousepressed_ = false;
    if (pentype_ == PenType::COUNT_) {
        setCursor(Qt::ArrowCursor);
    }
}

void ImageLabel::ClearMenu()
{
    QAction * temp;
    if (imagemenu_.menu_nolabel != NULL){
        temp = (QAction*)imagemenu_.menu_nolabel;
        temp->setChecked(false);
    }
    if (imagemenu_.menu_train_always != NULL){
        temp = (QAction*)imagemenu_.menu_train_always;
        temp->setChecked(false);
        imagemenu_.b_train_always = false;
    }
    if (imagemenu_.menu_train_auto != NULL){
        temp = (QAction*)imagemenu_.menu_train_auto;
        temp->setChecked(false);
        imagemenu_.b_train_auto = false;
    }
    if (imagemenu_.menu_train_never != NULL){
        temp = (QAction*)imagemenu_.menu_train_never;
        temp->setChecked(false);
        imagemenu_.b_train_never = false;
    }
}
void ImageLabel::InitMenu()
{
    QAction * temp;
    QAction * chil;
    temp = new QAction(tr("&ɾ��"), this);
    addAction(temp);
    connect(temp, SIGNAL(triggered()), this, SLOT(triget_menu_delete()));
    imagemenu_.menu_delet = temp;
    temp = new QAction(tr("&ѵ��"), this);
    temp->setCheckable(true);

    QMenu *menu = new QMenu();
    //this->setStyleSheet("QMenu{background-color:white; border: 1px solid rgb(235,110,36);}");
    chil = new QAction(tr("&����"), menu);
    chil->setCheckable(true);
    menu->addAction(chil);
    connect(chil, SIGNAL(triggered()), this, SLOT(triget_menu_train_always()));
    imagemenu_.menu_train_always = chil;

    chil = new QAction(tr("&�Զ�"), menu);
    chil->setCheckable(true);
    menu->addAction(chil);
    connect(chil, SIGNAL(triggered()), this, SLOT(triget_menu_train_auto()));
    imagemenu_.menu_train_auto = chil;

    chil = new QAction(tr("&�Ӳ�"), menu);
    chil->setCheckable(true);
    menu->addAction(chil);
    connect(chil, SIGNAL(triggered()), this, SLOT(triget_menu_train_never()));
    imagemenu_.menu_train_never = chil;

    temp->setMenu(menu);
    addAction(temp);

    temp = new QAction(tr("&��ȱ��"), this);
    temp->setCheckable(true);
    addAction(temp);
    connect(temp, SIGNAL(triggered()), this, SLOT(triget_menu_nolabel()));
    imagemenu_.menu_nolabel = temp;
    temp = new QAction(tr("&�رչ�����"),this);
  //  temp->setCheckable(false);
    addAction(temp);
    connect(temp, SIGNAL(triggered()), this, SLOT(triget_menu_close_auto()));
    imagemenu_.menu_close_auto = temp;
    temp = new QAction(tr("&�򿪹�����"),this);
   // temp->setCheckable(true);
    addAction(temp);
    imagemenu_.menu_open_auto = temp;
    connect(temp, SIGNAL(triggered()), this, SLOT(triget_menu_open_auto()));

    temp = new QAction(tr("&����"), this);
    // temp->setCheckable(true);
    addAction(temp);
    imagemenu_.menu_test = temp;
    
    connect(temp, SIGNAL(triggered()), this, SLOT(triget_menu_test()));
    if (_isBlabelregion) {
        setContextMenuPolicy(Qt::ActionsContextMenu);
    }
    
}

void ImageLabel::ClearLabel()
{
    maskpix_.fill(Qt::black);
    labelpix_ = srcpix_;
    labeltypepix_.fill(Qt::transparent);
    ClearMenu();
    update();
}

void ImageLabel::SendImageClicked(bool is)
{
    emit signalSendId(is, (void*)this);
}

void ImageLabel::ResetPix()
{
    srcpix_.fill(Qt::black);
    predictpix_.fill(Qt::black);
    predicttypepix_.fill(Qt::black);
    labelpix_.fill(Qt::black);
    labeltypepix_.fill(Qt::black);
    maskpix_.fill(Qt::black);
    infopix_.fill(Qt::black);
    if (ainnoimage_ != NULL)
    {
        delete ainnoimage_;
        ainnoimage_ = NULL;
    }
}

//����ѡ��״̬
void ImageLabel::SetCheck(bool is)
{
    mousecheck_.state = is;
    if (is) {
        mousecheck_.pix.fill(QColor(0, 80, 180, 50));
    }
    else {
        mousecheck_.pix.fill(QColor(0, 0, 0, 0));
    }
    update();
}

bool ImageLabel::GetCheck()
{
    return mousecheck_.state;
}

void ImageLabel::SetNoLabel(bool is)
{
    SetMenuNoLabel(is);
    triget_menu_nolabel();
}

bool ImageLabel::GetNoLabel()
{
    auto temp = (QAction*)imagemenu_.menu_nolabel;
    return temp->isChecked();
}

void ImageLabel::SetMenuNoLabel(bool is)
{
    auto temp = (QAction*)imagemenu_.menu_nolabel;
    temp->setChecked(is);
}

//���û������ͣ����ʻ�����Ƥ��
void ImageLabel::SetPenType(PenType type)
{
    pentype_ = type;
}

void ImageLabel::ShowInfo()
{
    infopix_.fill(Qt::transparent);
    int fsize;
    QPainter pe(&infopix_);
    if (blabelregion) {
        fsize = 15;
    }
    else {
        fsize = 8;
    }
    QFont font("����", fsize, QFont::Bold, true);
    font.setItalic(false);
    //font.setLetterSpacing(QFont::AbsoluteSpacing, 10);  //���
    pe.setFont(font);
    pe.setPen(Qt::green);

    if (ainnoimage_->trained) {
        pe.drawText(0, 20, QString("%1 trained").arg(QString::fromLocal8Bit(ainnoimage_->filename.c_str())));
             }
    else {
        pe.drawText(0, 20, QString("%1").arg(QString::fromLocal8Bit(ainnoimage_->filename.c_str())));
    }
    pe.drawText(0, 40, QString("width:%1,height:%2").arg(srcimage_.width()).arg(srcimage_.height()));
}


//������
void ImageLabel::SaveMastImage(QString &path)
{
    maskpix_.save(path);
}

void ImageLabel::ShowLabel()
{
    bshowlabel_ = !bshowlabel_;
    if (bshowlabel_) {
        pentype_ = penoldtype_;
        if((pentype_ == PenType::ERAS)||(pentype_ == PenType::PEN)){
            //setCursor(Qt::CrossCursor);
        }
    }
    else {
        penoldtype_ = pentype_;
        pentype_ = PenType::COUNT_;
        //setCursor(Qt::ArrowCursor);
    }
    update();
}

void ImageLabel::ShowPredict()
{
    bshowpredict = !bshowpredict;
    update();
}

void ImageLabel::SetPenStyle()
{
    if ((pentype_ == PenType::PEN)|| (pentype_ == PenType::ERAS)) {
        penoldtype_ = pentype_;
        pentype_ = PenType::COUNT_;
        setCursor(Qt::ArrowCursor);
    }
    else
    {
        pentype_ = penoldtype_;
        if (pentype_ == PenType::PEN) {
            setCursor(Qt::CrossCursor);
        }
        else if(pentype_ == PenType::ERAS) {
            setCursor(Qt::OpenHandCursor);
        }
        else {
            setCursor(Qt::ArrowCursor);
        }
    }
    update();
}


void ImageLabel::SetPenSize(int size)
{
    pensize_ = size;
}

void ImageLabel::SetPenColor(QColor color)
{
    pencolor_ = color;
}

void ImageLabel::SetMaskPenColor(QColor color)
{
    maskpencolor_ = color;
}


void ImageLabel::SetPredictType(QPen pen)
{
    predicttypepix_ = QPixmap(width(), height());
    predicttypepix_.fill(Qt::transparent);
    pen.setWidth(4);
    QPainter pe(&predicttypepix_);
    pe.setPen(pen);
    pe.drawRect(2, 2, width() - 4, height() - 4);
    predicttypepen_ = pen;
}


void ImageLabel::SetPredictType(LabelRectFlag flag)
{
    QPen pen; //����

    switch (flag)
    {
    case LabelRectFlag::NONE_:
        return;
    case LabelRectFlag::RED:
        pen.setColor(Qt::red);
        break;
    case LabelRectFlag::GREEN:
        pen.setColor(Qt::green);
        break;
    default:
        return;
    }

    SetPredictType(pen);
    update();
}

void ImageLabel::SetLabelType(LabelRectFlag flag)
{
    labeltypepix_ = QPixmap(width(), height());
    labeltypepix_.fill(Qt::transparent);
    QPen pen; //����
    pen.setWidth(20);
    switch (flag)
    {
    case LabelRectFlag::NONE_:
        repaint();
        return;
    case LabelRectFlag::RED:
        pen.setBrush(Qt::red);
        break;
    case LabelRectFlag::GREEN:
        pen.setBrush(Qt::green);
        break;
    default:
        return;
    }
    //�����ϵı�
    QPainter pe(&labeltypepix_);
    pe.setPen(pen);
    int x = width() / 10;
    QLine line(width() - x, 0, width(), x);
    pe.drawLine(line);
    update();
}

void ImageLabel::UpdateLabeled()
{
    if (ainnoimage_) {
        if (ainnoimage_->labeled) {
            if (ainnoimage_->labeltype) {
                //bad
                SetLabelType(LabelRectFlag::RED);

            }
            else {
                //good
                SetLabelType(LabelRectFlag::GREEN);
                if (!blabelregion) {
                    labelpix_.fill(Qt::transparent);
                }
            }
        }
        else {
            SetLabelType(LabelRectFlag::NONE_);
        }
    }
    
    update();
}

void ImageLabel::ResizeWidetSize(int w, int h)
{
    scalex_ = (float)srcimage_.width() / (float)w;
    scaley_ = (float)srcimage_.height() / (float)h;
    if ((srcpix_.width() != w) && (srcpix_.height() != h)) {
        if (bkeepaspectratio_)
        {
                scaley_ = scalex_;
                zoomx_ = (float)w / (float)srcimage_.width();
                zoomy_ = (float)h / (float)srcimage_.height();
                zoomx_ = (zoomx_ >= zoomy_ )? zoomy_ : zoomx_;
                zoomy_ = zoomx_;

        }
        else
        {
            zoomx_ = (float)w / (float)srcimage_.width();
            zoomy_ = (float)h / (float)srcimage_.height();
            zoomoldy_ = zoomy_;
            zoomoldx_ = zoomx_;
        }
    }
    mousecheck_.pix = QImage(w, h, QImage::Format_RGBA8888);
    mousecheck_.pix.fill(Qt::transparent);

    UpdateLabeled();
    update();
}
bool ImageLabel::IsNoDefect() {
    QAction *temp = (QAction*)imagemenu_.menu_nolabel;
    return temp->isChecked();
}

void ImageLabel::UpdateMenuTrainCheck(MenuOperate menuoperate, ImageMenu imagemenu)
{
    switch (menuoperate)
    {
    case MenuOperate::O_NOLABEL:
        break;
    case MenuOperate::O_AUTO:   //�Զ�
        MenuTrainAuto(imagemenu.b_train_auto);
        break;
    case MenuOperate::O_NEVER:  //�Ӳ�
        MenuTrainNever(imagemenu.b_train_never);
        break;
    case MenuOperate::O_ALWAYS: //����
        MenuTrainAlways(imagemenu.b_train_always);
        break;
    }
}

bool ImageLabel::IsValid()
{
    return ainnoimage_?true:false;
}
/*void ImageLabel::contextMenuEvent(QContextMenuEvent *event)
{
    InitMenu();
}*/