#include "propertyeditordialog.h"
#include "ui_propertyeditordialog.h"

PropertyEditorDialog::PropertyEditorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PropertyEditorDialog)
{
   ui->setupUi(this);

   property.name = "";
   property.type = propertyInteger;
   property.value = "";

   ui->propertyValues->setCurrentWidget(ui->valuePage);

   enumModel = new CPropertyEnumListModel(true);
   ui->enumView->setModel(enumModel);

   enumSymbolDelegate = new CPropertySymbolDelegate();
   ui->enumView->setItemDelegateForColumn(PropertyEnumCol_Symbol,enumSymbolDelegate);

   // Reuse symbol delegate as it's really not much more than a current-value parrot.
   ui->enumView->setItemDelegateForColumn(PropertyEnumCol_Value,enumSymbolDelegate);
}

PropertyEditorDialog::~PropertyEditorDialog()
{
   delete ui;
}

void PropertyEditorDialog::showEvent(QShowEvent */*event*/)
{
   ui->name->setText(property.name);
   ui->type->setCurrentIndex(property.type);

   // Can't edit the property name if it's not empty.
   ui->name->setReadOnly(!property.name.isEmpty());

   // Can't change the property type if the name's not empty.
   ui->type->setDisabled(!property.name.isEmpty());

   // Pick the right propertyValue page to show.
   switch ( property.type )
   {
   case propertyInteger:
      ui->propertyValues->setCurrentWidget(ui->valuePage);
      ui->value->setText(property.value);
      break;
   case propertyBoolean:
      ui->propertyValues->setCurrentWidget(ui->booleanPage);
      if ( property.value == "true" )
      {
         ui->trueBoolean->setChecked(true);
      }
      else
      {
         ui->falseBoolean->setChecked(true);
      }
      break;
   case propertyString:
      ui->propertyValues->setCurrentWidget(ui->valuePage);
      ui->value->setText(property.value);
      break;
   case propertyEnumeration:
      ui->propertyValues->setCurrentWidget(ui->enumPage);
      enumModel->setItems(property.value);
      enumModel->update();
      break;
   }
}

void PropertyEditorDialog::keyPressEvent(QKeyEvent *event)
{
   if ( property.type == propertyEnumeration )
   {
      if ( event->key() == Qt::Key_Delete )
      {
         if ( ui->enumView->currentIndex().isValid() )
         {
            enumModel->removeRow(ui->enumView->currentIndex().row());
         }
      }
   }
}

void PropertyEditorDialog::on_buttonBox_accepted()
{
   property.name = ui->name->text();
   property.type = (propertyTypeEnum)ui->type->currentIndex();

   // Grab the appropriate property value.
   switch ( property.type )
   {
   case propertyInteger:
      property.value = ui->value->text();
      break;
   case propertyBoolean:
      if ( ui->trueBoolean->isChecked() )
      {
         property.value = "true";
      }
      else
      {
         property.value = "false";
      }
      break;
   case propertyString:
      property.value = ui->value->text();
      break;
   case propertyEnumeration:
      property.value = enumModel->getItems();
      break;
   }
}

void PropertyEditorDialog::on_type_currentIndexChanged(int index)
{
   property.type = (propertyTypeEnum)index;

   // Pick the right propertyValue page to show.
   switch ( index )
   {
   case propertyInteger:
      ui->propertyValues->setCurrentWidget(ui->valuePage);
      break;
   case propertyBoolean:
      ui->propertyValues->setCurrentWidget(ui->booleanPage);
      break;
   case propertyString:
      ui->propertyValues->setCurrentWidget(ui->valuePage);
      break;
   case propertyEnumeration:
      ui->propertyValues->setCurrentWidget(ui->enumPage);
      break;
   }
}
