#ifndef REGISTERINSPECTORDOCKWIDGET_H
#define REGISTERINSPECTORDOCKWIDGET_H

#include "cdebuggerbase.h"

#include "cdebuggerregisterdisplaymodel.h"
#include "cdebuggerbitfielddisplaymodel.h"
#include "cdebuggerbitfieldcomboboxdelegate.h"
#include "cdebuggernumericitemdelegate.h"
#include "cregisterdata.h"
#include "cbreakpointinfo.h"

namespace Ui {
   class RegisterInspectorDockWidget;
}

class RegisterInspectorDockWidget : public CDebuggerBase
{
   Q_OBJECT

public:
   RegisterInspectorDockWidget(regDBFunc regDB,CBreakpointInfo* pBreakpoints,QWidget *parent = 0);
   virtual ~RegisterInspectorDockWidget();

protected:
   void changeEvent(QEvent* e);
   void contextMenuEvent(QContextMenuEvent* e);
   void showEvent(QShowEvent* e);
   void hideEvent(QHideEvent* e);

public slots:
   void updateMemory();

private:
   Ui::RegisterInspectorDockWidget *ui;
   CDebuggerRegisterDisplayModel* binaryModel;
   CDebuggerBitfieldDisplayModel* bitfieldModel;
   CDebuggerBitfieldComboBoxDelegate* bitfieldDelegate;
   CDebuggerNumericItemDelegate* binaryDelegate;
   CBreakpointInfo* m_pBreakpoints;
   regDBFunc m_regDBFunc;
   CRegisterDatabase* m_regDB;
   int       m_register;

private slots:
   void on_binaryView_entered(QModelIndex index);
   void on_binaryView_activated(QModelIndex index);
   void on_binaryView_pressed(QModelIndex index);
   void on_binaryView_clicked(QModelIndex index);
   void on_binaryView_doubleClicked(QModelIndex index);
   void on_bitfieldView_doubleClicked(QModelIndex index);
   void on_bitfieldView_clicked(QModelIndex index);
   void binaryView_currentChanged(QModelIndex index, QModelIndex);
   void updateTargetMachine(QString target);
};

#endif // REGISTERINSPECTORDOCKWIDGET_H
