#ifndef CQTMFC_FAMITRACKER_H
#define CQTMFC_FAMITRACKER_H

#include "cqtmfc.h"

extern QHash<int,CString> qtMfcStringResources;

extern QHash<int,CBitmap*> qtMfcBitmapResources;

extern QHash<int,QIcon*> qtIconResources;

extern QHash<int,CMenu*> qtMfcMenuResources;

void qtMfcInit();

void qtMfcInitDialogResource(UINT dlgID,CDialog* parent);

void qtMfcInitToolBarResource(UINT dlgID,CToolBar* parent);

#endif // CQTMFC_FAMITRACKER_H
