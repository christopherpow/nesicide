#include "searchbar.h"
#include "ui_searchbar.h"

#include <QSettings>

SearchBar::SearchBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SearchBar)
{
   QSettings settings;
   QStringList searches = settings.value("SearchBarHistory").toStringList();

   ui->setupUi(this);

   ui->searchText->addItem("");
   foreach ( QString search, searches )
   {
      ui->searchText->addItem(search);
   }
}

SearchBar::~SearchBar()
{
   delete ui;
}

void SearchBar::on_searchText_activated(QString search)
{
   QSettings settings;
   QStringList items;

   if ( !search.isEmpty() )
   {
      items = settings.value("SearchBarHistory").toStringList();
      if ( !items.contains(search) )
      {
         if ( items.count() >= 100 ) // CPTODO: arbitrary limit, consider propertyizing?
         {
            items.removeAt(0);
         }
         items.append(search);
      }
      settings.setValue("SearchBarHistory",QVariant(items));

      emit snapTo("SearchBar:"+search);
   }
}

void SearchBar::on_searchText_editTextChanged(QString search)
{
   if ( !search.isEmpty() )
   {
      emit snapTo("SearchBar:"+search);
   }
}
