#include "chrbankitemstabwidget.h"
#include "ui_chrbankitemstabwidget.h"

#include <QListView>
#include "cchrromitemlistdisplaymodel.h"
#include "cchrromitemtabledisplaymodel.h"
#include "iprojecttreeviewitem.h"

ChrBankItemsTabWidget::ChrBankItemsTabWidget(QWidget *parent) :
   QTabWidget(parent),
   ui(new Ui::ChrBankItemsTabWidget)
{
   ui->setupUi(this);
}

ChrBankItemsTabWidget::~ChrBankItemsTabWidget()
{
   QListView* pListView;
   int tab;

   // Delete existing tabs.
   for ( tab = count()-1; tab >= 0; tab-- )
   {
      pListView = dynamic_cast<QListView*>(widget(tab));

      removeTab(tab);

      delete pListView->model();
      delete pListView;
   }

   delete ui;
}

void ChrBankItemsTabWidget::setItems(QList<IChrRomBankItem *> items)
{
   QMap<QString,CChrRomItemListDisplayModel*> map;
   QList<IChrRomBankItem*> tabItems;
   QListView* pListView;
   CChrRomItemListDisplayModel* pModel;
   QString text;
   QImage  image;
   int tab;
   int item;
   bool found;

   // Delete existing tabs.
   for ( tab = count()-1; tab >= 0; tab-- )
   {
      pListView = dynamic_cast<QListView*>(widget(tab));

      removeTab(tab);

      delete pListView->model();
      delete pListView;
   }

   // Create new tabs for all different sizes.
   for ( item = 0; item < items.count(); item++ )
   {
      image = items.at(item)->getChrRomBankItemImage();

      text.sprintf("%d x %d",image.size().width(),image.size().height());

      found = false;
      for ( tab = 0; tab < count(); tab++ )
      {
         if ( tabText(tab) == text )
         {
            found = true;
            break;
         }
      }

      if ( !found )
      {
         pListView = new QListView();

         pListView->setFrameShape(QFrame::NoFrame);

         pModel = new CChrRomItemListDisplayModel();

         pListView->setModel(pModel);
         pListView->setModelColumn(ChrRomBankItemCol_Image);

         QObject::connect(pListView,SIGNAL(activated(QModelIndex)),this,SLOT(tileList_activated(QModelIndex)));
         QObject::connect(pListView,SIGNAL(clicked(QModelIndex)),this,SLOT(tileList_clicked(QModelIndex)));
         QObject::connect(pListView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(tileList_doubleClicked(QModelIndex)));

         map.insert(text,pModel);

         addTab(pListView,text);
      }
   }

   // Put each item into the right list.
   for ( tab = 0; tab < count(); tab++ )
   {
      tabItems.clear();

      pModel = map.find(tabText(tab)).value();

      for ( item = 0; item < items.count(); item++ )
      {
         image = items.at(item)->getChrRomBankItemImage();

         text.sprintf("%d x %d",image.size().width(),image.size().height());

         if ( text == tabText(tab) )
         {
            tabItems.append(items.at(item));
         }
      }

      pModel->setBankItems(tabItems);
      pModel->update();
   }
}

QModelIndex ChrBankItemsTabWidget::currentIndex() const
{
   QListView* pListView = dynamic_cast<QListView*>(currentWidget());

   if ( pListView )
   {
      return pListView->currentIndex();
   }

   return QModelIndex();
}

void ChrBankItemsTabWidget::setCurrentIndex(QModelIndex index)
{
   int tab;

   // Put each item into the right list.
   for ( tab = 0; tab < count(); tab++ )
   {
      QListView* pListView = dynamic_cast<QListView*>(currentWidget());
      pListView->setCurrentIndex(index);
   }
}

void ChrBankItemsTabWidget::tileList_activated(QModelIndex index)
{
   if ( index.isValid() )
   {
      emit tileSelected(index);
   }
}

void ChrBankItemsTabWidget::tileList_clicked(QModelIndex index)
{
   if ( index.isValid() )
   {
      emit tileSelected(index);
   }
}

void ChrBankItemsTabWidget::tileList_doubleClicked(QModelIndex index)
{
   if ( index.isValid() )
   {
      QListView* pListView = dynamic_cast<QListView*>(currentWidget());
      CChrRomItemListDisplayModel* pModel = dynamic_cast<CChrRomItemListDisplayModel*>(pListView->model());
      IChrRomBankItem* pChrItem = pModel->bankItems().at(index.row());
      IProjectTreeViewItem* pProjItem = dynamic_cast<IProjectTreeViewItem*>(pChrItem);

      emit snapToTab("Tile,"+pProjItem->uuid());
   }
}
