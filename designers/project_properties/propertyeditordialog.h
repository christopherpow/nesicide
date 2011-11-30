#ifndef PROPERTYEDITORDIALOG_H
#define PROPERTYEDITORDIALOG_H

#include <QDialog>

#include "cpropertyitem.h"

namespace Ui {
   class PropertyEditorDialog;
}

class PropertyEditorDialog : public QDialog
{
   Q_OBJECT

public:
   explicit PropertyEditorDialog(QWidget *parent = 0);
   ~PropertyEditorDialog();

   QString propertyName() { return property.name; }
   propertyTypeEnum propertyType() { return property.type; }
   QString propertyValue() { return property.value; }

   void setPropertyName(QString name) { property.name = name; }
   void setPropertyType(propertyTypeEnum type) { property.type = type; }
   void setPropertyValue(QString value) { property.value = value; }

protected:
   void showEvent(QShowEvent *event);

private:
   Ui::PropertyEditorDialog *ui;
   PropertyItem property;

private slots:
   void on_removeValue_clicked();
   void on_addValue_clicked();
   void on_buttonBox_accepted();
};

#endif // PROPERTYEDITORDIALOG_H
