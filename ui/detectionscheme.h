#pragma once

#include <QWidget>
#include <vector>
#include "ui_detectionscheme.h"
#include "detectionschemeroi.h"
#include <opencv2/opencv.hpp>
#include "solutionicon.h"
class DetectionScheme : public QWidget
{
	Q_OBJECT

public:
	DetectionScheme(QWidget *parent = Q_NULLPTR);
	~DetectionScheme();
	
private:
	Ui::DetectionScheme ui;
	int m_stationnum;		//��λ��
	int m_groupnum;			//����
	QString m_saveini;		//��鷽����
	std::vector<SolutionIcon*> m_vectorSolu;
	//��ʼ���㷨�б�
	void InitAlgList();
protected slots:
	void on_btnRoi_clicked();
	void on_btnDelRoi_clicked();
	void on_btnSave_clicked();
	void on_btntest_clicked();
	void on_btnExit_clicked();
	void on_checkLast_clicked();
	void SetParam(QString text);

protected:	
	//��ӽ���ͼ��
	void AddStationPro(int stationnum);
public:
	void UpdataIcon();
	void mouseMoveEvent(QMouseEvent * event);
	void closeEvent(QCloseEvent *event);

	void setStationNum(int stationnum);
	void setGroupNum(int groupnum);
	int getStationNum() { return m_stationnum; }
	int getGroupNum(){ return m_groupnum; }
	
	QString getSavePath() { return m_saveini; }

signals:
    void SendTriggerClose(int);//����״̬����Ϣ signals:

private slots:
    void OnSendTriggerClose(int);
    void OnCameraType();

};
