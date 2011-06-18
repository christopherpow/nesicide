#include "findinfilesdockwidget.h"
#include "ui_findinfilesdockwidget.h"

#include <QFileDialog>

#include "main.h"

FindInFilesDockWidget::FindInFilesDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::FindInFilesDockWidget)
{
   ui->setupUi(this);

   model = new QFileSystemModel();
}

FindInFilesDockWidget::~FindInFilesDockWidget()
{
   delete ui;
   delete model;
}

void FindInFilesDockWidget::on_browse_clicked()
{
   QString dir = QFileDialog::getExistingDirectory(this,"Find in...",QDir::currentPath());
   if ( !dir.isEmpty() )
   {
      ui->location->addItem(dir);
   }
}

void FindInFilesDockWidget::on_find_clicked()
{
   if ( !ui->searchText->currentText().isEmpty() )
   {
      model->setRootPath(ui->location->currentText());
      searchTextLogger->erase();
      searchTextLogger->write("<b>Searching for \""+ui->searchText->currentText()+"\"...</b>");
      searchTextLogger->write("<b>0 found.</b>");
   }
}
