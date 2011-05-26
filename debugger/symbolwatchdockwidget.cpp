#include "symbolwatchdockwidget.h"
#include "ui_symbolwatchdockwidget.h"

#include "main.h"

enum
{
   SymbolWatchCol_Name,
   SymbolWatchCol_Address,
   SymbolWatchCol_Value
};

SymbolWatchDockWidget::SymbolWatchDockWidget(QWidget *parent) :
   QDockWidget(parent),
   ui(new Ui::SymbolWatchDockWidget)
{
   ui->setupUi(this);
   model = new CSymbolWatchModel();
   symbolDelegate = new CDebuggerSymbolDelegate();
   valueDelegate = new CDebuggerNumericItemDelegate();

   ui->tableView->setModel(model);
   ui->tableView->setItemDelegateForColumn(SymbolWatchCol_Name,symbolDelegate);
   ui->tableView->setItemDelegateForColumn(SymbolWatchCol_Value,valueDelegate);

   QObject::connect(emulator,SIGNAL(cartridgeLoaded()),model,SLOT(update()));
   QObject::connect(emulator,SIGNAL(emulatorReset()),model,SLOT(update()));
   QObject::connect(emulator,SIGNAL(emulatorPaused(bool)),model,SLOT(update()));
   QObject::connect(emulator,SIGNAL(updateDebuggers()),model,SLOT(update()));
   QObject::connect(breakpointWatcher,SIGNAL(breakpointHit()),model,SLOT(update()));
}

SymbolWatchDockWidget::~SymbolWatchDockWidget()
{
   delete ui;
   delete model;
   delete valueDelegate;
   delete symbolDelegate;
}

void SymbolWatchDockWidget::keyPressEvent(QKeyEvent *event)
{
   if ( (event->key() == Qt::Key_Delete) &&
        (ui->tableView->currentIndex().row() >= 0) )
   {
      model->removeRow(ui->tableView->currentIndex().row(),QModelIndex());
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

      model->insertRow(text,QModelIndex());

      event->acceptProposedAction();
   }
}

bool SymbolWatchDockWidget::serialize(QDomDocument& doc, QDomNode& node)
{
   QDomElement element = addElement( doc, node, "symbolinspector" );
   QStringList symbols = model->getSymbols();

   for (int i=0; i < symbols.count(); i++)
   {
      QDomElement symbolElement = addElement( doc, element, "symbol" );
      symbolElement.setAttribute("name",symbols.at(i));
   }

   return true;
}

bool SymbolWatchDockWidget::deserialize(QDomDocument& doc, QDomNode& node, QString& errors)
{
   QDomNode childNode = node.firstChild();
   QDomNode symbolNode;
   QStringList symbols;
   QString symbol;

   if (!childNode.isNull())
   {
      do
      {
         if (childNode.nodeName() == "symbolinspector")
         {
            symbolNode = childNode.firstChild();
            do
            {
               QDomElement symbolElement = symbolNode.toElement();

               symbol = symbolElement.attribute("name");
               if ( !symbol.isEmpty() )
               {
                  symbols.append(symbol);
               }
            } while (!(symbolNode = symbolNode.nextSibling()).isNull());

            model->setSymbols(symbols);
            model->update();
         }
      } while (!(childNode = childNode.nextSibling()).isNull());
   }

   return true;
}
