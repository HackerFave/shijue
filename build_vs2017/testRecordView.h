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
#include "databaseSqlInset.h"
#include "DatabaseSqlite.h"
#include "sqlOperation.h"
#include "tableViewEditItemForm.h"
#include "ui_testRecordView.h"
#pragma execution_character_set("utf-8")
class testRecordView : public QTableView
{
    Q_OBJECT

public:
    testRecordView(QWidget *parent = Q_NULLPTR);
    ~testRecordView();
    void initTableView();
    void editTable(const QList<QVariantList> &list);
    void addRowItem();
    void addRowItem(const QMap<QString,QString> &data);
    void tableViewClear();
    bool getClearStatus();
    void setToolTipModel();
    void setTestRecordItem(std::map<std::string, std::string> map);
    void setCurrentRecordId(const QString &record);
    //void addTestData();//添加测试数据
    void stopUpdate();
    void startUpdate();
    void updataItem(const QString &recordID);
	void findCurPageRecord(const QString &curStr);
    bool timerStatusisActive();
	void setCurrentRowDataSize(QString sizeStr) {
		_rowDataSizeStr = sizeStr;
	}
	QString getCurrentRowDataSize() const{
		return _rowDataSizeStr;
	}
	QList<QVariantList> getTestRecordCurrentRowData(); 
	void setCurPage(int value)//当前页
	{
		_curPage = value;
	}
	void findCurrentData();
protected:
signals:
	void signalComboxPageUpdate(const QString&,int);
	void signalOldDataClear(bool);
	void signalGetSpinBoxValue();
private slots:
    void slotDoubleClicked(const QModelIndex &);
    void slotEditItemData(int row, int column, QString text);
    void showToolTip(const QModelIndex &index);
    void slotUpdataItem();
	void slotConcessionCheckedStatus(bool);
	void slotOutprintCheckedStatus(bool);
private:
    Ui::testRecordView ui;
    bool _isClear = false;
    QTimer *_timer = Q_NULLPTR;
    QString _recordID;
    QList<QVariantList> _currentRowData;//当前操作数据
    sqlOperation * p_sqlOperation = Q_NULLPTR;//操作数据库
	QString _rowDataSizeStr;//当前检测数据条数
	int _curPage;//当前页
};
