#include "cpropertyvaluedelegate.h"

CPropertyValueDelegate::CPropertyValueDelegate()
{}

QWidget* CPropertyValueDelegate::createEditor(QWidget* parent,
      const QStyleOptionViewItem& /* option */,
      const QModelIndex& index) const
{
   QLineEdit* edit;
   QComboBox* comboBox;
   QStringList itemsStrList;

   switch ( m_item.type )
   {
   case propertyInteger:
   case propertyString:
      edit = new QLineEdit(parent);
      return edit;
      break;

   case propertyBoolean:
      comboBox = new QComboBox(parent);
      comboBox->addItem("false");
      comboBox->addItem("true");
      comboBox->setEditable(false);
      return comboBox;
      break;
   case propertyEnumeration:
      comboBox = new QComboBox(parent);

      itemsStrList = m_item.value.split(";",QString::SkipEmptyParts);
      foreach ( QString itemStr, itemsStrList )
      {
         QStringList itemParts = itemStr.split(",");
         QString itemValue;
         itemValue = itemParts.at(1);
         itemValue += "=";
         itemValue += itemParts.at(2);
         comboBox->addItem(itemValue);
      }

      comboBox->setEditable(false);
      return comboBox;
      break;
   }
}

void CPropertyValueDelegate::setEditorData(QWidget* editor,
      const QModelIndex& index) const
{
   QLineEdit* edit;
   QComboBox* comboBox;

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
      comboBox = static_cast<QComboBox*>(editor);
      comboBox->setCurrentIndex(0);
      break;
   }
}

void CPropertyValueDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
      const QModelIndex& index) const
{
   QLineEdit* edit;
   QComboBox* comboBox;

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
      comboBox = static_cast<QComboBox*>(editor);
      // CPTODO: set model data for enumerations.
      break;
   }
}

void CPropertyValueDelegate::updateEditorGeometry(QWidget* editor,
      const QStyleOptionViewItem& option, const QModelIndex& /* index */) const
{
   editor->setGeometry(option.rect);
}
