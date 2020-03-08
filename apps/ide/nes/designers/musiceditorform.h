#ifndef MUSICEDITORFORM_H
#define MUSICEDITORFORM_H

#include "cdesignereditorbase.h"
#include "cdesignercommon.h"

#include "Source/MainFrm.h"

#include <QWidget>
#include <QToolBar>

namespace Ui {
class MusicEditorForm;
}

class MusicEditorForm : public CDesignerEditorBase
{
   Q_OBJECT
public:
   // This takes place of the constructor and provides a singleton instance.
   static MusicEditorForm* instance();
   QString fileName() { return m_fileName; }
   void setFileName(QString fileName) { m_fileName = fileName; }
   void setTreeLink(IProjectTreeViewItem* link) { InitTabItem(link); }
   virtual ~MusicEditorForm();
   
   // ICenterWidgetItem Interface Implmentation
   virtual void onSave();
   virtual void onClose();
   virtual QMessageBox::StandardButton onCloseQuery();
   
private:
   MusicEditorForm(QString fileName,QByteArray musicData,IProjectTreeViewItem* link = 0,QWidget* parent = 0);
   Ui::MusicEditorForm *ui;
   QString m_fileName;
   static MusicEditorForm* _instance;

public slots:
   void updateTargetMachine(QString /*target*/) {}
   void editor_modificationChanged(bool m);
   void documentSaved();
   void documentClosed();
};

#endif // MUSICEDITORFORM_H
