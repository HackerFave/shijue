#include "messageboxtest.h"
#include <QtWidgets/QApplication>
//#include "../testDll/testDll.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MessageBoxTest w;
    w.show();
    return a.exec();
}
