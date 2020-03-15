#ifndef PRGROMDISPLAYDIALOG_H
#define PRGROMDISPLAYDIALOG_H

#include "cdesignereditorbase.h"
#include "iprojecttreeviewitem.h"

#include "stdint.h"

#include <Qsci/qsciscintilla.h>

namespace Ui
{
class PRGROMDisplayDialog;
}

class PRGROMDisplayDialog : public CDesignerEditorBase
{
   Q_OBJECT
public:
   PRGROMDisplayDialog(uint8_t* bankData,IProjectTreeViewItem* link = 0,QWidget* parent = 0);
   virtual ~PRGROMDisplayDialog();
   void setRomData(unsigned char* data)
   {
      m_data = data;
   }

protected:
   void changeEvent(QEvent* e);
   void showEvent(QShowEvent* e);

protected:
   unsigned char* m_data;

private:
   Ui::PRGROMDisplayDialog* ui;
   QsciScintilla* m_scintilla;
   QsciLexer* m_lexer;

private slots:
   void linesChanged();
   void applyProjectPropertiesToTab();
   void updateTargetMachine(QString /*target*/) {}
};

#endif // PRGROMDISPLAYDIALOG_H
