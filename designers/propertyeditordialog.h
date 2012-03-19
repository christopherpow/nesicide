#ifndef PROPERTYEDITORDIALOG_H
#define PROPERTYEDITORDIALOG_H

#include <QDialog>
#include <QKeyEvent>

#include "cpropertyitem.h"
#include "cpropertyenumlistmodel.h"
#include "cpropertysymboldelegate.h"

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
   void keyPressEvent(QKeyEvent *event);

private:
   Ui::PropertyEditorDialog *ui;
   CPropertyEnumListModel* enumModel;
   CPropertySymbolDelegate* enumSymbolDelegate;
   PropertyItem property;

private slots:
   void on_type_currentIndexChanged(int index);
   void on_buttonBox_accepted();
};

#endif // PROPERTYEDITORDIALOG_H
