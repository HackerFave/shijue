#include "mrun_sqlsettingdialog.h"
#include "sqlsettingdialog.h"

void* CreateGui()
{
    return (void*)new SqlSettingDialog(NULL);
}

 void ReleaseGui(void* gui)
{
    if (gui != NULL) {
        SqlSettingDialog* ui = (SqlSettingDialog*)gui;
        delete ui;
        gui = NULL;
    }
}
