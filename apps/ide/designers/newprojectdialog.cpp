#include "newprojectdialog.h"
#include "ui_newprojectdialog.h"

#include "projectpropertiesdialog.h"
#include "main.h"

#include <QFileDialog>
#include <QSettings>

NewProjectDialog::NewProjectDialog(QString windowTitle,QString defName,QString defPath,bool addOnsOnly,QWidget* parent) :
   QDialog(parent),
   ui(new Ui::NewProjectDialog)
{
   ui->setupUi(this);
   ui->name->setText(defName);
   ui->path->setText(defPath);
   on_path_textChanged(defPath);
   setWindowTitle(windowTitle);
   ui->name->setFocus();

   ui->target->setItemData(0,"NES",Qt::UserRole);
   ui->target->setItemData(1,"C64",Qt::UserRole);

   ui->tabWidget->setCurrentIndex(0);
   if ( addOnsOnly )
   {
      ui->tabWidget->setTabEnabled(0,false);
   }

   QDir templatesDir(":/templates/NES");
   QStringList templates = templatesDir.entryList();

   ui->templateProject->addItem("Empty Project");
   ui->templateProject->addItems(templates);

   QDir addonsDir(":/addons/NES");
   QStringList addons = addonsDir.entryList();

   ui->addOnsList->addItems(addons);
   int idx;
   for ( idx = 0; idx < ui->addOnsList->count(); idx++ )
   {
      QListWidgetItem *pItem = ui->addOnsList->item(idx);
      QString addon_uri = ":/addons/NES/"+pItem->text();
      pItem->setData(Qt::UserRole,addon_uri);
      if ( nesicideProject->getProjectAddOns().contains(addon_uri) )
      {
         pItem->setCheckState(Qt::Checked);
      }
      else
      {
         pItem->setCheckState(Qt::Unchecked);
      }
   }

   ui->addOnsList->installEventFilter(this);
}

NewProjectDialog::~NewProjectDialog()
{
   delete ui;
}

void NewProjectDialog::changeEvent(QEvent* e)
{
   QDialog::changeEvent(e);

   switch (e->type())
   {
      case QEvent::LanguageChange:
         ui->retranslateUi(this);
         break;
      default:
         break;
   }
}

bool NewProjectDialog::eventFilter(QObject *obj, QEvent *event)
{
   int checkedCount = 0;

   if ( obj == ui->addOnsList )
   {
      if ( event->type() == QEvent::Enter ||
           event->type() == QEvent::Leave )
      {
         int idx;
         for ( idx = 0; idx < ui->addOnsList->count(); idx++ )
         {
            QListWidgetItem* item = ui->addOnsList->item(idx);
            item->setBackgroundColor(QColor(255,255,255,255));
            if ( item->checkState() == Qt::Checked )
            {
               checkedCount++;
            }
         }

         if ( !((checkedCount > 0) ||
              (ui->addOnsList->selectedItems().count() > 0)) )
         {
            ui->addOnReadme->setHtml("");
         }
      }
   }
}

QString NewProjectDialog::getName()
{
   return ui->name->text();
}

QString NewProjectDialog::getPath()
{
   return ui->path->text();
}

QString NewProjectDialog::getTarget()
{
   return ui->target->currentText();
}

QString NewProjectDialog::getTargetShort()
{
   return ui->target->itemData(ui->target->currentIndex(),Qt::UserRole).toString();
}

QString NewProjectDialog::getTemplate()
{
   return ui->templateProject->currentText();
}

int NewProjectDialog::getTemplateIndex()
{
   return ui->templateProject->currentIndex();
}

QStringList NewProjectDialog::getAddOns()
{
   QStringList addons;
   int idx;

   for ( idx = 0; idx < ui->addOnsList->count(); idx++ )
   {
      QListWidgetItem *pItem = ui->addOnsList->item(idx);
      QString addon_uri = ":/addons/NES/"+pItem->text();
      if ( pItem->checkState() == Qt::Checked )
      {
         addons << addon_uri;
      }
   }

   return addons;
}

void NewProjectDialog::on_pathBrowse_clicked()
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");

   QString path = settings.value("LastProjectBasePath").toString();

   QString value = QFileDialog::getExistingDirectory(this,"Project Path",path,QFileDialog::ShowDirsOnly);

   if ( !value.isEmpty() )
   {
      ui->path->setText(value);

      QFileInfo fileInfo(value);

      settings.setValue("LastProjectBasePath",fileInfo.absoluteFilePath());
   }
}

void NewProjectDialog::on_path_textChanged(QString /*text*/)
{
   ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(checkValidity());
}

void NewProjectDialog::on_name_textChanged(QString /*text*/)
{
   ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(checkValidity());
}

bool NewProjectDialog::checkValidity()
{
   QDir check(ui->path->text());

   if ( !ui->path->text().isEmpty() && !ui->name->text().isEmpty() )
   {
      return true;
   }
   else
   {
      return false;
   }
}

void NewProjectDialog::on_buttonBox_accepted()
{
   // If they hit Project Properties button then we're all set except for the project name may have changed.
   if ( !nesicideProject->isInitialized() )
   {
      setupProject();
   }
   else
   {
      nesicideProject->setProjectTitle(getName());
      nesicideProject->setDirty(true);
   }
}

void NewProjectDialog::setupProject()
{
   QDir check(ui->path->text());

   if ( !check.exists() )
   {
      check.mkpath(ui->path->text());
   }

   QDir::setCurrent(getPath());

   // Set project target before initializing project...
   if ( getTarget() == "Commodore 64" )
   {
      nesicideProject->setProjectTarget("c64");
   }
   else if ( getTarget() == "Nintendo Entertainment System" )
   {
      nesicideProject->setProjectTarget("nes");
   }
   nesicideProject->initializeProject();
   nesicideProject->setDirty(true);
   nesicideProject->setProjectTitle(getName());
}

void NewProjectDialog::on_target_currentIndexChanged(QString target)
{
   QStringList templates;
   QStringList addons;
   QString addonsLoc;

   ui->templateProject->clear();
   ui->templateProject->addItem("Empty Project");
   ui->addOnsList->clear();

   if ( target == "Nintendo Entertainment System" )
   {
      QDir templatesDir(":/templates/NES");
      templates = templatesDir.entryList();

      QDir addonsDir(":/addons/NES");
      addonsLoc = ":/addons/NES";
      addons = addonsDir.entryList();
   }
   else if ( target == "Commodore 64" )
   {
      QDir templatesDir(":/templates/C64");
      templates = templatesDir.entryList();

      QDir addonsDir(":/addons/C64");
      addonsLoc = ":/addons/C64";
      addons = addonsDir.entryList();
   }
   ui->templateProject->addItems(templates);
   ui->addOnsList->addItems(addons);
   int idx;
   for ( idx = 0; idx < ui->addOnsList->count(); idx++ )
   {
      QListWidgetItem *pItem = ui->addOnsList->item(idx);
      pItem->setCheckState(Qt::Unchecked);
      pItem->setData(Qt::UserRole,addonsLoc+pItem->text());
   }
}

void NewProjectDialog::on_projectProperties_clicked()
{
   ProjectPropertiesDialog* dlg;

   setupProject();

   dlg = new ProjectPropertiesDialog();

   if (dlg->exec() == QDialog::Accepted)
   {
      nesicideProject->setDirty(true);
      // They might have changed the project name...
      ui->name->setText(nesicideProject->getProjectTitle());
   }

   delete dlg;
}

void NewProjectDialog::on_addOnsList_itemClicked(QListWidgetItem *item)
{
   QString readmeFileName = ":/addons/";

   readmeFileName += getTargetShort();
   readmeFileName += "/";
   readmeFileName += item->text();
   readmeFileName += "/README.HTM";

   item->setBackgroundColor(QColor(248,248,248,255));

   QFileInfo fi(readmeFileName);
   if ( fi.exists() )
   {
      QFile f(readmeFileName);
      if ( f.open(QIODevice::ReadOnly) )
      {
         QByteArray b = f.readAll();
         ui->addOnReadme->setHtml(b);
      }
   }
   else
   {
      ui->addOnReadme->setHtml("");
   }

}

void NewProjectDialog::on_addOnsList_itemEntered(QListWidgetItem *item)
{
    on_addOnsList_itemClicked(item);
}
