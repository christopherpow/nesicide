#ifndef INSTRUMENTEDITDIALOG_H
#define INSTRUMENTEDITDIALOG_H

#include "FamiTrackerDoc.h"

class CInstrumentEditDlg_test : public CDialog
{
   Q_OBJECT
   
public:
   explicit CInstrumentEditDlg_test(CFamiTrackerDoc* pDoc,CWnd *parent = 0);
   ~CInstrumentEditDlg_test();
   
   CFamiTrackerDoc* GetDocument() { return m_pDoc; }
   
private:
   CFamiTrackerDoc* m_pDoc;
};

#endif // INSTRUMENTEDITDIALOG_H
