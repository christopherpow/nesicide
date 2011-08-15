#include "searchbar.h"
#include "ui_searchbar.h"

#include <QSettings>
#include <QCompleter>

SearchBar::SearchBar(QString settingsPrefix,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SearchBar)
{
   QSettings settings;
   QStringList searches;

   ui->setupUi(this);

   m_settingsPrefix = settingsPrefix;

   settings.beginGroup(m_settingsPrefix);
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
   ui->searchText->completer()->setCompletionMode(QCompleter::PopupCompletion);
}

SearchBar::~SearchBar()
{
   delete ui;
}

void SearchBar::focusInEvent(QFocusEvent *event)
{
   ui->searchText->setFocus();
}

QString SearchBar::currentSearchText()
{
   return ui->searchText->currentText();
}

bool    SearchBar::isCaseSensitive()
{
   return ui->caseSensitive->isChecked();
}

bool    SearchBar::isRegularExpression()
{
   return ui->regex->isChecked();
}

bool    SearchBar::searchIsDown()
{
   return ui->direction->isChecked();
}

QString SearchBar::snapTo()
{
   return QString("SearchBar,"
               +QString::number(ui->caseSensitive->isChecked())+","
               +QString::number(ui->regex->isChecked())+","
               +QString::number(ui->direction->isChecked())+","
               +ui->searchText->currentText());
}

void SearchBar::on_searchText_activated(QString search)
{
   QSettings settings;
   QStringList items;

   settings.beginGroup(m_settingsPrefix);
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
      settings.setValue("SearchTextHistory",QVariant(items));

      emit snapTo("SearchBar,"
                  +QString::number(ui->caseSensitive->isChecked())+","
                  +QString::number(ui->regex->isChecked())+","
                  +QString::number(ui->direction->isChecked())+","
                  +search);
   }
   settings.endGroup();
}

void SearchBar::on_searchText_editTextChanged(QString search)
{
   if ( !search.isEmpty() )
   {
      emit snapTo("SearchBar,"
                  +QString::number(ui->caseSensitive->isChecked())+","
                  +QString::number(ui->regex->isChecked())+","
                  +QString::number(ui->direction->isChecked())+","
                  +search);
   }
}

void SearchBar::on_caseSensitive_toggled(bool checked)
{
   QSettings settings;

   settings.beginGroup(m_settingsPrefix);
   settings.setValue("CaseSensitive",QVariant(checked));
   settings.endGroup();
}

void SearchBar::on_regex_toggled(bool checked)
{
   QSettings settings;

   settings.beginGroup(m_settingsPrefix);
   settings.setValue("RegularExpression",QVariant(checked));
   settings.endGroup();
}

void SearchBar::on_direction_toggled(bool checked)
{
   QSettings settings;

   settings.beginGroup(m_settingsPrefix);
   settings.setValue("Direction",QVariant(checked));
   settings.endGroup();
}
