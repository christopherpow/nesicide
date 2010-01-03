#include "cdebuggerregistercomboboxdelegate.h"

CDebuggerRegisterComboBoxDelegate::CDebuggerRegisterComboBoxDelegate()
   : m_pBitfield(NULL)
{}

QWidget *CDebuggerRegisterComboBoxDelegate::createEditor(QWidget *parent,
     const QStyleOptionViewItem &/* option */,
     const QModelIndex &/* index */) const
{
   QComboBox *editor = new QComboBox(parent);
   int idx;

   for ( idx = 0; idx < m_pBitfield->GetNumValues(); idx++ )
   {
      editor->addItem ( m_pBitfield->GetValue(idx), QVariant(m_pBitfield->GetValueRaw(idx)) );
   }
   return editor;
}

void CDebuggerRegisterComboBoxDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const
{
   // get register value from model...
   int value = index.model()->data(index, Qt::EditRole).toInt();

   // reduce to the bits we care about...
   value = m_pBitfield->GetValueRaw ( value );

   QComboBox *comboBox = static_cast<QComboBox*>(editor);
   comboBox->setCurrentIndex(value);
}

void CDebuggerRegisterComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
   QComboBox *comboBox = static_cast<QComboBox*>(editor);

   // get register value from model...
   int value = index.model()->data(index, Qt::EditRole).toInt();

   int bitfieldValue = comboBox->currentIndex();

   m_pBitfield->InsertValue ( value, bitfieldValue );

   model->setData(index, value, Qt::EditRole);
}

void CDebuggerRegisterComboBoxDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}
