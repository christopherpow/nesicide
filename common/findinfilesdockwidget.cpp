#include "findinfilesdockwidget.h"
#include "ui_findinfilesdockwidget.h"

#include <QFileDialog>

#include "main.h"

FindInFilesDockWidget::FindInFilesDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::FindInFilesDockWidget)
{
   QSettings settings;

   ui->setupUi(this);

   settings.beginGroup("FindInFiles");
   ui->searchText->addItems(settings.value("SearchTextHistory").toStringList());
   ui->location->addItems(settings.value("SearchLocationHistory").toStringList());
   ui->type->addItems(settings.value("FileTypeHistory").toStringList());
   ui->caseSensitive->setChecked(settings.value("CaseSensitive",QVariant(false)).toBool());
   ui->regex->setChecked(settings.value("RegularExpression",QVariant(false)).toBool());
   settings.endGroup();

   QObject::connect(searcher,SIGNAL(searchDone(int)),this,SLOT(searcher_searchDone(int)));
}

FindInFilesDockWidget::~FindInFilesDockWidget()
{
   delete ui;
}

void FindInFilesDockWidget::showEvent(QShowEvent *event)
{
   if ( !ui->location->count() )
   {
      ui->location->addItem(QDir::currentPath());
   }
   if ( !ui->type->count() )
   {
      ui->type->addItem("*");
   }
}

void FindInFilesDockWidget::on_browse_clicked()
{
   QString dir = QFileDialog::getExistingDirectory(this,"Find in...",QDir::currentPath());
   if ( !dir.isEmpty() )
   {
      ui->location->addItem(dir);
      ui->location->setEditText(dir);
   }
}

void FindInFilesDockWidget::on_find_clicked()
{
   QSettings settings;
   QStringList items;
   int found = 0;

   settings.beginGroup("FindInFiles");

   if ( !ui->searchText->currentText().isEmpty() )
   {
      settings.setValue("CaseSensitive",QVariant(ui->caseSensitive->isChecked()));
      settings.setValue("RegularExpression",QVariant(ui->regex->isChecked()));
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
      searcher->search(ui->location->currentText(),ui->searchText->currentText(),ui->type->currentText(),ui->regex->isChecked(),ui->caseSensitive->isChecked());
   }
   settings.endGroup();
}

void FindInFilesDockWidget::searcher_searchDone(int found)
{
   searchTextLogger->write("<b>"+QString::number(found)+" found.</b>");
}
