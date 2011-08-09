#ifndef SYMBOLWATCHDOCKWIDGET_H
#define SYMBOLWATCHDOCKWIDGET_H

#include "cdebuggerbase.h"

#include "csymbolwatchmodel.h"
#include "cdebuggernumericitemdelegate.h"
#include "cdebuggersymboldelegate.h"
#include "ixmlserializable.h"

namespace Ui {
   class SymbolWatchDockWidget;
}

class SymbolWatchDockWidget : public CDebuggerBase, public IXMLSerializable
{
   Q_OBJECT

public:
   explicit SymbolWatchDockWidget(QWidget *parent = 0);
   virtual ~SymbolWatchDockWidget();

   // IXMLSerializable interface
   virtual bool serialize(QDomDocument& doc, QDomNode& node);
   virtual bool deserialize(QDomDocument& doc, QDomNode& node, QString& errors);

protected:
   void keyPressEvent(QKeyEvent *event);
   void dragEnterEvent(QDragEnterEvent *event);
   void dragMoveEvent(QDragMoveEvent *event);
   void dropEvent(QDropEvent *event);
   void contextMenuEvent(QContextMenuEvent *event);
   void showEvent(QShowEvent *event);
   void hideEvent(QHideEvent *event);

private:
   Ui::SymbolWatchDockWidget *ui;
   CSymbolWatchModel* watchModel;
   CDebuggerSymbolDelegate* watchSymbolDelegate;
   CDebuggerNumericItemDelegate* watchValueDelegate;
   CSymbolWatchModel* ramModel;
   CDebuggerNumericItemDelegate* ramValueDelegate;
   CSymbolWatchModel* sramModel;
   CDebuggerNumericItemDelegate* sramValueDelegate;
   CSymbolWatchModel* exramModel;
   CDebuggerNumericItemDelegate* exramValueDelegate;

signals:
   void breakpointsChanged();

private slots:
   void updateUi();
   void updateVariables();
   void on_actionRemove_symbol_triggered();
   void on_actionBreak_on_CPU_access_here_triggered();
   void on_actionBreak_on_CPU_read_here_triggered();
   void on_actionBreak_on_CPU_write_here_triggered();
};

#endif // SYMBOLWATCHDOCKWIDGET_H
