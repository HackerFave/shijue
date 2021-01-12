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
//#define CRACK "crack"				//裂纹
//#define DROPBLOCK "dropblock"		//掉块
#define GEAR0 "gear0"				//档位0 最宽松
#define GEAR1 "gear1"				//档位1 宽松
#define GEAR2 "gear2"				//档位2 正常
#define GEAR3 "gear3"				//档位3 严格
#define GEAR4 "gear4"				//档位4 最严格
// grear4 = (float)裂纹长，(float)裂纹宽，(float)掉块长，(float)掉块宽

#define SGEAR0 "最宽松"
#define SGEAR1 "宽松"
#define SGEAR2 "正常"
#define SGEAR3 "严格"
#define SGEAR4 "最严格"



//PLC
#define PLCIP "plcip"//"PLC的ip地址"
#define PLCPORT "plcport"//"PLC端口"
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
//裂块调试配置文件
#define LIEKUAIINI "algparam.ini"
//大小边调试配置文件
#define DAXIAOBIANINI "daxiaobian.ini"


//数据库
#define SQLCOUNT "Count"
#define SQLGOOD "Good"
#define SQLBAD "Bad"
//批次表
#define BATCHTABLE "batches"
#define BATCHES "Batch"

//工位表
#define STATIONTABLE "station"
#define STATIONID "StationID"
#define STATIONGRADE "Grade" 

//产品表
#define PRODUCTTABLE "products"
#define PRODUCTID "ProductID"
#define ISGOOD "IsGood"
#define PICADDR "Addr"
#define PSQLTIME "CreateTime"



//PLC配置文件
#define PLCINI "plc.ini"

#define GROUP_DCF "dcf"
#define DLQD "dlqd"		//挡料启动
#define SSJ1 "ssj1"		//输送机1
#define SSJ2 "ssj2"		//输送机2
#define SSJ3 "ssj3"		//输送机3
#define SSJ4 "ssj4"		//输送机4
#define SSJ5 "ssj5"		//输送机5
#define SSJ6 "ssj6"		//输送机6
#define SSJ7 "ssj7"		//输送机7
#define SSJ8 "ssj8"		//输送机8
#define SSJ9 "ssj9"		//输送机9
#define SSJ10 "ssj10"	//输送机10
#define SSJ11 "ssj11"	//输送机11
#define SSJ12 "ssj12"	//输送机12
#define SSJ13 "ssj13"	//输送机13
#define SSJ14 "ssj14"	//输送机14

#define GROUP_TC "tc"
#define TCK1 "tck1"		//剔除口1
#define TCK2 "tck2"		//剔除口2
#define TCK3 "tck3"		//剔除口3
#define TCK4 "tck4"		//剔除口4
#define TCK5 "tck5"		//剔除口5

#define GROUP_TCSJ "tc"
#define TCKSJ1 "tcksj1"		//剔除口时间1
#define TCKSJ2 "tcksj2"		//剔除口时间2
#define TCKSJ3 "tcksj3"		//剔除口时间3
#define TCKSJ4 "tcksj4"		//剔除口时间4
#define TCKSJ5 "tcksj5"		//剔除口时间5
#define LPCLKSJ "lpclksj"	//良品出料口启停时间


#define USECAMERADLL   //使用相机库