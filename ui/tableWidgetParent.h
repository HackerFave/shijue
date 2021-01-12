#ifndef TABLEWIDGETPARENT_H
#define TABLEWIDGETPARENT_H

#include <QWidget>
#include <QTableWidgetItem>
#include <QTabWidget>
#include <QIcon>
#include <QImage>
#include <QLabel>
#include <QBrush>
#include <QToolButton>
namespace Ui {
class tableWidgetParent;
}

class tableWidgetParent : public QWidget
{
    Q_OBJECT

public:
    explicit tableWidgetParent(QWidget *parent = 0);
    ~tableWidgetParent();
    void init(const QStringList & headerList);
    void setRowColumn(const int row,const int column);
    bool isResizeFont(bool isResizeFont) ;
    void addItemIcon(int row ,int column,const QString &iconName);
    void addTtemButton(int row,int column,QToolButton * button);
    void setDataItem(int row, int column,const QVariant &dataStr);
    void setItem(int row,int column,QTableWidgetItem *item);
    void setHeaderItemIcon(int column, QString &icon);
    void initHeaderStyle(const QString &iconName);
    int getRowCount() const;
    int getColumnCount() const;
    void firstLineCenter(bool isOk);
    bool isFirstLineCenter() const;
    void setFirstLineCenter();
    void setHeaderColumnPosition(int column , Qt::AlignmentFlag flag);
    void setItemPosition(int row,int column, Qt::AlignmentFlag flag);
	//获取当前选中
	QModelIndex getIndex(int x, int y);
	//获取指定行列值
	QString getTableData(int row, int column);
	//清空表格数据
	void clearTableData();
	//获取表格指针
	void* getTablePoint();

public slots:
	void slotTip(int a, int b);
private:
	void initHeaderList(const QStringList &list);
    Ui::tableWidgetParent *ui;
    int _Row=10;
    int _Column=10;
    bool _isCenter=false;
};

#endif // TABLEWIDGETPARENT_H
