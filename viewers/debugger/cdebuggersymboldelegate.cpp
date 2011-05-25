#include "cdebuggersymboldelegate.h"

#include "ccc65interface.h"

CDebuggerSymbolDelegate::CDebuggerSymbolDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

QWidget* CDebuggerSymbolDelegate::createEditor(QWidget* parent,
      const QStyleOptionViewItem& /* option */,
      const QModelIndex& /* index */) const
{
   QComboBox* editor = new QComboBox(parent);
   editor->setEditable(true);
   return editor;
}

void CDebuggerSymbolDelegate::setEditorData(QWidget* editor,
      const QModelIndex& index) const
{
   QComboBox* edit = static_cast<QComboBox*>(editor);
   QStringList symbols = CCC65Interface::getSymbolsForSourceFile("<CPTODO:fixme>");

   edit->addItems(symbols);
}

void CDebuggerSymbolDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
      const QModelIndex& index) const
{
   QComboBox* edit = static_cast<QComboBox*>(editor);

   model->setData(index, edit->currentText(), Qt::EditRole);
}

void CDebuggerSymbolDelegate::updateEditorGeometry(QWidget* editor,
      const QStyleOptionViewItem& option, const QModelIndex& /* index */) const
{
   editor->setGeometry(option.rect);
}
