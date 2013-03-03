#include "instrumenteditdialog.h"
#include "ui_instrumenteditdialog.h"

#include "SequenceEditor.h"
#include "MainFrm.h"

CInstrumentEditDialog::CInstrumentEditDialog(CFamiTrackerDoc* pDoc,QDialog *parent) :
   QDialog(parent),
   ui(new Ui::CInstrumentEditDialog)
{
   ui->setupUi(this);
   
   CRect rect(SX(190 - 2 - 40) + 40, SY(30) - 2, SX(CSequenceEditor::SEQUENCE_EDIT_WIDTH - 190) + 190, SY(CSequenceEditor::SEQUENCE_EDIT_HEIGHT-42)+42);

   // CP: TEST
   CSequenceEditor* m_pSequenceEditor = new CSequenceEditor(pDoc);
	m_pSequenceEditor->CreateEditor((CWnd*)this, rect);

   CSequence* m_pSequence = pDoc->GetSequence(0, SEQ_VOLUME);
	m_pSequenceEditor->SelectSequence(m_pSequence, SEQ_VOLUME, INST_2A03);
   
   m_pSequenceEditor->ShowWindow(SW_SHOW);
}

CInstrumentEditDialog::~CInstrumentEditDialog()
{
   delete ui;
}
