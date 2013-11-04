#include "musiceditorform.h"
#include "ui_musiceditorform.h"

#include "cqtmfc.h"

#include "Source/FamiTracker.h"
#include "Source/cqtmfc_famitracker.h"

MusicEditorForm::MusicEditorForm(QString fileName,QByteArray musicData,IProjectTreeViewItem* link,QWidget* parent) :
   CDesignerEditorBase(link,parent),
   ui(new Ui::MusicEditorForm)
{
   ui->setupUi(this);
   
   m_fileName = fileName;
   
   // Initialize FamiTracker...
   qtMfcInit(this);
   AfxGetApp()->InitInstance();   

   setCentralWidget(AfxGetMainWnd()->toQWidget());
   
   openFile(m_fileName);
   
   QObject::connect(AfxGetApp()->m_pMainWnd,SIGNAL(addToolBarWidget(QToolBar*)),this,SIGNAL(addToolBarWidget(QToolBar*)));
   QObject::connect(AfxGetApp()->m_pMainWnd,SIGNAL(removeToolBarWidget(QToolBar*)),this,SIGNAL(removeToolBarWidget(QToolBar*)));
   QObject::connect(AfxGetApp()->m_pMainWnd,SIGNAL(editor_modificationChanged(bool)),this,SLOT(editor_modificationChanged(bool)));
}

MusicEditorForm::~MusicEditorForm()
{
   delete ui;
}

void MusicEditorForm::editor_modificationChanged(bool m)
{
   setModified(m);
   
   emit editor_modified(m);
}

void MusicEditorForm::onSave()
{
   CDesignerEditorBase::onSave();
   
   CFamiTrackerDoc* pDoc = (CFamiTrackerDoc*)AfxGetMainWnd()->GetActiveDocument();

   pDoc->OnSaveDocument((TCHAR*)m_fileName.toAscii().constData()); 
   
   setModified(false);
}

void MusicEditorForm::onClose()
{   
   AfxGetMainWnd()->OnClose();
   
   // TODO: Handle unsaved documents or other pre-close stuffs
   AfxGetApp()->ExitInstance();
}

