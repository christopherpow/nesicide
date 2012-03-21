#include "cdebuggerbitfieldcomboboxdelegate.h"

CDebuggerBitfieldComboBoxDelegate::CDebuggerBitfieldComboBoxDelegate()
   : m_pBitfield(NULL)
{
}

QWidget* CDebuggerBitfieldComboBoxDelegate::createEditor(QWidget* parent,
      const QStyleOptionViewItem& /* option */,
      const QModelIndex& /* index */) const
{
   char data [ 64 ];
   int idx;

   if ( m_pBitfield )
   {
      if ( m_pBitfield->GetNumValues() )
      {
         QComboBox* editor = new QComboBox(parent);

         for ( idx = 0; idx < m_pBitfield->GetNumValues(); idx++ )
         {
            sprintf ( data, "%s", m_pBitfield->GetValueByIndex(idx) );
            editor->addItem ( data, QVariant(idx) );
         }

         editor->setEditable(false);
         return editor;
      }
      else
      {
         QLineEdit* editor = new QLineEdit(parent);
         return editor;
      }
   }
   else
   {
      QLineEdit* editor = new QLineEdit(parent);
      return editor;
   }
}

void CDebuggerBitfieldComboBoxDelegate::setEditorData(QWidget* editor,
      const QModelIndex& index) const
{
   // get register value from model...
   bool ok;
   int value;
   char data [ 8 ];

   // reduce to the bits we care about...
   if ( m_pBitfield )
   {
      value = m_pBitfield->GetValueRaw ( index.model()->data(index, Qt::EditRole).toString().toInt(&ok,16) );

      if ( m_pBitfield->GetNumValues() )
      {
         QComboBox* comboBox = static_cast<QComboBox*>(editor);
         comboBox->setCurrentIndex(value);
         comboBox->showPopup();
      }
      else
      {
         QLineEdit* edit = static_cast<QLineEdit*>(editor);

         sprintf ( data, m_pBitfield->GetDisplayFormat(), value );

         edit->setText(data);
      }
   }
   else
   {
      QLineEdit* edit = static_cast<QLineEdit*>(editor);
      edit->setText(index.model()->data(index, Qt::DisplayRole).toString());
   }
}

void CDebuggerBitfieldComboBoxDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
      const QModelIndex& index) const
{
   // get register value from model...
   bool ok;
   int value;
   char data [ 8 ];

   if ( m_pBitfield )
   {
      if ( m_pBitfield->GetNumValues() )
      {
         QComboBox* comboBox = static_cast<QComboBox*>(editor);

         int bitfieldValue = comboBox->currentIndex();

         value = m_pBitfield->InsertValue ( index.model()->data(index, Qt::EditRole).toString().toInt(&ok,16), bitfieldValue );

         sprintf ( data, m_pBitfield->GetDisplayFormat(), value );

         model->setData(index, data, Qt::EditRole);
      }
      else
      {
         QLineEdit* edit = static_cast<QLineEdit*>(editor);

         value = m_pBitfield->InsertValue ( index.model()->data(index, Qt::EditRole).toString().toInt(&ok,16), edit->text().toInt(&ok,16) );

         sprintf ( data, m_pBitfield->GetDisplayFormat(), value );

         model->setData(index, data, Qt::EditRole);
      }
   }
}

void CDebuggerBitfieldComboBoxDelegate::updateEditorGeometry(QWidget* editor,
      const QStyleOptionViewItem& option, const QModelIndex& /* index */) const
{
   editor->setGeometry(option.rect);
}
