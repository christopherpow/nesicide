#include "graphicsbankadditemsdialog.h"
#include "ui_graphicsbankadditemsdialog.h"
#include "cnesicideproject.h"

GraphicsBankAddItemsDialog::GraphicsBankAddItemsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GraphicsBankAddItemsDialog)
{
    ui->setupUi(this);
    model = new CChrRomItemListDisplayModel(ui->tableView);

    for (int i=0; i < nesicideProject->childCount(); i++)
      enumChildren(nesicideProject->child(i));
    model->layoutChangedEvent();
    ui->tableView->setModel(model);
    ui->tableView->setSelectionMode(QAbstractItemView::MultiSelection);
    ui->tableView->setColumnWidth(0, 26);
    ui->tableView->setColumnWidth(1, 90);
    ui->tableView->setColumnWidth(2, 140);
    ui->tableView->setColumnWidth(3, 70);
}

void GraphicsBankAddItemsDialog::enumChildren(IProjectTreeViewItem* item)
{
   if (IChrRomBankItem* chrRomItem = dynamic_cast<IChrRomBankItem*>(item))
   {
      model->chrRomBankItems.append(chrRomItem);
   }

   for (int i=0; i < item->childCount(); i++)
     enumChildren(item->child(i));
}

GraphicsBankAddItemsDialog::~GraphicsBankAddItemsDialog()
{
    delete ui;
    delete model;
}

void GraphicsBankAddItemsDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void GraphicsBankAddItemsDialog::on_buttonBox_accepted()
{
   this->setResult(QDialog::Accepted);
   this->close();
}

void GraphicsBankAddItemsDialog::on_buttonBox_rejected()
{
   this->setResult(QDialog::Rejected);
   this->close();
}
