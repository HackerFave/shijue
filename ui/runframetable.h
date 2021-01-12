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
	int m_count;	//����
	int m_goodcount;//��Ʒ
	int m_badcount; //��Ʒ
	QString m_batch;
	int m_count_slave;		//�ӻ�����
	int m_goodcount_slave;	//�ӻ���Ʒ
	int m_ngcount_slave;	//�ӻ���Ʒ
	std::map<int, long> m_solution_count;
	std::map<int, std::chrono::steady_clock::time_point> m_solution_time;
private:
	int m_comboxcount;

private slots:
	//����ӻ����ܵ���ͳ����Ϣ
	void on_NewJsonData(const QJsonDocument &doc);	
	//�����������ܵ���ͳ����Ϣ
	void on_ChangeGrade(const QJsonDocument &doc);
	//����ӻ����յ�������ˢ��ͳ����Ϣ
	void on_NetUpTable();
	//��λ�仯ʱ�����¼����ⷽ��
	void OnGradeChanged(int msg);
	//��ʼ����λ����
	void GradeInit();

};
