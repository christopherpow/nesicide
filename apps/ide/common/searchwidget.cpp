#include "searchwidget.h"
#include "ui_searchwidget.h"

#include <QFileDialog>
#include <QCompleter>
#include <QSettings>
#include <QKeyEvent>
#include <QLineEdit>

#include "searcherthread.h"

#include "cobjectregistry.h"

#include "cbuildertextlogger.h"

SearchWidget::SearchWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SearchWidget)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");

   ui->setupUi(this);

   settings.beginGroup("Search");
   ui->searchText->addItems(settings.value("SearchTextHistory").toStringList());
   ui->location->addItems(settings.value("SearchLocationHistory").toStringList());
   ui->type->addItems(settings.value("FileTypeHistory").toStringList());
   ui->subfolders->setChecked(settings.value("IncludeSubfolders",QVariant(true)).toBool());
   ui->sourceSearchPaths->setChecked(settings.value("IncludeSourceSearchPaths",QVariant(false)).toBool());
   ui->caseSensitive->setChecked(settings.value("CaseSensitive",QVariant(false)).toBool());
   ui->regex->setChecked(settings.value("RegularExpression",QVariant(false)).toBool());
   ui->projectFolder->setChecked(settings.value("UseProjectFolder",QVariant(true)).toBool());
   settings.endGroup();

   on_projectFolder_clicked(ui->projectFolder->isChecked());

   ui->searchText->completer()->setCompletionMode(QCompleter::PopupCompletion);
   ui->location->completer()->setCompletionMode(QCompleter::PopupCompletion);
   ui->type->completer()->setCompletionMode(QCompleter::PopupCompletion);

   QObject* searcher = CObjectRegistry::instance()->getObject("Searcher");
   qRegisterMetaType<QDir>("QDir");
   QObject::connect(this,SIGNAL(search(QDir,QString,QString,bool,bool,bool,bool)),searcher,SLOT(search(QDir,QString,QString,bool,bool,bool,bool)));
   QObject::connect(searcher,SIGNAL(searchDone(int)),this,SLOT(searcher_searchDone(int)));

   ui->searchText->installEventFilter(this);
}

SearchWidget::~SearchWidget()
{
   delete ui;
}

bool SearchWidget::eventFilter(QObject *object, QEvent *event)
{
   if ( object == ui->searchText )
   {
      if ( event->type() == QEvent::KeyPress )
      {
         QKeyEvent* keyEvent = dynamic_cast<QKeyEvent*>(event);

         if ( (keyEvent->key() == Qt::Key_Return) ||
              (keyEvent->key() == Qt::Key_Enter) )
         {
            on_find_clicked();
            return true;
         }
      }
   }
   return false;
}

void SearchWidget::showEvent(QShowEvent */*event*/)
{
   if ( !ui->location->count() )
   {
      ui->location->addItem(QDir::currentPath());
   }
   if ( !ui->type->count() )
   {
      ui->type->addItem("*");
   }
   ui->searchText->lineEdit()->selectAll();
   ui->searchText->setFocus();
}

void SearchWidget::on_browse_clicked()
{
   QString dir = QFileDialog::getExistingDirectory(this,"Search in...",QDir::currentPath());
   if ( !dir.isEmpty() )
   {
      ui->location->addItem(dir);
      ui->location->setEditText(dir);
   }
}

void SearchWidget::on_find_clicked()
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");
   QStringList items;
   SearcherThread* searcher = dynamic_cast<SearcherThread*>(CObjectRegistry::instance()->getObject("Searcher"));

   if ( !ui->searchText->currentText().isEmpty() )
   {
      settings.beginGroup("Search");

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
         emit search(QDir::currentPath(),ui->searchText->currentText(),ui->type->currentText(),ui->subfolders->isChecked(),ui->sourceSearchPaths->isChecked(),ui->regex->isChecked(),ui->caseSensitive->isChecked());
      }
      else
      {
         emit search(ui->location->currentText(),ui->searchText->currentText(),ui->type->currentText(),ui->subfolders->isChecked(),ui->sourceSearchPaths->isChecked(),ui->regex->isChecked(),ui->caseSensitive->isChecked());
      }

      settings.endGroup();
   }
}

void SearchWidget::searcher_searchDone(int found)
{
   searchTextLogger->write("<b>"+QString::number(found)+" found.</b>");
}

void SearchWidget::on_projectFolder_clicked(bool checked)
{
   ui->location->setEnabled(!checked);
   ui->browse->setEnabled(!checked);
}
