#include "cdebuggernumericitemdelegate.h"

CDebuggerNumericItemDelegate::CDebuggerNumericItemDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

QWidget* CDebuggerNumericItemDelegate::createEditor(QWidget* parent,
      const QStyleOptionViewItem& /* option */,
      const QModelIndex& /* index */) const
{
   QLineEdit* editor = new QLineEdit(parent);
   return editor;
}

void CDebuggerNumericItemDelegate::setEditorData(QWidget* editor,
      const QModelIndex& index) const
{
   QLineEdit* edit = static_cast<QLineEdit*>(editor);
   edit->setText(index.model()->data(index, Qt::DisplayRole).toString());
}

void CDebuggerNumericItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
      const QModelIndex& index) const
{
   QLineEdit* edit = static_cast<QLineEdit*>(editor);

   model->setData(index, edit->text(), Qt::EditRole);
}

void CDebuggerNumericItemDelegate::updateEditorGeometry(QWidget* editor,
      const QStyleOptionViewItem& option, const QModelIndex& /* index */) const
{
   editor->setGeometry(option.rect);
}
