#ifndef REGISTERINSPECTORDOCKWIDGET_H
#define REGISTERINSPECTORDOCKWIDGET_H

#include "cdebuggermemorydisplaymodel.h"
#include "cdebuggerregisterdisplaymodel.h"
#include "cdebuggerregistercomboboxdelegate.h"
#include "cregisterdata.h"

#include <QDockWidget>

namespace Ui {
   class RegisterInspectorDockWidget;
}

class RegisterInspectorDockWidget : public QDockWidget
{
   Q_OBJECT

public:
   explicit RegisterInspectorDockWidget(eMemoryType display = eMemory_IOregs, QWidget *parent = 0);
   ~RegisterInspectorDockWidget();

protected:
   void changeEvent(QEvent* e);
   void contextMenuEvent(QContextMenuEvent* e);
   void showEvent(QShowEvent* e);
   void hideEvent(QHideEvent* e);

public slots:
   void updateMemory();

private:
   Ui::RegisterInspectorDockWidget *ui;
   CDebuggerMemoryDisplayModel* binaryModel;
   CDebuggerRegisterDisplayModel* bitfieldModel;
   CDebuggerRegisterComboBoxDelegate* bitfieldDelegate;
   CRegisterData** m_tblRegisters;
   eMemoryType m_display;
   int m_register;

private slots:
   void on_binaryView_entered(QModelIndex index);
   void on_binaryView_activated(QModelIndex index);
   void on_binaryView_pressed(QModelIndex index);
   void on_bitfieldView_doubleClicked(QModelIndex index);
   void on_bitfieldView_clicked(QModelIndex index);
   void on_binaryView_doubleClicked(QModelIndex index);
   void on_binaryView_clicked(QModelIndex index);
   void binaryView_currentChanged(QModelIndex index, QModelIndex);
};

#endif // REGISTERINSPECTORDOCKWIDGET_H
