#include "thermometer.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	thermometer w;
	w.show();
	return a.exec();
}
