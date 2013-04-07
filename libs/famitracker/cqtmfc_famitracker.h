#ifndef CQTMFC_FAMITRACKER_H
#define CQTMFC_FAMITRACKER_H

#include "cqtmfc.h"

extern QHash<int,CString> qtMfcStringResources;

extern QHash<int,CMenu> qtMfcMenuResources;

extern QHash<int,CBitmap> qtMfcBitmapResources;

void qtMfcInit();

void qtMfcInitDialogResource(UINT dlgID,CDialog* parent);

#endif // CQTMFC_FAMITRACKER_H
