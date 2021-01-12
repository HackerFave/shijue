#pragma once
#pragma execution_character_set("utf-8")
#define SOLUTION	"/solution/"
#define SOLUTION_   "Solution"
#define ININAME		"solution.ini"
#define ININAME_SYSTEM "System.ini"
#define ININAME_PLC "plc.ini"
#define CAMERANAME	"CameraName"
#define CAMERAID	"CameraId"
#define CAMERATYPE  "CameraType"
#define ISLAST		"IsLast"
#define SGROUP		"Group"
#define ALG			"Alg"
#define ROI			"roi"
#define FRAMEID		"FrameId"
#define MASTER		"Master"
#define GRADE		"grade"
#define ISUSED      "IsUsed"
#define MAXCAMERA	30
#define SATATIONNUM 30
#define SATATIONNUMTABLE 60

//socket
#define CIWASOCKETININAME	"mcc.ini"
#define ISMASTER			"ismaster"
#define SERVERIP			"serverip"
#define SERVERPORT			"serverport"
#define LOCALIP				"localip"
#define LOCALPROT			"loacalprot"
#define OPENNET				"opennet"
#define COMMUNICATION		"communication"
#define READ_RAW_LEN		4


//threshold.ini
#define THRESHOLDNAME "threshold.ini"
//#define CRACK "crack"				//����
//#define DROPBLOCK "dropblock"		//����
#define GEAR0 "gear0"				//��λ0 �����
#define GEAR1 "gear1"				//��λ1 ����
#define GEAR2 "gear2"				//��λ2 ����
#define GEAR3 "gear3"				//��λ3 �ϸ�
#define GEAR4 "gear4"				//��λ4 ���ϸ�
// grear4 = (float)���Ƴ���(float)���ƿ�(float)���鳤��(float)�����

#define SGEAR0 "�����"
#define SGEAR1 "����"
#define SGEAR2 "����"
#define SGEAR3 "�ϸ�"
#define SGEAR4 "���ϸ�"



//PLC
#define PLCIP "plcip"//"PLC��ip��ַ"
#define PLCPORT "plcport"//"PLC�˿�"
#define DEBUGTIME


//fenp.ini
#define FENPINI "fenp.ini"
#define ISOPENCUSTOM "isopencustom"
#define COPEN "open"
#define FWINDOWNUM "windownum"
#define AUTOOPEN "autoopen"


//systemconfig
#define GOURP1 3
#define NUM_ALARM_STATUS 7
#define NUM_SHU_SONG_JI_LOW 7
#define NUM_ALARM_STATUS_HIGH 8


#define CIWAUI
//�ѿ���������ļ�
#define LIEKUAIINI "algparam.ini"
//��С�ߵ��������ļ�
#define DAXIAOBIANINI "daxiaobian.ini"


//���ݿ�
#define SQLCOUNT "Count"
#define SQLGOOD "Good"
#define SQLBAD "Bad"
//���α�
#define BATCHTABLE "batches"
#define BATCHES "Batch"

//��λ��
#define STATIONTABLE "station"
#define STATIONID "StationID"
#define STATIONGRADE "Grade" 

//��Ʒ��
#define PRODUCTTABLE "products"
#define PRODUCTID "ProductID"
#define ISGOOD "IsGood"
#define PICADDR "Addr"
#define PSQLTIME "CreateTime"



//PLC�����ļ�
#define PLCINI "plc.ini"

#define GROUP_DCF "dcf"
#define DLQD "dlqd"		//��������
#define SSJ1 "ssj1"		//���ͻ�1
#define SSJ2 "ssj2"		//���ͻ�2
#define SSJ3 "ssj3"		//���ͻ�3
#define SSJ4 "ssj4"		//���ͻ�4
#define SSJ5 "ssj5"		//���ͻ�5
#define SSJ6 "ssj6"		//���ͻ�6
#define SSJ7 "ssj7"		//���ͻ�7
#define SSJ8 "ssj8"		//���ͻ�8
#define SSJ9 "ssj9"		//���ͻ�9
#define SSJ10 "ssj10"	//���ͻ�10
#define SSJ11 "ssj11"	//���ͻ�11
#define SSJ12 "ssj12"	//���ͻ�12
#define SSJ13 "ssj13"	//���ͻ�13
#define SSJ14 "ssj14"	//���ͻ�14

#define GROUP_TC "tc"
#define TCK1 "tck1"		//�޳���1
#define TCK2 "tck2"		//�޳���2
#define TCK3 "tck3"		//�޳���3
#define TCK4 "tck4"		//�޳���4
#define TCK5 "tck5"		//�޳���5

#define GROUP_TCSJ "tc"
#define TCKSJ1 "tcksj1"		//�޳���ʱ��1
#define TCKSJ2 "tcksj2"		//�޳���ʱ��2
#define TCKSJ3 "tcksj3"		//�޳���ʱ��3
#define TCKSJ4 "tcksj4"		//�޳���ʱ��4
#define TCKSJ5 "tcksj5"		//�޳���ʱ��5
#define LPCLKSJ "lpclksj"	//��Ʒ���Ͽ���ͣʱ��


#define USECAMERADLL   //ʹ�������