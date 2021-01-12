#pragma once

#include <QWidget>
#include "qjsonarray.h"
#include "qjsonobject.h"
#include "qjsondocument.h"
#include "ui_runframetable.h"
#include "qmutex.h"
#include "uicommon.h"
#include <chrono>
class RunFrameTable : public QWidget
{
	Q_OBJECT

public:
	RunFrameTable(QWidget *parent = Q_NULLPTR);
	void SetTableStyle(bool issingle);
	void SetColWidth(QList<int> width);
	void ClearAndLoadTable();
	bool UpTableData();
	bool UpSqlTableData();
	~RunFrameTable();

private:
	Ui::RunFrameTable ui;
	QList<int> m_width;
	int m_swidth;
	bool m_issingle;
	QMutex mutex;
	
private:
	void clearJsonObject(QJsonObject &object);
	void ResizeTable(int newcol);

public:
	QList<int> m_swidth_list;
	int m_count;	//总数
	int m_goodcount;//良品
	int m_badcount; //劣品
	QString m_batch;
	int m_count_slave;		//从机总数
	int m_goodcount_slave;	//从机良品
	int m_ngcount_slave;	//从机次品
	std::map<int, long> m_solution_count;
	std::map<int, std::chrono::steady_clock::time_point> m_solution_time;
private:
	int m_comboxcount;

private slots:
	//处理从机接受到的统计信息
	void on_NewJsonData(const QJsonDocument &doc);	
	//处理主机接受到的统计信息
	void on_ChangeGrade(const QJsonDocument &doc);
	//处理从机接收到的请求刷新统计信息
	void on_NetUpTable();
	//档位变化时保存记录到检测方案
	void OnGradeChanged(int msg);
	//初始化档位数据
	void GradeInit();

};
