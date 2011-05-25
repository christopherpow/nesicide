#include "cdebuggersymboldelegate.h"

#include "ccc65interface.h"

CDebuggerSymbolDelegate::CDebuggerSymbolDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}
#include <QCompleter>
QWidget* CDebuggerSymbolDelegate::createEditor(QWidget* parent,
      const QStyleOptionViewItem& /* option */,
      const QModelIndex& /* index */) const
{
   QComboBox* editor = new QComboBox(parent);
   editor->setEditable(true);
   editor->completer()->setCompletionMode(QCompleter::UnfilteredPopupCompletion);

   return editor;
}

void CDebuggerSymbolDelegate::setEditorData(QWidget* editor,
      const QModelIndex& index) const
{
   QComboBox* edit = static_cast<QComboBox*>(editor);
   QStringList symbols = CCC65Interface::getSymbolsForSourceFile("<CPTODO:fixme>");

   edit->addItems(symbols);
   edit->completer()->setCompletionPrefix(edit->currentText());
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
