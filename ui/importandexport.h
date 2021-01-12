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
	//�����Ի���
	void* m_excelexport;
	//����excel��
	void* m_excelEngine;
	//��ʼ���б�
	void InitSqlTable();
	//������excel
	bool IntoExcel(QString filename, QTableWidget* tableinto);
	//���Ͳ�Ʒ��ѯ���󵽴ӻ�
	void SendProductQueryRequest(QString str);
	//���ܴӻ���Ʒ����
	void OnSendProduct(const QJsonDocument & doc);
	//��Ʒ���ѯ
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
	
	//�����Զ����ź�
	void OnSendMsgProduct(QString path);
	void OnSendMsgStation(QString path);
	void OnSendMsgBatch(QString path);



private:
	//��ǰ��� 1���Σ�2��λ��3��Ʒ
	int m_currentable;
	//��ǰ��
	int m_currenrow;
};
