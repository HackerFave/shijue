#include "TemplateMainForm.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TemplateMainForm w;
    w.show();
    //QDir::setCurrent(app.applicationDirPath());//������ �����
    return a.exec();
}
