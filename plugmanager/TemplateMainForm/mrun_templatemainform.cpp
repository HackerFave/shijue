#include "mrun_templatemainform.h"
#include "TemplateMainForm.h"

void* CreateGui()
{
	return (void*)new TemplateMainForm(NULL);
}

void ReleaseGui(void* gui)
{
	if (gui != NULL) {
		TemplateMainForm* ui = (TemplateMainForm*)gui;
		delete ui;
		gui = NULL;
	}
}

