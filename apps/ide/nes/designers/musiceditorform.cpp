#include "musiceditorform.h"
#include "ui_musiceditorform.h"

#include "cqtmfc_famitracker.h"

#include "cqtmfc.h"

#include "Source/FamiTrackerDoc.h"

MusicEditorForm* MusicEditorForm::_instance = NULL;

MusicEditorForm* MusicEditorForm::instance()
{
   if ( !_instance )
   {
      _instance = new MusicEditorForm(QString(),QByteArray());
   }

   return _instance;
}

MusicEditorForm::MusicEditorForm(QString fileName,QByteArray musicData,IProjectTreeViewItem* link,QWidget* parent) :
   CDesignerEditorBase(link,parent),
   ui(new Ui::MusicEditorForm)
{
   ui->setupUi(this);
   
   // Initialize FamiTracker...
   ideifyFamiTracker();
   qtMfcInit(this);
   AfxGetApp()->InitInstance();
   
   CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
   setCentralWidget(pMainFrame->toQWidget());
   pMainFrame->toQWidget()->setAcceptDrops(true);

   QObject::connect(AfxGetApp()->m_pMainWnd,SIGNAL(addToolBarWidget(QToolBar*)),this,SIGNAL(addToolBarWidget(QToolBar*)));
   QObject::connect(AfxGetApp()->m_pMainWnd,SIGNAL(removeToolBarWidget(QToolBar*)),this,SIGNAL(removeToolBarWidget(QToolBar*)));
   QObject::connect(AfxGetApp()->m_pMainWnd,SIGNAL(editor_modificationChanged(bool)),this,SLOT(editor_modificationChanged(bool)));
}

MusicEditorForm::~MusicEditorForm()
{
   // Close FamiTracker.
   AfxGetMainWnd()->OnClose();   
   AfxGetApp()->ExitInstance();

   delete ui;
}

void MusicEditorForm::editor_modificationChanged(bool m)
{
   setModified(m);
   
   emit editor_modified(m);
}

void MusicEditorForm::documentSaved()
{
}

void MusicEditorForm::documentClosed()
{
//   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "FamiTracker");

//   settings.setValue("FamiTrackerWindowGeometry",saveGeometry());
//   settings.setValue("FamiTrackerWindowState",saveState());

//   // TODO: Handle unsaved documents or other pre-close stuffs
//   theApp.ExitInstance();

//   exit(0);
}

void MusicEditorForm::onSave()
{
   CDesignerEditorBase::onSave();

   AfxGetMainWnd()->SendMessage(ID_FILE_SAVE);
   
   setModified(false);
}

void MusicEditorForm::onClose()
{   
   openFile(NULL);
}

QMessageBox::StandardButton MusicEditorForm::onCloseQuery()
{
   QMessageBox::StandardButton doSave;
   
   doSave = CDesignerEditorBase::onCloseQuery();
   if ( doSave == QMessageBox::No )
   {
      // Need to nix the MFC modified flag.
      CFamiTrackerDoc* pDoc = (CFamiTrackerDoc*)AfxGetMainWnd()->GetActiveDocument();
   
      pDoc->SetModifiedFlag(FALSE);
   }
   
   return doSave;
}
