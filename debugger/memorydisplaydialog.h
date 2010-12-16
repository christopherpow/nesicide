#ifndef MEMORYDISPLAYDIALOG_H
#define MEMORYDISPLAYDIALOG_H

#include <QDialog>
#include "cdebuggermemorydisplaymodel.h"

#include "cregisterdata.h"

namespace Ui
{
class MemoryDisplayDialog;
}

class MemoryDisplayDialog : public QDialog
{
   Q_OBJECT
public:
   MemoryDisplayDialog(QWidget* parent = 0, eMemoryType display = eMemory_CPU);
   ~MemoryDisplayDialog();

protected:
   void showEvent(QShowEvent* e);
   void contextMenuEvent(QContextMenuEvent* e);
   void changeEvent(QEvent* e);

public slots:
   void cartridgeLoaded();
   void updateMemory();

signals:
   void showMe(eMemoryType display);

private:
   Ui::MemoryDisplayDialog* ui;
   CDebuggerMemoryDisplayModel* model;
   eMemoryType m_display;

private slots:
   void on_actionBreak_on_CPU_write_here_triggered();
   void on_actionBreak_on_CPU_read_here_triggered();
   void on_actionBreak_on_CPU_access_here_triggered();
};

#endif // MEMORYDISPLAYDIALOG_H
