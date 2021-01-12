#include <iostream>
#include <vector>
#include <QString>

enum PRODUCTTYPE {
	LIANGPIN = 0,
	BULIANGPIN = 1,
	ALL = 2,
};
namespace baseparam {
	void loadini();

	//set
	void set_loginasoperator(bool val);
	void set_uselastsolution(bool val);
	void set_badproductratealarm(bool val);
	void set_badproductratethr(int val);
	void set_productblockedalarm(bool val);
	void set_productblockedalarmtime(bool val);
	void set_autosave(bool val);
	void set_autosavetype(PRODUCTTYPE val);
	void set_autosavenum(int val);
	void set_badproductratealarmstop(bool val);
	void set_productblockedalarmstop(bool val);

	//get
	bool get_loginasoperator();
    QString get_uselastsolutionstr();
	bool get_uselastsolution();
	bool get_badproductratealarm();
	int get_badproductratethr();
	bool get_productblockedalarm();
	int get_productblockedalarmtime();
	bool get_autosave();
	PRODUCTTYPE get_autosavetype();
	int get_autosavenum();
	bool get_badproductratealarmstop();
	bool get_productblockedalarmstop();
}