#include "sysStatusFrame.h"
sysStatusFrame* sysStatusFrame::P_sysStatusFrame = Q_NULLPTR;
sysStatusFrame::sysStatusFrame(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    ui.label_proList->setText("<a style='color: blue;'href = baidu.com>工程列表</a>");
    _proListForm = new proNameListForm;//解决方案列表
    connect(ui.label_proList, SIGNAL(linkActivated(const QString&)), this, SLOT(slotLabelProList(const QString&)));
    connect(this, SIGNAL(signalStatusStr(ststusID, const QString &)), this, SLOT(slotStatusStr(ststusID, const QString &)));
    connect(ui.comboBox_page, SIGNAL(currentTextChanged(const QString &)), this, SIGNAL(signalPageIndex(const QString &)));
}
 sysStatusFrame* sysStatusFrame::instance()
{
     if (P_sysStatusFrame == NULL) {
         P_sysStatusFrame = new sysStatusFrame;
     }
     return P_sysStatusFrame;
}
sysStatusFrame::~sysStatusFrame()
{
    //Release();
}
void sysStatusFrame::slotStatusStr(ststusID flag, const QString &statusStr)
{
    switch (flag)
    {
    case ststusID::Loading: {
        ui.label_load_status->setText(statusStr);
    }
        break;
    case ststusID::PageNum: {
        ui.label_page->setText(statusStr);
    }
        break;
    case ststusID::Pro: {
        ui.label_pro->setText(statusStr);
    }
        break;
    default:
        break;
    }
}
void sysStatusFrame::comboBox_pageAddItem(int size)
{
    ui.comboBox_page->clear();
    for (int i = 0; i < size; i++) {
        ui.comboBox_page->addItem(QString::number(i + 1));
    }
}
void sysStatusFrame::setCurrentIndex(int index)
{
    if (ui.comboBox_page->count() > 0) {
        ui.comboBox_page->setCurrentIndex(index-1);
   }
}
void sysStatusFrame::slotLabelProList(const QString&link)
{
    //工程列表界面
    _proListForm->showForm();
}
void sysStatusFrame::setProIni(const QString key, QVariant value)
{
    _proListForm->setIniFile(key, value);
}