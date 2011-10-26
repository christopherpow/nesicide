#include "searchdockwidget.h"
#include "ui_searchdockwidget.h"

#include <QFileDialog>
#include <QCompleter>

#include "main.h"

SearchDockWidget::SearchDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::SearchDockWidget)
{
   QSettings settings;

   ui->setupUi(this);

   ui->replaceText->addItem("");

   settings.beginGroup("Search");
   ui->searchText->addItems(settings.value("SearchTextHistory").toStringList());
   ui->location->addItems(settings.value("SearchLocationHistory").toStringList());
   ui->type->addItems(settings.value("FileTypeHistory").toStringList());
   ui->subfolders->setChecked(settings.value("IncludeSubfolders",QVariant(true)).toBool());
   ui->sourceSearchPaths->setChecked(settings.value("IncludeSourceSearchPaths",QVariant(false)).toBool());
   ui->caseSensitive->setChecked(settings.value("CaseSensitive",QVariant(false)).toBool());
   ui->regex->setChecked(settings.value("RegularExpression",QVariant(false)).toBool());
   ui->projectFolder->setChecked(settings.value("UseProjectFolder",QVariant(true)).toBool());
   ui->replaceText->addItems(settings.value("ReplaceTextHistory").toStringList());
   settings.endGroup();

   on_projectFolder_clicked(ui->projectFolder->isChecked());

   ui->searchText->completer()->setCompletionMode(QCompleter::PopupCompletion);
   ui->location->completer()->setCompletionMode(QCompleter::PopupCompletion);
   ui->type->completer()->setCompletionMode(QCompleter::PopupCompletion);

   searchBar = new SearchBar("SearchReplaceBar");
   ui->searchBarLayout->addWidget(searchBar);

   QObject::connect(searchBar,SIGNAL(snapTo(QString)),this,SIGNAL(snapTo(QString)));

   ui->replaceText->completer()->setCompletionMode(QCompleter::PopupCompletion);

   QObject::connect(searcher,SIGNAL(searchDone(int)),this,SLOT(searcher_searchDone(int)));
}

SearchDockWidget::~SearchDockWidget()
{
   delete ui;
   delete searchBar;
}

void SearchDockWidget::showEvent(QShowEvent *event)
{
   if ( !ui->location->count() )
   {
      ui->location->addItem(QDir::currentPath());
   }
   if ( !ui->type->count() )
   {
      ui->type->addItem("*");
      ui->type->addItem("*.s");
   }
   ui->searchText->setFocus();
}

void SearchDockWidget::on_browse_clicked()
{
   QString dir = QFileDialog::getExistingDirectory(this,"Search in...",QDir::currentPath());
   if ( !dir.isEmpty() )
   {
      ui->location->addItem(dir);
      ui->location->setEditText(dir);
   }
}

void SearchDockWidget::on_find_clicked()
{
   QSettings settings;
   QStringList items;

   settings.beginGroup("Search");

   if ( !ui->searchText->currentText().isEmpty() )
   {
      settings.setValue("IncludeSubfolders",QVariant(ui->subfolders->isChecked()));
      settings.setValue("IncludeSourceSearchPaths",QVariant(ui->sourceSearchPaths->isChecked()));
      settings.setValue("CaseSensitive",QVariant(ui->caseSensitive->isChecked()));
      settings.setValue("RegularExpression",QVariant(ui->regex->isChecked()));
      settings.setValue("UseProjectFolder",QVariant(ui->projectFolder->isChecked()));
      if ( (!ui->location->currentText().isEmpty()) && (ui->location->findText(ui->location->currentText(),Qt::MatchExactly|Qt::MatchCaseSensitive) == -1) )
      {
         ui->location->addItem(ui->location->currentText());
         items = settings.value("SearchLocationHistory").toStringList();
         if ( !items.contains(ui->location->currentText()) )
         {
            if ( items.count() >= 100 ) // CPTODO: arbitrary limit, consider propertyizing?
            {
               items.removeAt(0);
            }
            items.append(ui->location->currentText());
         }
         settings.setValue("SearchLocationHistory",QVariant(items));
      }
      if ( (!ui->type->currentText().isEmpty()) && (ui->type->findText(ui->type->currentText(),Qt::MatchExactly|Qt::MatchCaseSensitive) == -1) )
      {
         ui->type->addItem(ui->type->currentText());
         items = settings.value("FileTypeHistory").toStringList();
         if ( !items.contains(ui->type->currentText()) )
         {
            if ( items.count() >= 100 ) // CPTODO: arbitrary limit, consider propertyizing?
            {
               items.removeAt(0);
            }
            items.append(ui->type->currentText());
         }
         settings.setValue("FileTypeHistory",QVariant(items));
      }
      if ( ui->searchText->findText(ui->searchText->currentText(),Qt::MatchExactly|Qt::MatchCaseSensitive) == -1 )
      {
         ui->searchText->addItem(ui->searchText->currentText());
         items = settings.value("SearchTextHistory").toStringList();
         if ( !items.contains(ui->searchText->currentText()) )
         {
            if ( items.count() >= 100 ) // CPTODO: arbitrary limit, consider propertyizing?
            {
               items.removeAt(0);
            }
            items.append(ui->searchText->currentText());
         }
         settings.setValue("SearchTextHistory",QVariant(items));
      }
      searchTextLogger->erase();
      searchTextLogger->write("<b>Searching for \""+ui->searchText->currentText()+"\"...</b>");
      if ( ui->projectFolder->isChecked() )
      {
         searcher->search(QDir::currentPath(),ui->searchText->currentText(),ui->type->currentText(),ui->subfolders->isChecked(),ui->sourceSearchPaths->isChecked(),ui->regex->isChecked(),ui->caseSensitive->isChecked());
      }
      else
      {
         searcher->search(ui->location->currentText(),ui->searchText->currentText(),ui->type->currentText(),ui->subfolders->isChecked(),ui->sourceSearchPaths->isChecked(),ui->regex->isChecked(),ui->caseSensitive->isChecked());
      }
   }
   settings.endGroup();
}

void SearchDockWidget::searcher_searchDone(int found)
{
   searchTextLogger->write("<b>"+QString::number(found)+" found.</b>");
}

void SearchDockWidget::on_projectFolder_clicked(bool checked)
{
   ui->location->setEnabled(!checked);
   ui->browse->setEnabled(!checked);
}

void SearchDockWidget::on_findForReplace_clicked()
{
   emit snapTo("SearchBar,"
               +QString::number(searchBar->isCaseSensitive())+","
               +QString::number(searchBar->isRegularExpression())+","
               +QString::number(searchBar->searchIsDown())+","
               +searchBar->currentSearchText());
}

void SearchDockWidget::on_replace_clicked()
{
   QSettings   settings;
   QStringList items;

   settings.beginGroup("Search");

   if ( (!ui->replaceText->currentText().isEmpty()) && (ui->replaceText->findText(ui->replaceText->currentText(),Qt::MatchExactly|Qt::MatchCaseSensitive) == -1) )
   {
      ui->replaceText->addItem(ui->replaceText->currentText());
      items = settings.value("ReplaceTextHistory").toStringList();
      if ( !items.contains(ui->replaceText->currentText()) )
      {
         if ( items.count() >= 100 ) // CPTODO: arbitrary limit, consider propertyizing?
         {
            items.removeAt(0);
         }
         items.append(ui->replaceText->currentText());
      }
      settings.setValue("ReplaceTextHistory",QVariant(items));
   }

   settings.endGroup();

   emit replaceText(searchBar->snapTo(),ui->replaceText->currentText(),false);
}

void SearchDockWidget::on_replaceAll_clicked()
{
   QSettings   settings;
   QStringList items;

   settings.beginGroup("Search");

   if ( (!ui->replaceText->currentText().isEmpty()) && (ui->replaceText->findText(ui->replaceText->currentText(),Qt::MatchExactly|Qt::MatchCaseSensitive) == -1) )
   {
      ui->replaceText->addItem(ui->replaceText->currentText());
      items = settings.value("ReplaceTextHistory").toStringList();
      if ( !items.contains(ui->replaceText->currentText()) )
      {
         if ( items.count() >= 100 ) // CPTODO: arbitrary limit, consider propertyizing?
         {
            items.removeAt(0);
         }
         items.append(ui->replaceText->currentText());
      }
      settings.setValue("ReplaceTextHistory",QVariant(items));
   }

   settings.endGroup();

   emit replaceText(searchBar->snapTo(),ui->replaceText->currentText(),true);
}
