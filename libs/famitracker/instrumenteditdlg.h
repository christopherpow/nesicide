#ifndef INSTRUMENTEDITDIALOG_H
#define INSTRUMENTEDITDIALOG_H

#include "FamiTrackerDoc.h"

class CInstrumentEditDlg : public CDialog
{
   Q_OBJECT
   
public:
   explicit CInstrumentEditDlg(CFamiTrackerDoc* pDoc,CWnd *parent = 0);
   ~CInstrumentEditDlg();
   
   CFamiTrackerDoc* GetDocument() { return m_pDoc; }
   
private:
   CFamiTrackerDoc* m_pDoc;
};

#endif // INSTRUMENTEDITDIALOG_H
