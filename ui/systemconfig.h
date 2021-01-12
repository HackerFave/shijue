#pragma once

#include <QWidget>
#include "ui_systemconfig.h"
#include "customtabstyle.h"
#include "systeminfo.h"
#include "cpumemorylabel.h"
#include "gpumemorylabel.h"
#include "writeregistermodel.h"
#include "../socket/ciwasocket.h"
#include <QStandardItemModel>
#include <QElapsedTimer>

class SystemConfig : public QWidget
{
	Q_OBJECT

public:
	SystemConfig(QWidget *parent = Q_NULLPTR, QRect rect = QRect());
	~SystemConfig();
	void SetSocketLab(bool is);
private:
	GpuMemoryLabel *m_gpu;
	int m_plcindex;
	bool m_isnetwork;
	bool m_istabinit[7];
private:
	Ui::SystemConfig ui;
	SystemInfo *m_systeminfo;
	QString m_currcam;
	QStringList m_listwindow;
	void *m_pwindow;
    void *m_cameratable;
    WriteRegisterModel *writeModel;
	std::vector<QString> valuse_alarm_status_names;
	std::vector<QString> valuse_shusongji_low_names;
	std::vector<QString> valuse_shusongji_high_names;
	std::vector<int> valuse_shebeimoshi;

	void InitTab();
	//��������
	void InitTab1();
	//�û�����
	void InitTab2();
	//��������
	void InitTab3();
	//�������
	void InitTab4();
	//��Ϣϵͳ
	void InitTab5();
	//��λ��ʾ
	void InitTab8();
	//����������
	void GetCameraParam(QString camname);
	//�����������
	void SetCameraParam(QString camname);
	//��ʼ������״̬ID
	void InitAlarmTable();
	//�˺Ź���
	bool UserManage(bool is);
	//���������
	void ClearUserEdit();

	void InitTab0();

	int GetBitStatu(int num, int pos);
private slots:
	void OnCellClicked(int r, int c);
	//�������������ť
	void on_btnSet_clicked();
	//�����û������밴ť
	void on_modifypwd_clicked();
	//����û���ť
	void on_addpwd_clicked();
	//ɾ���û���ť
	void on_delpwd_clicked();
	//������԰�ť
	void on_btnNetTest_clicked();
	//��������
	void on_ptnNetwork_clicked();
	//��������
	void on_btnSocketSave_clicked();
	//��Ϊ������
	void on_radioIsMaster_clicked();
	//��Ϊ�ӻ���
	void on_radioIsSlave_clicked();
	//UI���Բ�
	void on_btnUITest_clicked();
	//UI���Բ�2
	void on_btnUITest_2_clicked();
	//�������ñ༭
	void on_btnEdit_clicked();
	//��������ȡ��
	void on_btnSocketChanel_clicked();
	//�����Զ��忪��
	void on_btnSwitch_change(bool is);
	//����Զ���ˮƽ��ת����
	void on_btnSPFZ_change(bool is);
	//������ò鿴
	void OnCameraConfigClicked();
	//������ñ༭
	void on_btnCameraEdit_clicked();
	//���ȡ������
	void on_btnReload_clicked();
	//��ʾ��λˢ��
	void on_btnShowRefresh_clicked();
	//��Ļѡ��
	void OnDDisplay(int index);
	//�ӿ�ѡ��
	void OnDWindow(int index);
	//�ӿ�����
	void on_btnShowReset_clicked();
	//�ӿڱ���
	void on_btnShowSave_clicked();
    //����Ͽ�
    void on_btnCloseDevice_clicked();
    //�������
    void on_btnReLoadDevice_clicked();

	void OnSaveBaseParam();
	void OnReadBaseParam();

	void OnTabChange(int index);

public:
	signals:
	void SendCustomFenpSignal();
};
