#include <qlibrary.h>
#include <qdebug.h>
#include "messageboxtest.h"
#include "mrun_messagebox.h"

MessageBoxTest::MessageBoxTest(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
}

void MessageBoxTest::on_toolButton_clicked()
{
    setmessage(MRUN_ERROR, "xxx");
}