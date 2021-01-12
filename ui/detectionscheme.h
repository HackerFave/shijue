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
	int m_stationnum;		//工位数
	int m_groupnum;			//组数
	QString m_saveini;		//检查方案名
	std::vector<SolutionIcon*> m_vectorSolu;
	//初始化算法列表
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
	//添加进度图标
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
    void SendTriggerClose(int);//发送状态栏信息 signals:

private slots:
    void OnSendTriggerClose(int);
    void OnCameraType();

};
