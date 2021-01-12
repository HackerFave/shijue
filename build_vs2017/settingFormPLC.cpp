#include "settingFormPLC.h"
#include <windows.h>
#include <qsettings.h>
#include "uicommon.h"
#include <plcinterface.h>
#include "log.h"

settingFormPLC::settingFormPLC(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
	initcomm();
}

settingFormPLC::~settingFormPLC()
{
}

void settingFormPLC::initcomm()
{
	QString path = "HKEY_LOCAL_MACHINE\\HARDWARE\\DEVICEMAP\\SERIALCOMM";
	QSettings *settings = new QSettings(path, QSettings::NativeFormat);
	QStringList key = settings->allKeys();
	QStringList comlist;
	QStringList baudlist;
	QStringList paritylist;
	QStringList databitslist;
	QStringList stopbitslist;
	QStringList ControlFlowlist;

	ui.tableplctemp->horizontalHeader()->setStyleSheet("QHeaderView::section{background:#808080;}");

	comlist.clear();
	for (int i = 0; i < 100; i++) {
		comlist << QString("COM%1").arg(i);
	}
	ui.combocomlist_2->addItems(comlist);

	baudlist.clear();
	baudlist << "300" << "600" << "2400" << "4800" << "9600" << "19200" <<
		"56000" << "57600" << "115200" << "128000" << "256000" << "921600";
	ui.comboserialbaud_2->addItems(baudlist);
	ui.comboserialbaud_2->setCurrentText("9600");

	databitslist.clear();
	databitslist << "7 Data bits" << "8 Data bits";
	ui.comboserialdatabit_2->addItems(databitslist);
	ui.comboserialdatabit_2->setCurrentText("8 Data bits");

	paritylist.clear();
	paritylist << "None Parity" << "Odd Parity" << "Even Parity";
	ui.comboserialparity_2->addItems(paritylist);
	ui.comboserialparity_2->setCurrentText("None Parity");

	stopbitslist.clear();
	stopbitslist << "1 stop bit" << "2 stop bits";
	ui.comboserialstopbit_2->addItems(stopbitslist);
	ui.comboserialstopbit_2->setCurrentText("1 stop bit");

	on_btnloadini_2_clicked();

	//部署主程序已经连接了，这里只能断开
	ui.btnplcconnect_2->setEnabled(false);
	ui.btnplcdisconnect_2->setEnabled(true);
}


void settingFormPLC::plcmessageprocess(plc::PlcMessageType msg) {

}

void settingFormPLC::on_btnplcconnect_2_clicked() {
	int retval;

	//默认要更新配置文件
	QString path = QCoreApplication::applicationDirPath() + '/' + QString(ININAME_PLC);
	QSettings configini(path, QSettings::IniFormat);
	configini.setValue(QString("connect/type"), "Serial");
	configini.setValue(QString("serial/device"), ui.combocomlist_2->currentText());
	configini.setValue(QString("serial/baud"), ui.comboserialbaud_2->currentText());
	configini.setValue(QString("serial/data_bit"), QString(ui.comboserialdatabit_2->currentText()[0]));
	configini.setValue(QString("serial/stop_bit"), QString(ui.comboserialstopbit_2->currentText()[0]));
	configini.setValue(QString("serial/parity"), QString(ui.comboserialparity_2->currentText()[0]));
	configini.setValue(QString("function/offset"), ui.editplccameraoffset->text());

    retval = plcmaster::initialize([this](plc::PlcMessageType msg) {plcmessageprocess(msg); });

	if (retval == 0) {
		ui.btnplcconnect_2->setEnabled(false);
		ui.btnplcdisconnect_2->setEnabled(true);
	}
}

void settingFormPLC::on_btnplcdisconnect_2_clicked() {
	//断开PLC设备
	int retval = plcmaster::deinitialize();
	if (retval == 0) {
		ui.btnplcconnect_2->setEnabled(true);
		ui.btnplcdisconnect_2->setEnabled(false);
	}
}

void settingFormPLC::on_btnplcreadspeed_clicked() {
	float speed = plcmaster::getspeed();
	ui.editplcspeed->setText(QString::number(speed, 'f', 2));
}

void settingFormPLC::on_btnplcreadlength_clicked() {
	lengthinfo_s length;
	length = plcmaster::getcameralength();
	if (START == length.workstatus)
	{
		ui.editplcplclength->setText(QString::fromUtf8("质检中"));
	}
	else
	{
		ui.editplcplclength->setText(QString::fromUtf8("等待新卷"));
	}

	ui.editplccameralength->setText(QString::number(length.length, 'f', 1));
}

void settingFormPLC::on_btnplcreadtotallength_clicked() {
	float length;
	length = plcmaster::gettotallength();
	ui.editplctotallength->setText(QString::number(length, 'f', 1));
}

void settingFormPLC::on_btnplcreadtemp_clicked() {
	processinfo_s info;

	info = plcmaster::gettemperature();

	//清除之前的寄存器列表
	ui.tableplctemp->clearContents();

	//新建寄存器列表
	ui.tableplctemp->setRowCount(info.area_temperature.size() + info.composite_temperature.size()
		+ info.composite_pressure.size());

	for (int i = 0; i < info.area_temperature.size(); i++) {
		//地址
		QTableWidgetItem *qtablewidgetitem0 = new QTableWidgetItem();
		ui.tableplctemp->setItem(i, 0, qtablewidgetitem0);
		QString area = QString::number(i + 1, 10) + QString::fromUtf8("区");
		qtablewidgetitem0->setText(area);
		//temperature
		QTableWidgetItem *qtablewidgetitem1 = new QTableWidgetItem();
		ui.tableplctemp->setItem(i, 1, qtablewidgetitem1);
		QString temp = QString::number(info.area_temperature[i], 'f', 1);
		qtablewidgetitem1->setText(temp);
	}

	for (int i = 0; i < info.composite_temperature.size(); i++) {
		//地址
		QTableWidgetItem *qtablewidgetitem0 = new QTableWidgetItem();
		ui.tableplctemp->setItem(i + info.area_temperature.size(), 0, qtablewidgetitem0);
		QString area = QString::fromUtf8("复合") + QString::number(i + 1, 10) + QString::fromUtf8("温度");;
		qtablewidgetitem0->setText(area);
		//temperature
		QTableWidgetItem *qtablewidgetitem1 = new QTableWidgetItem();
		ui.tableplctemp->setItem(i + info.area_temperature.size(), 1, qtablewidgetitem1);
		QString temp = QString::number(info.composite_temperature[i], 'f', 1);
		qtablewidgetitem1->setText(temp);
	}

	for (int i = 0; i < info.composite_pressure.size(); i++) {
		//地址
		QTableWidgetItem *qtablewidgetitem0 = new QTableWidgetItem();
		ui.tableplctemp->setItem(i + info.area_temperature.size() + info.composite_temperature.size(), 0, qtablewidgetitem0);
		QString area = QString::fromUtf8("复合") + QString::number(i + 1, 10) + QString::fromUtf8("压力");
		qtablewidgetitem0->setText(area);
		//pressure
		QTableWidgetItem *qtablewidgetitem1 = new QTableWidgetItem();
		ui.tableplctemp->setItem(i + info.area_temperature.size() + info.composite_temperature.size(), 1, qtablewidgetitem1);
		QString temp = QString::number(info.composite_pressure[i], 'f', 1);
		qtablewidgetitem1->setText(temp);
	}
	update();
}

void settingFormPLC::on_btnclearini_2_clicked() {
	QString path = QCoreApplication::applicationDirPath() + '/' + QString(ININAME_PLC);
	QSettings configini(path, QSettings::IniFormat);
	configini.clear();
}

void settingFormPLC::on_btnloadini_2_clicked() {
	QString path = QCoreApplication::applicationDirPath() + '/' + QString(ININAME_PLC);
	QSettings configini(path, QSettings::IniFormat);
	if (configini.value("connect/type").toString() == "Serial") {
		ui.comboconnecttype_2->setCurrentIndex(0);
		ui.combocomlist_2->setCurrentText(configini.value("serial/device").toString());
		ui.comboserialbaud_2->setCurrentText(configini.value("serial/baud").toString());
		if ("7" == configini.value("serial/data_bit").toString()) {
			ui.comboserialdatabit_2->setCurrentIndex(0);
		}
		else {
			ui.comboserialdatabit_2->setCurrentIndex(1);
		}
		if ("1" == configini.value("serial/stop_bit").toString()) {
			ui.comboserialstopbit_2->setCurrentIndex(0);
		}
		else {
			ui.comboserialstopbit_2->setCurrentIndex(1);
		}
		if ("N" == configini.value("serial/parity").toString()) {
			ui.comboserialstopbit_2->setCurrentIndex(0);
		}
		else if ("O" == configini.value("serial/parity").toString()) {
			ui.comboserialstopbit_2->setCurrentIndex(1);
		}
		else {
			ui.comboserialstopbit_2->setCurrentIndex(2);
		}
		float plc2cameraoffset = configini.value("function/offset", QVariant(20.0)).toFloat();
		ui.editplccameraoffset->setText(QString::number(plc2cameraoffset, 'f', 1));
	}
	else {
		log_error("Zhongji Only support Serial, Please check plc config file!!!\n");
		return;
	}
}

void settingFormPLC::on_btnsaveini_clicked() {
	QString path = QCoreApplication::applicationDirPath() + '/' + QString(ININAME_PLC);
	QSettings configini(path, QSettings::IniFormat);
	configini.setValue(QString("connect/type"), "Serial");
	configini.setValue(QString("serial/device"), ui.combocomlist_2->currentText());
	configini.setValue(QString("serial/baud"), ui.comboserialbaud_2->currentText());
	configini.setValue(QString("serial/data_bit"), QString(ui.comboserialdatabit_2->currentText()[0]));
	configini.setValue(QString("serial/stop_bit"), QString(ui.comboserialstopbit_2->currentText()[0]));
	configini.setValue(QString("serial/parity"), QString(ui.comboserialparity_2->currentText()[0]));
	configini.setValue(QString("function/offset"), ui.editplccameraoffset->text());
}