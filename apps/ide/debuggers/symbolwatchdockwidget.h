#ifndef SYMBOLWATCHDOCKWIDGET_H
#define SYMBOLWATCHDOCKWIDGET_H

#include "cdebuggerbase.h"

#include "csymbolwatchmodel.h"
#include "cdebuggernumericitemdelegate.h"
#include "cdebuggersymboldelegate.h"
#include "ixmlserializable.h"

#include "ui_symbolwatchdockwidget.h"

namespace Ui {
   class SymbolWatchDockWidget;
}

class SymbolWatchDockWidget : public CDebuggerBase, public IXMLSerializable, private Ui::SymbolWatchDockWidget
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
   void createNesUi();
   void destroyNesUi();
   void createC64Ui();
   void destroyC64Ui();

private:
   CSymbolWatchModel* watchModel;
   CDebuggerSymbolDelegate* watchSymbolDelegate;
   CDebuggerNumericItemDelegate* watchValueDelegate;
   CSymbolWatchModel* ramModel;
   CDebuggerNumericItemDelegate* ramValueDelegate;
   CSymbolWatchModel* sramModel;
   CDebuggerNumericItemDelegate* sramValueDelegate;
   CSymbolWatchModel* exramModel;
   CDebuggerNumericItemDelegate* exramValueDelegate;
   QWidget *sramTab;
   QGridLayout *sramGridLayout;
   QTableView *sram;
   QWidget *exramTab;
   QGridLayout *exramGridLayout;
   QTableView *exram;
   QString m_targetLoaded;

signals:
   void breakpointsChanged();

private slots:
   void on_actionGo_to_Definition_triggered();
   void updateUi();
   void updateVariables();
   void addWatchedItem(QString item);
   void on_actionRemove_symbol_triggered();
   void on_actionBreak_on_CPU_access_here_triggered();
   void on_actionBreak_on_CPU_read_here_triggered();
   void on_actionBreak_on_CPU_write_here_triggered();
   void updateTargetMachine(QString target);
   void on_watch_doubleClicked(const QModelIndex &index);
   void on_ram_doubleClicked(const QModelIndex &index);
   void sram_doubleClicked(const QModelIndex &index);
   void exram_doubleClicked(const QModelIndex &index);
   void on_tabWidget_currentChanged(int index);
};

#endif // SYMBOLWATCHDOCKWIDGET_H
