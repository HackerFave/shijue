#include "messageboxgui.h"
#include <qdebug.h>
#include "ui_messageboxgui.h"
MessageBoxGui::MessageBoxGui(QDialog *parent)
    : QDialog(parent)
    , ui(new Ui::MessageBoxGui)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    this->setModal(true);
    ui->label_hint->adjustSize();
    //ui->label_head->setVisible(false);
    //2. 让QLabel能够自动判断并换行显示：
    ui->label_hint->setGeometry(QRect(328, 240, 329, 27 * 4)); //四倍行距
    ui->label_hint->setWordWrap(true);
    ui->label_hint->setAlignment(Qt::AlignTop);
    ui->toolbutton_close->setIcon(QIcon(tr("%1/icons/ic_solution_exit.png").arg(qApp->applicationDirPath())));
    connect(ui->toolbutton_close, SIGNAL(clicked()), this, SLOT(SlotCancel()));
    connect(ui->pushbutton_ok, SIGNAL(clicked()), this, SLOT(SlotOk()));
    connect(ui->pushbutton_cancel, SIGNAL(clicked()), this, SLOT(SlotCancel()));
    ok_cancel_flag_ = false;
}

MessageBoxGui::~MessageBoxGui()
{
}

void MessageBoxGui::SetMessageHint(QString &icon, QString &strhead, QString &strhint)
{

    QImage Image;
    Image.load(icon);
    QPixmap pixmap = QPixmap::fromImage(Image);
    int with = ui->label_icon->width();
    int height = ui->label_icon->height();
    QPixmap fitpixmap = pixmap.scaled(with, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);  // 按比例缩放
    ui->label_icon->setAlignment(Qt::AlignCenter);
    ui->label_icon->setPixmap(fitpixmap);

    ui->label_head->setText(strhead);
    ui->label_hint->setAlignment(Qt::AlignCenter);
    ui->label_hint->setText(strhint);
}
//ok // 取消
void MessageBoxGui::MessageOperT(QString tips)
{
	ui->pushbutton_cancel->setVisible(true);

	QString strHead;
	QPixmap pixmap;
	
	pixmap = QPixmap(tr("%1/icons/ic_success.png").arg(qApp->applicationDirPath()));
	strHead = "提示";
	ui->label_icon->setPixmap(pixmap);
	int with = ui->label_icon->width();
	int height = ui->label_icon->height();
	ui->label_icon->setAlignment(Qt::AlignCenter);
	ui->label_head->setText(strHead);
	ui->label_hint->setAlignment(Qt::AlignCenter);
	//QString strTemp = QString::fromLocal8Bit(strHint.c_str());
	ui->label_hint->setText(tips);
}
//strhint 为空：成功； ？ 警告； 其他：报警；
void MessageBoxGui::SetMessageHint(HINT_FLAG flag, QString strHint)
{
    ui->pushbutton_cancel->hide();
    QString strHead;
    QPixmap pixmap;
    switch (flag)
    {
    case HINT_FLAG::MRUN_WARNING:
    {
        pixmap = QPixmap(tr("%1/icons/ic_warning.png").arg(qApp->applicationDirPath()));
        strHead = "警告";
    }
        break;
    case HINT_FLAG::MRUN_ERROR:
    {
        pixmap = QPixmap(tr("%1/icons/ic_error.png").arg(qApp->applicationDirPath()));
        strHead = "错误";
    }
        break;
    case HINT_FLAG::MRUN_HINT:
    {
        pixmap = QPixmap(tr("%1/icons/ic_success.png").arg(qApp->applicationDirPath()));
        strHead = "提示";
    }
        break;
    default:
        break;
    }
    ui->label_icon->setPixmap(pixmap);
    int with = ui->label_icon->width();
    int height = ui->label_icon->height();
    ui->label_icon->setAlignment(Qt::AlignCenter);
    ui->label_head->setText(strHead);
    ui->label_hint->setAlignment(Qt::AlignCenter);
    //QString strTemp = QString::fromLocal8Bit(strHint.c_str());
    ui->label_hint->setText(strHint);
}

bool MessageBoxGui::GetCurrentIndex()
{
    return ok_cancel_flag_;
}
void MessageBoxGui::SetFontSize(int size, int indexLabel)
{
    switch (indexLabel)
    {
    case 0:
    {
        QFont ft;
        ft.setPointSize(size);
        ui->label_head->setFont(ft);
    }
    break;
    case 1:
    {
        QFont ft;
        ft.setPointSize(size);
        ui->label_hint->setFont(ft);
    }
    break;
    default:
        break;
    }
}
void MessageBoxGui::SetFontColor(QColor color, int indexLabel)
{

    switch (indexLabel)
    {
    case 0:
    {
        QPalette pa;
        pa.setColor(QPalette::WindowText, color);
        ui->label_head->setPalette(pa);
    }
    break;
    case 1:
    {
        QPalette pa;
        pa.setColor(QPalette::WindowText, color);
        ui->label_hint->setPalette(pa);
    }
    break;
    default:
        break;
    }
}
void MessageBoxGui::SetIconSize(int width, int height)
{
    ui->label_icon->setFixedSize(QSize(width, height));
}
void MessageBoxGui::SlotOk()
{
    //emit signalIndexBtn(true);
    this->close();
    ok_cancel_flag_ = true;
}
void MessageBoxGui::SlotCancel()
{
    //emit signalIndexBtn(false);
    this->close();
    ok_cancel_flag_ = false;
}
void MessageBoxGui::mousePressEvent(QMouseEvent *event)
{
    if (ui->frame_title->geometry().contains(this->mapFromGlobal(QCursor::pos()))) {
        lastpos_ = event->globalPos();
        ispressedwidget_ = true; // 当前鼠标按下的即是QWidget而非界面上布局的其它控件
    }

}

void MessageBoxGui::mouseMoveEvent(QMouseEvent *event)
{
    if (ispressedwidget_) {
        this->move(this->x() + (event->globalX() - lastpos_.x()),
            this->y() + (event->globalY() - lastpos_.y()));
        lastpos_ = event->globalPos();
    }
}

void MessageBoxGui::mouseReleaseEvent(QMouseEvent *event)
{
    // 其实这里的mouseReleaseEvent函数可以不用重写
    lastpos_ = event->globalPos();
    ispressedwidget_ = false; // 鼠标松开时，置为false
}