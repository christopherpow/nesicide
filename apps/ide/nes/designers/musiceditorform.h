#ifndef MUSICEDITORFORM_H
#define MUSICEDITORFORM_H

#include "cdesignereditorbase.h"
#include "cdesignercommon.h"

#include "famitracker/MainFrame.h"
#include "famitracker/Action.h"

#include <QWidget>
#include <QToolBar>

namespace Ui {
class MusicEditorForm;
}

class MusicEditorForm : public CDesignerEditorBase
{
   Q_OBJECT
public:
   MusicEditorForm(QString fileName,QByteArray musicData,IProjectTreeViewItem* link = 0,QWidget* parent = 0);
   ~MusicEditorForm();
   
   // ICenterWidgetItem Interface Implmentation
   virtual void onSave();

private:
   Ui::MusicEditorForm *ui;
   CMainFrame* m_pMainFrame;
   CFamiTrackerDoc* m_pDocument;
   QString m_fileName;

private slots:
   void updateTargetMachine(QString /*target*/) {}
   void editor_modificationChanged(bool m);
};

#endif // MUSICEDITORFORM_H
