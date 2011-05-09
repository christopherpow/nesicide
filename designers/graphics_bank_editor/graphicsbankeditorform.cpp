#include "graphicsbankeditorform.h"
#include "ui_graphicsbankeditorform.h"

GraphicsBankEditorForm::GraphicsBankEditorForm(QWidget* parent) :
   QWidget(parent),
   ui(new Ui::GraphicsBankEditorForm)
{
   ui->setupUi(this);
   model = new CChrRomItemListDisplayModel(ui->tableView);
   model->chrRomBankItems.append(chrRomBankItems);
   ui->tableView->setModel(model);

   ui->tableView->setColumnWidth(0, 26);
}

GraphicsBankEditorForm::~GraphicsBankEditorForm()
{
   delete ui;
   delete model;
}

void GraphicsBankEditorForm::changeEvent(QEvent* e)
{
   QWidget::changeEvent(e);

   switch (e->type())
   {
      case QEvent::LanguageChange:
         ui->retranslateUi(this);
         break;
      default:
         break;
   }
}

void GraphicsBankEditorForm::on_removeItemPushButton_clicked()
{

}

void GraphicsBankEditorForm::on_AddItemPushButton_clicked()
{
   GraphicsBankAddItemsDialog* dlg = new GraphicsBankAddItemsDialog(this);

   if ( (dlg->exec() == QDialog::Accepted) &&
        (dlg->getSelectedBankItem()) )
   {
      chrRomBankItems.append(dlg->getSelectedBankItem());
      model->chrRomBankItems.append(dlg->getSelectedBankItem());
      ui->progressBar->setMaximum(0x2000);
      int dataSize = 0;

      for (int i=0; i<chrRomBankItems.count(); i++)
      {
         dataSize += chrRomBankItems.at(i)->getChrRomBankItemSize();
      }

      ui->progressBar->setValue(dataSize);
      model->layoutChangedEvent();
   }

   delete dlg;
}

void GraphicsBankEditorForm::updateChrRomBankItemList(QList<IChrRomBankItem*> newList)
{
   chrRomBankItems.clear();
   chrRomBankItems.append(newList);
   model->chrRomBankItems.clear();
   model->chrRomBankItems.append(newList);
   ui->progressBar->setMaximum(0x2000);
   int dataSize = 0;

   for (int i=0; i<chrRomBankItems.count(); i++)
   {
      dataSize += chrRomBankItems.at(i)->getChrRomBankItemSize();
   }

   ui->progressBar->setValue(dataSize);
   model->layoutChangedEvent();

}
