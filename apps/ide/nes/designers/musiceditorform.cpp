#include "musiceditorform.h"
#include "ui_musiceditorform.h"

#include "cqtmfc.h"

MusicEditorForm::MusicEditorForm(QString fileName,QByteArray musicData,IProjectTreeViewItem* link,QWidget* parent) :
   CDesignerEditorBase(link,parent),
   ui(new Ui::MusicEditorForm)
{
   ui->setupUi(this);
   
   m_fileName = fileName;
   
   openFile(m_fileName);
   
   ui->stackedWidget->addWidget(AfxGetApp()->m_pMainWnd->toQWidget());
   ui->stackedWidget->setCurrentWidget(AfxGetApp()->m_pMainWnd->toQWidget());
   
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
   
   CFamiTrackerDoc* pDoc = (CFamiTrackerDoc*)AfxGetApp()->m_pMainWnd->GetActiveDocument();

   pDoc->OnSaveDocument((TCHAR*)m_fileName.toAscii().constData()); 
   
   setModified(false);
}
