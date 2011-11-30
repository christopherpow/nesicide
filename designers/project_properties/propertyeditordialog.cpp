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


}

PropertyEditorDialog::~PropertyEditorDialog()
{
   delete ui;
}

void PropertyEditorDialog::showEvent(QShowEvent *event)
{
   ui->name->setText(property.name);
   ui->type->setCurrentIndex(property.type);

   // Can't edit the property name if it's not empty.
   ui->name->setReadOnly(!property.name.isEmpty());

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
      break;
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
      break;
   }
}

void PropertyEditorDialog::on_addValue_clicked()
{

}

void PropertyEditorDialog::on_removeValue_clicked()
{

}
