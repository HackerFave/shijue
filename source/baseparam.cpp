#include <iostream>
#include <vector>
#include <QString>
#include <QSettings>
#include <QCoreApplication>
#include "uicommon.h"
#include "baseparam.h"

namespace baseparam {

	class BaseparamService {
	public:
		static BaseparamService& instance() {
			static BaseparamService the_instance;
			return the_instance;
		}

	private:
		BaseparamService() = default;

	public:
		void loadini() {
			QString path = QCoreApplication::applicationDirPath() + '/' + QString(ININAME_SYSTEM);
			QSettings configiniSystem(path, QSettings::IniFormat);
			configiniSystem.setIniCodec("UTF-8");
			configiniSystem.beginGroup("Baseinfo");
			QString str;
			int num;

			str = configiniSystem.value("bBadProductRateAlarm").toString();
			if (str == "true")
			{
				set_badproductratealarm(true);
			}
			else
			{
				set_badproductratealarm(false);
			}
			num = configiniSystem.value("BadProductRateThr").toInt();
			set_badproductratethr(num);

			str = configiniSystem.value("bProductBlockedAlarm").toString();
			if (str == "true")
			{
				set_productblockedalarm(true);
			}
			else
			{
				set_productblockedalarm(false);
			}
			num = configiniSystem.value("ProductBlockedTime").toInt();
			set_productblockedalarmtime(num);

			str = configiniSystem.value("bAutoSave").toString();
			if (str == "true")
			{
				set_autosave(true);
			}
			else
			{
				set_autosave(false);
			}
			num = configiniSystem.value("AutoSaveNum").toInt();
			set_autosavenum(num);
			//str = configiniSystem.value("AutoSaveType").toString();
			str = QString::fromUtf8(configiniSystem.value("AutoSaveType").toByteArray());
			if (QString("²»Á¼Æ·") == str) {
				set_autosavetype(PRODUCTTYPE::BULIANGPIN);
			}
			else if (QString("Á¼Æ·") == str) {
				set_autosavetype(PRODUCTTYPE::LIANGPIN);
			}

			str = configiniSystem.value("bBadProductRateAlarmStop").toString();
			if (str == "true")
			{
				set_badproductratealarmstop(true);
			}
			else
			{
				set_badproductratealarmstop(false);
			}

			str = configiniSystem.value("bProductBlockedAlarmStop").toString();
			if (str == "true")
			{
				set_productblockedalarmstop(true);
			}
			else
			{
				set_productblockedalarmstop(false);
			}

            str = configiniSystem.value("bUserLastSolution").toString();
            if (str == "true")
            {
                set_uselastsolution(true);
            }
            else
            {
                set_uselastsolution(false);
            }

            str = configiniSystem.value("bLoginAsOperator=true").toString();
            if (str == "true")
            {
                set_loginasoperator(true);
            }
            else
            {
                set_loginasoperator(false);
            }

            str = configiniSystem.value("CurrenSolution").toString();
            set_uselastsolution(str);
			configiniSystem.endGroup();
		}

		//set
		void set_loginasoperator(bool val) {
			bLoginAsOperator = val;
		}
        void set_uselastsolution(QString val) {
            sUseLastSolution = val;
        }
        void set_uselastsolution(bool val) {
			bUseLastSolution = val;
		}
		void set_badproductratealarm(bool val) {
			bBadProductRateAlarm = val;
		}
		void set_badproductratethr(int val) {
			BadProductRateThr = val;
		}
		void set_productblockedalarm(bool val) {
			bProductBlockedAlarm = val;
		}
		void set_productblockedalarmtime(int val) {
			ProductBlockedTime = val;
		}
		void set_autosave(bool val) {
			bAutoSave = val;
		}
		void set_autosavetype(PRODUCTTYPE val) {
			AutoSaveType = val;
		}
		void set_autosavenum(int val) {
			AutoSaveNum = val;
		}
		void set_badproductratealarmstop(bool val) {
			bBadProductRateAlarmStop = val;
		}
		void set_productblockedalarmstop(bool val) {
			bProductBlockedAlarmStop = val;
		}

		//get
		bool get_loginasoperator() {
			return bLoginAsOperator;
		}
        QString get_uselastsolutionstr() {
            return sUseLastSolution;
        }
		bool get_uselastsolution() {
			return bUseLastSolution;
		}
		bool get_badproductratealarm() {
			return bBadProductRateAlarm;
		}
		int get_badproductratethr() {
			return BadProductRateThr;
		}
		bool get_productblockedalarm() {
			return bProductBlockedAlarm;
		}
		int get_productblockedalarmtime() {
			return ProductBlockedTime;
		}
		bool get_autosave() {
			return bAutoSave;
		}
		PRODUCTTYPE get_autosavetype() {
			return AutoSaveType;
		}
		int get_autosavenum() {
			return AutoSaveNum;
		}
		bool get_badproductratealarmstop() {
			return bBadProductRateAlarmStop;
		}
		bool get_productblockedalarmstop() {
			return bProductBlockedAlarmStop;
		}
	private:
		bool bLoginAsOperator;
        QString sUseLastSolution;
		bool bUseLastSolution;
		bool bBadProductRateAlarm;
		int BadProductRateThr;
		bool bProductBlockedAlarm;
		int ProductBlockedTime;
		bool bAutoSave;
		PRODUCTTYPE AutoSaveType;
		int AutoSaveNum;
		bool bBadProductRateAlarmStop;
		bool bProductBlockedAlarmStop;
	};
	//initialize
	void loadini() {
		BaseparamService::instance().loadini();
	}

	//set
	void set_loginasoperator(bool val) {
		BaseparamService::instance().set_loginasoperator(val);
	}
	void set_uselastsolution(bool val) {
		BaseparamService::instance().set_uselastsolution(val);
	}
	void set_badproductratealarm(bool val) {
		BaseparamService::instance().set_badproductratealarm(val);
	}
	void set_badproductratethr(int val) {
		BaseparamService::instance().set_badproductratethr(val);
	}
	void set_productblockedalarm(bool val) {
		BaseparamService::instance().set_productblockedalarm(val);
	}
	void set_productblockedalarmtime(bool val) {
		BaseparamService::instance().set_productblockedalarmtime(val);
	}
	void set_autosave(bool val) {
		BaseparamService::instance().set_autosave(val);
	}
	void set_autosavetype(PRODUCTTYPE val) {
		BaseparamService::instance().set_autosavetype(val);
	}
	void set_autosavenum(int val) {
		BaseparamService::instance().set_autosavenum(val);
	}
	void set_badproductratealarmstop(bool val) {
		BaseparamService::instance().set_badproductratealarmstop(val);
	}
	void set_productblockedalarmstop(bool val) {
		BaseparamService::instance().set_productblockedalarmstop(val);
	}

	//get
	bool get_loginasoperator() {
		return BaseparamService::instance().get_loginasoperator();
	}
    QString get_uselastsolutionstr() {
        return BaseparamService::instance().get_uselastsolutionstr();
    }
	bool get_uselastsolution() {
		return BaseparamService::instance().get_uselastsolution();
	}
	bool get_badproductratealarm() {
		return BaseparamService::instance().get_badproductratealarm();
	}
	int get_badproductratethr() {
		return BaseparamService::instance().get_badproductratethr();
	}
	bool get_productblockedalarm() {
		return BaseparamService::instance().get_productblockedalarm();
	}
	int get_productblockedalarmtime() {
		return BaseparamService::instance().get_productblockedalarmtime();
	}
	bool get_autosave() {
		return BaseparamService::instance().get_autosave();
	}
	PRODUCTTYPE get_autosavetype() {
		return BaseparamService::instance().get_autosavetype();
	}
	int get_autosavenum() {
		return BaseparamService::instance().get_autosavenum();
	}
	bool get_badproductratealarmstop() {
		return BaseparamService::instance().get_badproductratealarmstop();
	}
	bool get_productblockedalarmstop() {
		return BaseparamService::instance().get_productblockedalarmstop();
	}
};
