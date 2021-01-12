#pragma once
#include <QWidget>
#include "qgridlayout.h"
#include "qlabel.h"
class SolutionIcon
{

public:
	SolutionIcon(QWidget *parent = NULL);
	~SolutionIcon();
	//���õ�һ������ͼ
	void SetFristIcon();
	//�������һ������ͼ
	void SetLastIcon();
	//�����м�ı���ͼ
	void SetNormalIcon();
	//������
	void Settled();
	//δ����
	void UnSettled();
	//��������
	void Setting();
	//����� 1�����ã�0δ����
	bool m_issettled;
	
public:
	QGridLayout* m_pgridlayout;
	QLabel* m_lab;
	QWidget* m_widget;
};
