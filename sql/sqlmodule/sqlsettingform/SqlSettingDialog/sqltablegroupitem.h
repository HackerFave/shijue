#pragma once

#include <QDialog>
#include "sqliditemlabel.h"
#include <QMap>
namespace Ui
{
    class SqlTableGroupItem;
}
class SqlTableGroupItem : public QDialog
{
    Q_OBJECT

public:
    SqlTableGroupItem(QWidget *parent = Q_NULLPTR);
    ~SqlTableGroupItem();
    void SetGroupBoxTitle(const QString &title);
    void SetIdItemList(const QStringList &listitem);
    void SetGroupBoxChecked(bool checked);
    void GetLayoutItems(QLayout *layout, bool checked);
private slots:
    void SlotGroupChecked(bool checked);
private:
    Ui::SqlTableGroupItem *ui;
    QMap<QString, SqlIdItemLabel*> map_id_label_;//groupboxÀïµÄ×Ö¶Î¿Ø¼þ
};
