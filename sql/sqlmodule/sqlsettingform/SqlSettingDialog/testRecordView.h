#pragma once

#include <QTableView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <qdebug.h>
#include <QMouseEvent>
#include <QModelIndex>
#include <QToolTip>
#include <qtimer.h>
#include <QDateTime>
#include "ui_testRecordView.h"
#pragma execution_character_set("utf-8")
class testRecordView : public QTableView
{
    Q_OBJECT

public:
    testRecordView(QWidget *parent = Q_NULLPTR);
    ~testRecordView();
    void initTableView(const QStringList &list, QString head);
	void iniTableView();
    void editTable(const QList<QVariantList> &list);
    void addRowItem();
    void addRowItem(const QMap<QString,QString> &data, const QStringList &keyList);
    void tableViewClear();
    bool getClearStatus();
    void setToolTipModel();
    void setTestRecordItem(std::map<std::string, std::string> map);
    void setCurrentRecordId(const QString &record);
   // void addTestData();//添加测试数据
    //void stopUpdate();
    //void startUpdate();
   // void updataItem(const QString &recordID);
	void findCurPageRecord(const QString &curStr);
    //bool timerStatusisActive();
	void setCurrentRowDataSize(QString sizeStr) {
		_rowDataSizeStr = sizeStr;
	}
	QString getCurrentRowDataSize() const{
		return _rowDataSizeStr;
	}
	//QList<QVariantList> getTestRecordCurrentRowData(); 
    void setrelatedid(const QString & relatedid);
protected:
signals:
	void signalComboxPageUpdate(const QString&,int);
    void signalItemText(const QString &);
private slots:
    void slotDoubleClicked(const QModelIndex &);
    void slotEditItemData(int row, int column, QString text);
    void showToolTip(const QModelIndex &index);
   
private:
    Ui::testRecordView ui;
    bool _isClear = false;
    QTimer *_timer = Q_NULLPTR;
    QString relatedid_;
    QList<QVariantList> _currentRowData;//当前操作数据
	QString _rowDataSizeStr;//当前检测数据条数
	int _curPage;//当前页
};
