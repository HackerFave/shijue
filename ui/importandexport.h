#pragma once

#include <QWidget>
#include "ui_importandexport.h"
#include "qjsondocument.h"
class ImportAndExport : public QWidget
{
	Q_OBJECT

public:
	ImportAndExport(QWidget *parent = Q_NULLPTR, QRect rect = QRect());
	~ImportAndExport();

private:
	Ui::ImportAndExport ui;
	//导出对话框
	void* m_excelexport;
	//导出excel类
	void* m_excelEngine;
	//初始化列表
	void InitSqlTable();
	//导出到excel
	bool IntoExcel(QString filename, QTableWidget* tableinto);
	//发送产品查询请求到从机
	void SendProductQueryRequest(QString str);
	//接受从机产品数据
	void OnSendProduct(const QJsonDocument & doc);
	//检品表查询
	void ProductQuery(QString batch, QString station, int page);




private slots:
	void on_btnEnd_clicked();
	void on_btnNext_clicked();
	void on_btnExport_clicked();
	void on_btnIndexOK_clicked();
	void on_btnPrevious_clicked();
	void on_btnDeleteSQL_clicked();
	void on_btnSelectSQL_clicked();
	void on_btnDatabaseBack_clicked();
	void on_btnDatabaseRest_clicked();
	void on_tableBatch_cellClicked(int row,int col);
	void on_tableStation_cellClicked(int row, int col);
	void on_tableProducts_cellClicked(int row, int col);
	
	//接收自定义信号
	void OnSendMsgProduct(QString path);
	void OnSendMsgStation(QString path);
	void OnSendMsgBatch(QString path);



private:
	//当前表格 1批次，2工位，3检品
	int m_currentable;
	//当前行
	int m_currenrow;
};
