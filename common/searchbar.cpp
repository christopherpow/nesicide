#include "searchbar.h"
#include "ui_searchbar.h"

#include <QSettings>

SearchBar::SearchBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SearchBar)
{
   QSettings settings;
   QStringList searches;

   ui->setupUi(this);

   settings.beginGroup("SearchBar");
   searches = settings.value("SearchTextHistory").toStringList();
   ui->caseSensitive->setChecked(settings.value("CaseSensitive",QVariant(false)).toBool());
   ui->regex->setChecked(settings.value("RegularExpression",QVariant(false)).toBool());
   ui->direction->setChecked(settings.value("Direction",QVariant(true)).toBool());
   settings.endGroup();

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

void SearchBar::focusInEvent(QFocusEvent *event)
{
   ui->searchText->setFocus();
}

void SearchBar::on_searchText_activated(QString search)
{
   QSettings settings;
   QStringList items;

   settings.beginGroup("SearchBar");
   if ( !search.isEmpty() )
   {
      items = settings.value("SearchTextHistory").toStringList();
      if ( !items.contains(search) )
      {
         if ( items.count() >= 100 ) // CPTODO: arbitrary limit, consider propertyizing?
         {
            items.removeAt(0);
         }
         items.append(search);
      }
      settings.setValue("SearchBarHistory",QVariant(items));

      emit snapTo("SearchBar:"
                  +QString::number(ui->caseSensitive->isChecked())+":"
                  +QString::number(ui->regex->isChecked())+":"
                  +QString::number(ui->direction->isChecked())+":"
                  +search);
   }
   settings.endGroup();
}

void SearchBar::on_searchText_editTextChanged(QString search)
{
   if ( !search.isEmpty() )
   {
      emit snapTo("SearchBar:"
                  +QString::number(ui->caseSensitive->isChecked())+":"
                  +QString::number(ui->regex->isChecked())+":"
                  +QString::number(ui->direction->isChecked())+":"
                  +search);
   }
}

void SearchBar::on_caseSensitive_toggled(bool checked)
{
   QSettings settings;

   settings.beginGroup("SearchBar");
   settings.setValue("CaseSensitive",QVariant(checked));
   settings.endGroup();
}

void SearchBar::on_regex_toggled(bool checked)
{
   QSettings settings;

   settings.beginGroup("SearchBar");
   settings.setValue("RegularExpression",QVariant(checked));
   settings.endGroup();
}

void SearchBar::on_direction_toggled(bool checked)
{
   QSettings settings;

   settings.beginGroup("SearchBar");
   settings.setValue("Direction",QVariant(checked));
   settings.endGroup();
}
