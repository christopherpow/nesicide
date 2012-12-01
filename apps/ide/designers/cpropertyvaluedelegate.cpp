#include "cpropertyvaluedelegate.h"
#include "checkboxlist.h"

CPropertyValueDelegate::CPropertyValueDelegate()
{}

QWidget* CPropertyValueDelegate::createEditor(QWidget* parent,
      const QStyleOptionViewItem& /* option */,
      const QModelIndex& /*index*/) const
{
   QLineEdit* edit;
   QComboBox* comboBox;
   CheckBoxList* checkComboBox;
   QStringList itemsStrList;
   QWidget* widget = NULL;
   int idx = 0;

   switch ( m_item.type )
   {
   case propertyInteger:
   case propertyString:
      edit = new QLineEdit(parent);
      widget = dynamic_cast<QWidget*>(edit);
      break;

   case propertyBoolean:
      comboBox = new QComboBox(parent);
      comboBox->addItem("false");
      comboBox->addItem("true");
      comboBox->setEditable(false);
      widget = dynamic_cast<QWidget*>(comboBox);
      break;
   case propertyEnumeration:
      checkComboBox = new CheckBoxList(parent);

      itemsStrList = m_item.value.split(";",QString::SkipEmptyParts);
      foreach ( QString itemStr, itemsStrList )
      {
         QStringList itemParts = itemStr.split(",");
         QString itemValue;
         itemValue = itemParts.at(1);
         itemValue += "=";
         itemValue += itemParts.at(2);
         checkComboBox->addItem(itemValue);
         checkComboBox->setItemData(idx,itemParts.at(0).toInt());
         idx++;
      }

      checkComboBox->setEditable(false);
      widget = dynamic_cast<QWidget*>(checkComboBox);
      break;
   }
   return widget;
}

void CPropertyValueDelegate::setEditorData(QWidget* editor,
      const QModelIndex& index) const
{
   QLineEdit* edit;
   QComboBox* comboBox;
   CheckBoxList* checkComboBox;

   switch ( m_item.type )
   {
   case propertyInteger:
   case propertyString:
      edit = static_cast<QLineEdit*>(editor);
      edit->setText(index.model()->data(index, Qt::DisplayRole).toString());
      break;

   case propertyBoolean:
      comboBox = static_cast<QComboBox*>(editor);
      comboBox->setCurrentIndex(comboBox->findText(index.model()->data(index, Qt::DisplayRole).toString()));
      break;
   case propertyEnumeration:
      checkComboBox = static_cast<CheckBoxList*>(editor);
      checkComboBox->SetDisplayText(index.model()->data(index, Qt::DisplayRole).toString());
      checkComboBox->showPopup();
      break;
   }
}

void CPropertyValueDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
      const QModelIndex& index) const
{
   QLineEdit* edit;
   QComboBox* comboBox;
   CheckBoxList* checkComboBox;
   QStringList itemsStrList;
   QString itemStr;
   int idx = 0;

   switch ( m_item.type )
   {
   case propertyInteger:
   case propertyString:
      edit = static_cast<QLineEdit*>(editor);
      model->setData(index, edit->text(), Qt::EditRole);
      break;

   case propertyBoolean:
      comboBox = static_cast<QComboBox*>(editor);
      model->setData(index, comboBox->currentText(), Qt::EditRole);
      break;
   case propertyEnumeration:
      checkComboBox = static_cast<CheckBoxList*>(editor);

      itemsStrList = m_item.value.split(";",QString::SkipEmptyParts);
      itemStr = "";
      foreach ( QString itemStr, itemsStrList )
      {
         QStringList itemParts = itemStr.split(",");
         QString itemValue;
         if ( checkComboBox->itemData(idx).toBool() )
         {
            itemValue = "1";
         }
         else
         {
            itemValue = "0";
         }
         itemParts.replace(0,itemValue);
         itemsStrList.replace(idx,itemParts.join(","));
         idx++;
      }
      model->setData(index,itemsStrList.join(";"),Qt::EditRole);
      break;
   }
}

void CPropertyValueDelegate::updateEditorGeometry(QWidget* editor,
      const QStyleOptionViewItem& option, const QModelIndex& /* index */) const
{
   editor->setGeometry(option.rect);
}
