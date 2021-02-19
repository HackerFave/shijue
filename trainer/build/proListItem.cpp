#include "proListItem.h"

proListItem::proListItem(QWidget *parent, QString id)
    : QWidget(parent),
    _id(id)
{
    ui.setupUi(this);
    this->setMouseTracking(true);
}

proListItem::~proListItem()
{
}
void proListItem::on_toolButton_status_clicked(bool checked)
{
    emit signalStatus(checked,_id,ui.label_proName->text().simplified());
}
void proListItem::on_toolButton_select_clicked(bool checked)
{
    emit signalSelect(checked,_id,ui.label_proName->text().simplified());
}
void proListItem::setProName(const QString &textProName)
{
    ui.label_proName->setText(textProName);
}
void proListItem::mouseDoubleClickEvent(QMouseEvent * ev)

{
    if (ev->button() == Qt::LeftButton) {
        if (ui.label_proName->geometry().contains(ev->pos())) {
            emit signalSelect(true, _id, ui.label_proName->text().simplified());
        }
    }

}
