#include "instrumenteditdlg.h"

#include "InstrumentEditor2A03.h"
#include "MainFrm.h"

CInstrumentEditDlg::CInstrumentEditDlg(CFamiTrackerDoc* pDoc,CWnd *parent) :
   CDialog(0,parent)
{
   m_pDoc = pDoc;

   // CP: TEST
   CInstrumentEditor2A03* pInst = new CInstrumentEditor2A03(this);
   pInst->SelectInstrument(0);
}

CInstrumentEditDlg::~CInstrumentEditDlg()
{
}
