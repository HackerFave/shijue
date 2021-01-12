#include "sqlsettingdialog.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SqlSettingDialog w;
    w.show();
    return a.exec();
}
