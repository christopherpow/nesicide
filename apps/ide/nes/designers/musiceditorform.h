#ifndef MUSICEDITORFORM_H
#define MUSICEDITORFORM_H

#include "cdesignereditorbase.h"
#include "cdesignercommon.h"

#include <QWidget>

namespace Ui {
class MusicEditorForm;
}

class MusicEditorForm : public CDesignerEditorBase
{
   Q_OBJECT

public:
   explicit MusicEditorForm(IProjectTreeViewItem* link,QWidget *parent = 0);
   ~MusicEditorForm();

   // ICenterWidgetItem interface

private:
   Ui::MusicEditorForm *ui;

private slots:
   void updateTargetMachine(QString /*target*/) {}
};

#endif // MUSICEDITORFORM_H
