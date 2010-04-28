#include "graphicsbankeditorform.h"
#include "ui_graphicsbankeditorform.h"

GraphicsBankEditorForm::GraphicsBankEditorForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GraphicsBankEditorForm)
{
    ui->setupUi(this);
}

GraphicsBankEditorForm::~GraphicsBankEditorForm()
{
    delete ui;
}

void GraphicsBankEditorForm::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
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
   GraphicsBankAddItemsDialog *dlg = new GraphicsBankAddItemsDialog(this);
   if (dlg->exec() == QDialog::Accepted)
   {

   }
   delete dlg;
}
