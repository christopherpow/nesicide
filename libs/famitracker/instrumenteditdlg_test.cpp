#include "instrumenteditdlg_test.h"

//#include "InstrumentEditor2A03.h"
#include "InstrumentEditDlg.h"
#include "MainFrm.h"

CInstrumentEditDlg_test::CInstrumentEditDlg_test(CFamiTrackerDoc* pDoc,CWnd *parent) :
   CDialog(0,parent)
{
   m_pDoc = pDoc;

   // CP: TEST
//   CInstrumentEditor2A03* pInst = new CInstrumentEditor2A03(this);
//   pInst->SelectInstrument(0);
   
   CInstrumentEditDlg* pDlg = new CInstrumentEditDlg(this);
//   pDlg->setGeometry(this->geometry());
   pDlg->SetCurrentInstrument(0);
   pDlg->DoModal();
}

CInstrumentEditDlg_test::~CInstrumentEditDlg_test()
{
}
