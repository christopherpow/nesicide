#include "cpropertysymboldelegate.h"

CPropertySymbolDelegate::CPropertySymbolDelegate()
{}

QWidget* CPropertySymbolDelegate::createEditor(QWidget* parent,
      const QStyleOptionViewItem& /* option */,
      const QModelIndex& /*index*/) const
{
   QLineEdit* edit;

   edit = new QLineEdit(parent);
   return edit;
}

void CPropertySymbolDelegate::setEditorData(QWidget* editor,
      const QModelIndex& index) const
{
   QLineEdit* edit;

   edit = static_cast<QLineEdit*>(editor);
   edit->setText(index.model()->data(index, Qt::DisplayRole).toString());
}

void CPropertySymbolDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
      const QModelIndex& index) const
{
   QLineEdit* edit = static_cast<QLineEdit*>(editor);

   model->setData(index, edit->text(), Qt::EditRole);
}

void CPropertySymbolDelegate::updateEditorGeometry(QWidget* editor,
      const QStyleOptionViewItem& option, const QModelIndex& /* index */) const
{
   editor->setGeometry(option.rect);
}
