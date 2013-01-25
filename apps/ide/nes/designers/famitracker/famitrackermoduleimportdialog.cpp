#include "famitrackermoduleimportdialog.h"
#include "ui_famitrackermoduleimportdialog.h"

static LPCTSTR TRACK_FORMAT = _T("#%02i %s");

FamiTrackerModuleImportDialog::FamiTrackerModuleImportDialog(CFamiTrackerDoc* pDoc,QWidget *parent) :
   QDialog(parent),
   ui(new Ui::FamiTrackerModuleImportDialog)
{
   ui->setupUi(this);
   
   m_pDocument = pDoc;
   
   tracksModel = new QStringListModel();
   
   ui->tracks->setModel(tracksModel);
   
   ui->includeInstruments->setChecked(true);
}

FamiTrackerModuleImportDialog::~FamiTrackerModuleImportDialog()
{
   delete ui;
   delete tracksModel;
   SAFE_RELEASE(m_pImportedDoc);
}

void FamiTrackerModuleImportDialog::on_buttonBox_accepted()
{
   if (!(importInstruments() && importTracks()))
//		AfxMessageBox(IDS_IMPORT_FAILED, MB_ICONERROR);
  // CPTODO: intentional until I reimplement AfxMessageBox...
   return;
}

bool FamiTrackerModuleImportDialog::loadFile(QString path)
{
   CString TrackTitle;
   int idx;
   
   m_pImportedDoc = m_pDocument->LoadImportFile((LPCTSTR)path.toAscii().constData());
   
   // Check if load failed
	if (m_pImportedDoc == NULL)
		return false;

	// Check expansion chip match
	if (m_pImportedDoc->GetExpansionChip() != m_pDocument->GetExpansionChip()) {
//		AfxMessageBox(IDS_IMPORT_CHIP_MISMATCH);
		SAFE_RELEASE(m_pImportedDoc);
		return false;
	}

   QStringList songs;
   for ( idx = 0; idx < m_pImportedDoc->GetTrackCount(); idx++ )
   {      
      TrackTitle.Format((char*)TRACK_FORMAT, idx, m_pImportedDoc->GetTrackTitle(idx));
      songs.append((const char*)TrackTitle);
   }
   tracksModel->setStringList(songs);
      
	return true;
}

bool FamiTrackerModuleImportDialog::importInstruments()
{
   if (ui->includeInstruments->isChecked()) 
   {
		// Import instruments
		if (!m_pDocument->ImportInstruments(m_pImportedDoc, m_iInstrumentTable))
			return false;
	}
	else {
		// No instrument translation
		for (int i = 0; i < MAX_INSTRUMENTS; ++i)
			m_iInstrumentTable[i] = i;
	}

	return true;
}

bool FamiTrackerModuleImportDialog::importTracks()
{
   foreach (QModelIndex index, ui->tracks->selectionModel()->selectedIndexes()) 
   {
      // Import track
      if (!m_pDocument->ImportTrack(index.row(), m_pImportedDoc, m_iInstrumentTable))
         return false;
	}

	// Rebuild instrument list
	m_pDocument->SetModifiedFlag();
	m_pDocument->UpdateAllViews(NULL, UPDATE_INSTRUMENTS);
	m_pDocument->UpdateAllViews(NULL, CHANGED_PATTERN);

	return true;
}
