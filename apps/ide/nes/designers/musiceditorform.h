#ifndef MUSICEDITORFORM_H
#define MUSICEDITORFORM_H

#include "cdesignereditorbase.h"
#include "cdesignercommon.h"

#include "MainFrm.h"

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
   QString m_fileName;

private slots:
   void updateTargetMachine(QString /*target*/) {}
   void editor_modificationChanged(bool m);
};

#endif // MUSICEDITORFORM_H
