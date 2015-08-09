#ifndef CQTMFC_FAMITRACKER_H
#define CQTMFC_FAMITRACKER_H

#include "cqtmfc.h"

extern QHash<int,CString> qtMfcStringResources;

extern QHash<int,CBitmap*> qtMfcBitmapResources;

extern QHash<int,QString> qtIconNames;
extern QHash<int,QIcon*> qtIconResources;

void qtMfcInit(QMainWindow* parent);

void qtMfcInitDialogResource(UINT dlgID,CDialog* parent);

void qtMfcInitToolBarResource(UINT dlgID,CToolBar* parent);

#endif // CQTMFC_FAMITRACKER_H
