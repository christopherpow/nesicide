#include "symbolwatchdockwidget.h"
#include "ui_symbolwatchdockwidget.h"

#include "cobjectregistry.h"
#include "main.h"

#include "ccc65interface.h"

enum
{
   Symbol_Watch_Window,
   Symbol_RAM_Window,
   Symbol_SRAM_Window,
   Symbol_EXRAM_Window
};

SymbolWatchDockWidget::SymbolWatchDockWidget(QWidget *parent) :
   CDebuggerBase(parent)
{
   setupUi(this);

   watchModel = new CSymbolWatchModel(true);
   watchSymbolDelegate = new CDebuggerSymbolDelegate();
   watchValueDelegate = new CDebuggerNumericItemDelegate();

   ramModel = new CSymbolWatchModel(false);
   ramValueDelegate = new CDebuggerNumericItemDelegate();

   watch->setModel(watchModel);
   watch->setItemDelegateForColumn(SymbolWatchCol_Symbol,watchSymbolDelegate);
   watch->setItemDelegateForColumn(SymbolWatchCol_Value,watchValueDelegate);
   watch->resizeColumnsToContents();

   ram->setModel(ramModel);
   ram->setItemDelegateForColumn(SymbolWatchCol_Value,ramValueDelegate);
   ram->resizeColumnsToContents();

#if defined(Q_OS_MAC) || defined(Q_OS_MACX) || defined(Q_OS_MAC64)
   watch->setFont(QFont("Monaco", 11));
   ram->setFont(QFont("Monaco", 11));
#endif
#ifdef Q_OS_LINUX
   watch->setFont(QFont("Monospace", 10));
   ram->setFont(QFont("Monospace", 10));
#endif
#ifdef Q_OS_WIN
   watch->setFont(QFont("Consolas", 11));
   ram->setFont(QFont("Consolas", 11));
#endif

   QObject::connect(watch->horizontalHeader(),SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)),watchModel,SLOT(sort(int,Qt::SortOrder)));
   QObject::connect(watchModel,SIGNAL(rowsInserted(QModelIndex,int,int)),this,SLOT(updateUi()));
   QObject::connect(ram->horizontalHeader(),SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)),ramModel,SLOT(sort(int,Qt::SortOrder)));

   m_targetLoaded = "none";
}

SymbolWatchDockWidget::~SymbolWatchDockWidget()
{
   delete watchModel;
   delete watchValueDelegate;
   delete watchSymbolDelegate;
   delete ramModel;
   delete ramValueDelegate;
}

void SymbolWatchDockWidget::createNesUi()
{
   if ( !m_targetLoaded.compare("nes") )
   {
      return;
   }

   QObject* breakpointWatcher = CObjectRegistry::getInstance()->getObject("Breakpoint Watcher");
   QObject* emulator = CObjectRegistry::getInstance()->getObject("Emulator");

   sramTab = new QWidget();
   sramTab->setObjectName(QString::fromUtf8("sramTab"));
   sramGridLayout = new QGridLayout(sramTab);
   sramGridLayout->setContentsMargins(0, 0, 0, 0);
   sramGridLayout->setObjectName(QString::fromUtf8("sramGridLayout"));
   sram = new QTableView(sramTab);
   sram->setObjectName(QString::fromUtf8("sram"));
   sram->setAcceptDrops(true);
   sram->setFrameShape(QFrame::NoFrame);
   sram->setEditTriggers(QAbstractItemView::AnyKeyPressed|QAbstractItemView::DoubleClicked|QAbstractItemView::EditKeyPressed|QAbstractItemView::SelectedClicked);
   sram->setDragEnabled(true);
   sram->setDragDropMode(QAbstractItemView::DragDrop);
   sram->setSelectionMode(QAbstractItemView::SingleSelection);
   sram->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
   sram->setShowGrid(false);
   sram->setWordWrap(false);
   sram->setCornerButtonEnabled(false);
   sram->horizontalHeader()->setDefaultSectionSize(150);
   sram->horizontalHeader()->setProperty("showSortIndicator", QVariant(true));
   sram->horizontalHeader()->setStretchLastSection(true);
   sram->verticalHeader()->setVisible(false);
   sram->verticalHeader()->setDefaultSectionSize(23);

   sramGridLayout->addWidget(sram, 0, 0, 1, 1);

   tabWidget->addTab(sramTab, "SRAM");
   exramTab = new QWidget();
   exramTab->setObjectName(QString::fromUtf8("exramTab"));
   exramGridLayout = new QGridLayout(exramTab);
   exramGridLayout->setContentsMargins(0, 0, 0, 0);
   exramGridLayout->setObjectName(QString::fromUtf8("exramGridLayout"));
   exram = new QTableView(exramTab);
   exram->setObjectName(QString::fromUtf8("exram"));
   exram->setAcceptDrops(true);
   exram->setFrameShape(QFrame::NoFrame);
   exram->setEditTriggers(QAbstractItemView::AnyKeyPressed|QAbstractItemView::DoubleClicked|QAbstractItemView::EditKeyPressed|QAbstractItemView::SelectedClicked);
   exram->setDragEnabled(true);
   exram->setDragDropMode(QAbstractItemView::DragDrop);
   exram->setSelectionMode(QAbstractItemView::SingleSelection);
   exram->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
   exram->setShowGrid(false);
   exram->setWordWrap(false);
   exram->setCornerButtonEnabled(false);
   exram->horizontalHeader()->setDefaultSectionSize(150);
   exram->horizontalHeader()->setProperty("showSortIndicator", QVariant(true));
   exram->horizontalHeader()->setStretchLastSection(true);
   exram->verticalHeader()->setVisible(false);
   exram->verticalHeader()->setDefaultSectionSize(23);

   exramGridLayout->addWidget(exram, 0, 0, 1, 1);

   tabWidget->addTab(exramTab, "EXRAM");

   sramModel = new CSymbolWatchModel(false);
   sramValueDelegate = new CDebuggerNumericItemDelegate();

   exramModel = new CSymbolWatchModel(false);
   exramValueDelegate = new CDebuggerNumericItemDelegate();

   sram->setModel(sramModel);
   sram->setItemDelegateForColumn(SymbolWatchCol_Value,sramValueDelegate);
   sram->resizeColumnsToContents();

   exram->setModel(exramModel);
   exram->setItemDelegateForColumn(SymbolWatchCol_Value,exramValueDelegate);
   exram->resizeColumnsToContents();

#if defined(Q_OS_MAC) || defined(Q_OS_MACX) || defined(Q_OS_MAC64)
   sram->setFont(QFont("Monaco", 11));
   exram->setFont(QFont("Monaco", 11));
#endif
#ifdef Q_OS_LINUX
   sram->setFont(QFont("Monospace", 10));
   exram->setFont(QFont("Monospace", 10));
#endif
#ifdef Q_OS_WIN
   sram->setFont(QFont("Consolas", 11));
   exram->setFont(QFont("Consolas", 11));
#endif

   QObject::connect(sram->horizontalHeader(),SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)),sramModel,SLOT(sort(int,Qt::SortOrder)));
   QObject::connect(exram->horizontalHeader(),SIGNAL(sortIndicatorChanged(int,Qt::SortOrder)),exramModel,SLOT(sort(int,Qt::SortOrder)));

   QObject::connect(sram,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(sram_doubleClicked(QModelIndex)));
   QObject::connect(exram,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(exram_doubleClicked(QModelIndex)));

   QObject::connect(breakpointWatcher,SIGNAL(breakpointHit()),sramModel,SLOT(update()));
   QObject::connect(breakpointWatcher,SIGNAL(breakpointHit()),exramModel,SLOT(update()));
   QObject::connect(emulator,SIGNAL(machineReady()),sramModel,SLOT(update()));
   QObject::connect(emulator,SIGNAL(emulatorReset()),sramModel,SLOT(update()));
   QObject::connect(emulator,SIGNAL(emulatorPaused(bool)),sramModel,SLOT(update()));
   QObject::connect(emulator,SIGNAL(machineReady()),exramModel,SLOT(update()));
   QObject::connect(emulator,SIGNAL(emulatorReset()),exramModel,SLOT(update()));
   QObject::connect(emulator,SIGNAL(emulatorPaused(bool)),exramModel,SLOT(update()));

   m_targetLoaded = "nes";
}

void SymbolWatchDockWidget::destroyNesUi()
{
   if ( m_targetLoaded.compare("nes") )
   {
      return;
   }

   delete sram;
   delete sramGridLayout;
   delete sramTab;
   delete exram;
   delete exramGridLayout;
   delete exramTab;

   delete sramModel;
   delete sramValueDelegate;
   delete exramModel;
   delete exramValueDelegate;

   m_targetLoaded = "none";
}

void SymbolWatchDockWidget::createC64Ui()
{
   // Nothing new.

   m_targetLoaded = "c64";
}

void SymbolWatchDockWidget::destroyC64Ui()
{
   // Nothing new.

   m_targetLoaded = "none";
}

void SymbolWatchDockWidget::updateTargetMachine(QString target)
{
   QObject* breakpointWatcher = CObjectRegistry::getInstance()->getObject("Breakpoint Watcher");
   QObject* emulator = CObjectRegistry::getInstance()->getObject("Emulator");
   QObject* compiler = CObjectRegistry::getInstance()->getObject("Compiler");

   if ( !target.compare("nes",Qt::CaseInsensitive) )
   {
      if ( !m_targetLoaded.compare("c64",Qt::CaseInsensitive) )
      {
         destroyC64Ui();
      }
      createNesUi();
   }
   else if ( !target.compare("c64",Qt::CaseInsensitive) )
   {
      if ( !m_targetLoaded.compare("nes",Qt::CaseInsensitive) )
      {
         destroyNesUi();
      }
      createC64Ui();
   }

   QObject::connect(compiler,SIGNAL(compileDone(bool)),this,SLOT(updateVariables()));
   QObject::connect(breakpointWatcher,SIGNAL(breakpointHit()),watchModel,SLOT(update()));
   QObject::connect(breakpointWatcher,SIGNAL(breakpointHit()),ramModel,SLOT(update()));
   if ( emulator )
   {
      QObject::connect(emulator,SIGNAL(machineReady()),watchModel,SLOT(update()));
      QObject::connect(emulator,SIGNAL(emulatorReset()),watchModel,SLOT(update()));
      QObject::connect(emulator,SIGNAL(emulatorPaused(bool)),watchModel,SLOT(update()));
      QObject::connect(emulator,SIGNAL(machineReady()),ramModel,SLOT(update()));
      QObject::connect(emulator,SIGNAL(emulatorReset()),ramModel,SLOT(update()));
      QObject::connect(emulator,SIGNAL(emulatorPaused(bool)),ramModel,SLOT(update()));
      QObject::connect(emulator,SIGNAL(machineReady()),this,SLOT(updateVariables()));
   }

}

void SymbolWatchDockWidget::updateUi()
{
   emit markProjectDirty(true);
}

void SymbolWatchDockWidget::updateVariables()
{
   QStringList symbols = CCC65Interface::getSymbolsForSourceFile("");
   int addr;

   if ( !m_targetLoaded.compare("nes",Qt::CaseInsensitive) )
   {
      ramModel->removeRows(0,ramModel->rowCount());
      sramModel->removeRows(0,sramModel->rowCount());
      exramModel->removeRows(0,exramModel->rowCount());
      foreach ( QString symbol,symbols )
      {
         addr = CCC65Interface::getSymbolAddress(symbol);
         if ( addr < MEM_2KB )
         {
            // Symbol is in RAM...
            ramModel->insertRow(symbol,addr);
         }
         else if ( (addr >= 0x5C00) && (addr < 0x6000) )
         {
            // Symbol is in EXRAM...
            exramModel->insertRow(symbol,addr);
         }
         else if ( (addr >= 0x6000) && (addr < 0x8000) )
         {
            // Symbol is in SRAM...
            sramModel->insertRow(symbol,addr);
         }
      }
      ramModel->update();
      sramModel->update();
      exramModel->update();
   }
   else if ( !m_targetLoaded.compare("c64",Qt::CaseInsensitive) )
   {
      ramModel->removeRows(0,ramModel->rowCount());
      foreach ( QString symbol,symbols )
      {
         addr = CCC65Interface::getSymbolAddress(symbol);

         // Symbol is in RAM...
         ramModel->insertRow(symbol,addr);
      }
      ramModel->update();
   }
}

void SymbolWatchDockWidget::addWatchedItem(QString item)
{
   show();

   tabWidget->setCurrentIndex(Symbol_Watch_Window);

   watchModel->insertRow(item);

   emit markProjectDirty(true);
}

void SymbolWatchDockWidget::keyPressEvent(QKeyEvent *event)
{
   if ( tabWidget->currentIndex() == Symbol_Watch_Window )
   {
      if ( (event->key() == Qt::Key_Delete) &&
           (watch->currentIndex().row() >= 0) )
      {
         watchModel->removeRow(watch->currentIndex().row());

         emit markProjectDirty(true);
      }
   }
}

void SymbolWatchDockWidget::dragEnterEvent(QDragEnterEvent *event)
{
   QString text;

   if ( event->mimeData()->hasText() )
   {
      text = event->mimeData()->text();

      event->acceptProposedAction();
   }
}

void SymbolWatchDockWidget::dragMoveEvent(QDragMoveEvent *event)
{
   QString text;

   if ( event->mimeData()->hasText() )
   {
      text = event->mimeData()->text();

      event->acceptProposedAction();
   }
}

void SymbolWatchDockWidget::dropEvent(QDropEvent *event)
{
   QString text;

   if ( event->mimeData()->hasText() )
   {
      text = event->mimeData()->text();

      tabWidget->setCurrentIndex(Symbol_Watch_Window);

      watchModel->insertRow(text);

      emit markProjectDirty(true);

      event->acceptProposedAction();
   }
}

void SymbolWatchDockWidget::contextMenuEvent(QContextMenuEvent *event)
{
   QModelIndex index;
   QMenu menu;

   switch ( tabWidget->currentIndex() )
   {
   case Symbol_Watch_Window:
      index = watch->currentIndex();
      break;
   case Symbol_RAM_Window:
      index = ram->currentIndex();
      break;
   case Symbol_SRAM_Window:
      index = sram->currentIndex();
      break;
   case Symbol_EXRAM_Window:
      index = exram->currentIndex();
      break;
   }

   if ( index.isValid() )
   {
      if ( tabWidget->currentIndex() == Symbol_Watch_Window )
      {
         menu.addAction(actionRemove_symbol);
         menu.addSeparator();
      }
      menu.addAction(actionGo_to_Definition);
      menu.addSeparator();
      menu.addAction(actionBreak_on_CPU_access_here);
      menu.addAction(actionBreak_on_CPU_read_here);
      menu.addAction(actionBreak_on_CPU_write_here);

      menu.exec(event->globalPos());
   }
}

void SymbolWatchDockWidget::showEvent(QShowEvent*)
{
   QObject* emulator = CObjectRegistry::getInstance()->getObject("Emulator");

   if ( emulator )
   {
      // Only update symbols on activated tab.
      switch ( tabWidget->currentIndex() )
      {
      case Symbol_Watch_Window:
         QObject::connect(emulator,SIGNAL(updateDebuggers()),watchModel,SLOT(update()));
         watchModel->update();
         break;
      case Symbol_RAM_Window:
         QObject::connect(emulator,SIGNAL(updateDebuggers()),ramModel,SLOT(update()));
         ramModel->update();
         break;
      case Symbol_SRAM_Window:
         if ( !m_targetLoaded.compare("nes",Qt::CaseInsensitive) )
         {
            QObject::connect(emulator,SIGNAL(updateDebuggers()),sramModel,SLOT(update()));
            sramModel->update();
         }
         break;
      case Symbol_EXRAM_Window:
         if ( !m_targetLoaded.compare("nes",Qt::CaseInsensitive) )
         {
            QObject::connect(emulator,SIGNAL(updateDebuggers()),exramModel,SLOT(update()));
            exramModel->update();
         }
         break;
      }
   }
}

void SymbolWatchDockWidget::hideEvent(QHideEvent */*event*/)
{
   QObject* emulator = CObjectRegistry::getInstance()->getObject("Emulator");

   if ( emulator )
   {
      QObject::disconnect(emulator,SIGNAL(updateDebuggers()),watchModel,SLOT(update()));
      QObject::disconnect(emulator,SIGNAL(updateDebuggers()),ramModel,SLOT(update()));
      if ( !m_targetLoaded.compare("nes",Qt::CaseInsensitive) )
      {
         QObject::disconnect(emulator,SIGNAL(updateDebuggers()),sramModel,SLOT(update()));
         QObject::disconnect(emulator,SIGNAL(updateDebuggers()),exramModel,SLOT(update()));
      }
   }
}

bool SymbolWatchDockWidget::serialize(QDomDocument& doc, QDomNode& node)
{
   QDomElement element = addElement( doc, node, "symbolinspector" );
   QList<WatchedItem> items = watchModel->getItems();

   for (int i=0; i < items.count(); i++)
   {
      QDomElement symbolElement = addElement( doc, element, "symbol" );
      symbolElement.setAttribute("name",items.at(i).symbol);
      symbolElement.setAttribute("segment",items.at(i).segment);
   }

   return true;
}

bool SymbolWatchDockWidget::deserialize(QDomDocument& /*doc*/, QDomNode& node, QString& /*errors*/)
{
   QDomNode childNode = node.firstChild();
   QDomNode symbolNode;
   QList<WatchedItem> items;
   WatchedItem item;

   if (!childNode.isNull())
   {
      do
      {
         if (childNode.nodeName() == "symbolinspector")
         {
            symbolNode = childNode.firstChild();
            while ( !(symbolNode.isNull()) )
            {
               QDomElement symbolElement = symbolNode.toElement();

               item.symbol = symbolElement.attribute("name");
               item.segment = symbolElement.attribute("segment","0").toInt();

               if ( !item.symbol.isEmpty() )
               {
                  items.append(item);
               }
               symbolNode = symbolNode.nextSibling();
            }

            watchModel->setItems(items);
            watchModel->update();
         }
      } while (!(childNode = childNode.nextSibling()).isNull());
   }

   return true;
}

void SymbolWatchDockWidget::on_actionBreak_on_CPU_write_here_triggered()
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   int row;
   QModelIndex index;
   int addr;
   int bpIdx;
   bool ok;

   switch ( tabWidget->currentIndex() )
   {
   case Symbol_Watch_Window:
      row = watch->currentIndex().row();
      index = watchModel->index(row,SymbolWatchCol_Address);
      addr = index.data(Qt::DisplayRole).toString().toInt(&ok,16);
      break;
   case Symbol_RAM_Window:
      row = ram->currentIndex().row();
      index = ramModel->index(row,SymbolWatchCol_Address);
      addr = index.data(Qt::DisplayRole).toString().toInt(&ok,16);
      break;
   case Symbol_SRAM_Window:
      row = sram->currentIndex().row();
      index = sramModel->index(row,SymbolWatchCol_Address);
      addr = index.data(Qt::DisplayRole).toString().toInt(&ok,16);
      break;
   case Symbol_EXRAM_Window:
      row = exram->currentIndex().row();
      index = exramModel->index(row,SymbolWatchCol_Address);
      addr = index.data(Qt::DisplayRole).toString().toInt(&ok,16);
      break;
   default:
      addr = 0;
      break;
   }

   if ( ok )
   {
      bpIdx = pBreakpoints->AddBreakpoint ( eBreakOnCPUMemoryWrite,
                                            eBreakpointItemAddress,
                                            0,
                                            addr,
                                            addr,
                                            addr,
                                            0xFFFF,
                                            false,
                                            eBreakpointConditionTest,
                                            0,
                                            eBreakpointDataPure,
                                            0,
                                            true );

      if ( bpIdx < 0 )
      {
         QString str;
         str.sprintf("Cannot add breakpoint, maximum of %d already used.", NUM_BREAKPOINTS);
         QMessageBox::information(0, "Error", str);
      }
      else
      {
         emit breakpointsChanged();
         emit markProjectDirty(true);
      }
   }
}

void SymbolWatchDockWidget::on_actionBreak_on_CPU_read_here_triggered()
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   int row;
   QModelIndex index;
   int addr;
   int bpIdx;
   bool ok;

   switch ( tabWidget->currentIndex() )
   {
   case Symbol_Watch_Window:
      row = watch->currentIndex().row();
      index = watchModel->index(row,SymbolWatchCol_Address);
      addr = index.data(Qt::DisplayRole).toString().toInt(&ok,16);
      break;
   case Symbol_RAM_Window:
      row = ram->currentIndex().row();
      index = ramModel->index(row,SymbolWatchCol_Address);
      addr = index.data(Qt::DisplayRole).toString().toInt(&ok,16);
      break;
   case Symbol_SRAM_Window:
      row = sram->currentIndex().row();
      index = sramModel->index(row,SymbolWatchCol_Address);
      addr = index.data(Qt::DisplayRole).toString().toInt(&ok,16);
      break;
   case Symbol_EXRAM_Window:
      row = exram->currentIndex().row();
      index = exramModel->index(row,SymbolWatchCol_Address);
      addr = index.data(Qt::DisplayRole).toString().toInt(&ok,16);
      break;
   default:
      addr = 0;
      break;
   }

   if ( ok )
   {
      bpIdx = pBreakpoints->AddBreakpoint ( eBreakOnCPUMemoryRead,
                                            eBreakpointItemAddress,
                                            0,
                                            addr,
                                            addr,
                                            addr,
                                            0xFFFF,
                                            false,
                                            eBreakpointConditionTest,
                                            0,
                                            eBreakpointDataPure,
                                            0,
                                            true );

      if ( bpIdx < 0 )
      {
         QString str;
         str.sprintf("Cannot add breakpoint, maximum of %d already used.", NUM_BREAKPOINTS);
         QMessageBox::information(0, "Error", str);
      }
      else
      {
         emit breakpointsChanged();
         emit markProjectDirty(true);
      }
   }
}

void SymbolWatchDockWidget::on_actionBreak_on_CPU_access_here_triggered()
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   int row;
   QModelIndex index;
   int addr;
   int bpIdx;
   bool ok;

   switch ( tabWidget->currentIndex() )
   {
   case Symbol_Watch_Window:
      row = watch->currentIndex().row();
      index = watchModel->index(row,SymbolWatchCol_Address);
      addr = index.data(Qt::DisplayRole).toString().toInt(&ok,16);
      break;
   case Symbol_RAM_Window:
      row = ram->currentIndex().row();
      index = ramModel->index(row,SymbolWatchCol_Address);
      addr = index.data(Qt::DisplayRole).toString().toInt(&ok,16);
      break;
   case Symbol_SRAM_Window:
      row = sram->currentIndex().row();
      index = sramModel->index(row,SymbolWatchCol_Address);
      addr = index.data(Qt::DisplayRole).toString().toInt(&ok,16);
      break;
   case Symbol_EXRAM_Window:
      row = exram->currentIndex().row();
      index = exramModel->index(row,SymbolWatchCol_Address);
      addr = index.data(Qt::DisplayRole).toString().toInt(&ok,16);
      break;
   default:
      addr = 0;
      break;
   }

   if ( ok )
   {
      bpIdx = pBreakpoints->AddBreakpoint ( eBreakOnCPUMemoryAccess,
                                            eBreakpointItemAddress,
                                            0,
                                            addr,
                                            addr,
                                            addr,
                                            0xFFFF,
                                            false,
                                            eBreakpointConditionTest,
                                            0,
                                            eBreakpointDataPure,
                                            0,
                                            true );

      if ( bpIdx < 0 )
      {
         QString str;
         str.sprintf("Cannot add breakpoint, maximum of %d already used.", NUM_BREAKPOINTS);
         QMessageBox::information(0, "Error", str);
      }
      else
      {
         emit breakpointsChanged();
         emit markProjectDirty(true);
      }
   }
}

void SymbolWatchDockWidget::on_actionRemove_symbol_triggered()
{
   if ( watch->currentIndex().isValid() )
   {
      watchModel->removeRow(watch->currentIndex().row());

      emit markProjectDirty(true);
   }
}

void SymbolWatchDockWidget::on_actionGo_to_Definition_triggered()
{
   int row;
   QModelIndex index;
   QString symbol;
   QString file;

   switch ( tabWidget->currentIndex() )
   {
   case Symbol_Watch_Window:
      row = watch->currentIndex().row();
      index = watchModel->index(row,SymbolWatchCol_Symbol);
      break;
   case Symbol_RAM_Window:
      row = ram->currentIndex().row();
      index = ramModel->index(row,SymbolWatchCol_Symbol);
      break;
   case Symbol_SRAM_Window:
      row = sram->currentIndex().row();
      index = sramModel->index(row,SymbolWatchCol_Symbol);
      break;
   case Symbol_EXRAM_Window:
      row = exram->currentIndex().row();
      index = exramModel->index(row,SymbolWatchCol_Symbol);
      break;
   }

   symbol = index.data(Qt::DisplayRole).toString();
   file = CCC65Interface::getSourceFileFromSymbol(symbol);

   emit snapTo("SourceNavigatorFile,"+file);
   emit snapTo("SourceNavigatorSymbol,"+symbol);
}

void SymbolWatchDockWidget::on_watch_doubleClicked(const QModelIndex &index)
{
   if ( index.isValid() )
   {
      emit snapTo("Address,"+watchModel->data(watchModel->index(index.row(),SymbolWatchCol_Address),Qt::DisplayRole).toString());
      on_actionGo_to_Definition_triggered();
   }
}

void SymbolWatchDockWidget::on_ram_doubleClicked(const QModelIndex &index)
{
   if ( index.isValid() )
   {
      emit snapTo("Address,"+ramModel->data(ramModel->index(index.row(),SymbolWatchCol_Address),Qt::DisplayRole).toString());
      on_actionGo_to_Definition_triggered();
   }
}

void SymbolWatchDockWidget::sram_doubleClicked(const QModelIndex &index)
{
   if ( index.isValid() )
   {
      emit snapTo("Address,"+sramModel->data(sramModel->index(index.row(),SymbolWatchCol_Address),Qt::DisplayRole).toString());
      on_actionGo_to_Definition_triggered();
   }
}

void SymbolWatchDockWidget::exram_doubleClicked(const QModelIndex &index)
{
   if ( index.isValid() )
   {
      emit snapTo("Address,"+exramModel->data(exramModel->index(index.row(),SymbolWatchCol_Address),Qt::DisplayRole).toString());
      on_actionGo_to_Definition_triggered();
   }
}

void SymbolWatchDockWidget::on_tabWidget_currentChanged(int index)
{
   QObject* emulator = CObjectRegistry::getInstance()->getObject("Emulator");

   if ( emulator )
   {
      // Only update symbols on activated tab.
      QObject::disconnect(emulator,SIGNAL(updateDebuggers()),watchModel,SLOT(update()));
      QObject::disconnect(emulator,SIGNAL(updateDebuggers()),ramModel,SLOT(update()));
      if ( !m_targetLoaded.compare("nes",Qt::CaseInsensitive) )
      {
         QObject::disconnect(emulator,SIGNAL(updateDebuggers()),sramModel,SLOT(update()));
         QObject::disconnect(emulator,SIGNAL(updateDebuggers()),exramModel,SLOT(update()));
      }
      switch ( index )
      {
      case Symbol_Watch_Window:
         QObject::connect(emulator,SIGNAL(updateDebuggers()),watchModel,SLOT(update()));
         watchModel->update();
         break;
      case Symbol_RAM_Window:
         QObject::connect(emulator,SIGNAL(updateDebuggers()),ramModel,SLOT(update()));
         ramModel->update();
         break;
      case Symbol_SRAM_Window:
         if ( !m_targetLoaded.compare("nes",Qt::CaseInsensitive) )
         {
            QObject::connect(emulator,SIGNAL(updateDebuggers()),sramModel,SLOT(update()));
            sramModel->update();
         }
         break;
      case Symbol_EXRAM_Window:
         if ( !m_targetLoaded.compare("nes",Qt::CaseInsensitive) )
         {
            QObject::connect(emulator,SIGNAL(updateDebuggers()),exramModel,SLOT(update()));
            exramModel->update();
         }
         break;
      }
   }
}
