#include "cdebuggersymboldelegate.h"

#include <QLineEdit>
#include <QCompleter>

#include "ccc65interface.h"

CDebuggerSymbolDelegate::CDebuggerSymbolDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

QWidget* CDebuggerSymbolDelegate::createEditor(QWidget* parent,
      const QStyleOptionViewItem& /* option */,
      const QModelIndex& /* index */) const
{
   QLineEdit* editor = new QLineEdit(parent);

   return editor;
}

void CDebuggerSymbolDelegate::setEditorData(QWidget* editor,
      const QModelIndex& index) const
{
   QLineEdit* edit = static_cast<QLineEdit*>(editor);
   QStringList symbols = CCC65Interface::instance()->getSymbolsForSourceFile("<CPTODO:fixme>");
   QCompleter* completer = new QCompleter(symbols);
   completer->setCompletionMode(QCompleter::PopupCompletion);
   completer->setCompletionPrefix(index.data(Qt::DisplayRole).toString());
   edit->setCompleter(completer);
   if ( index.data(Qt::DisplayRole).toString() == "<click to add or edit>" )
   {
      edit->setText("");
   }
   else
   {
      edit->setText(index.data(Qt::DisplayRole).toString());
   }
}

void CDebuggerSymbolDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
      const QModelIndex& index) const
{
   QLineEdit* edit = static_cast<QLineEdit*>(editor);

   if ( !edit->text().isEmpty() )
   {
      model->setData(index, edit->text(), Qt::EditRole);
   }
}

void CDebuggerSymbolDelegate::updateEditorGeometry(QWidget* editor,
      const QStyleOptionViewItem& option, const QModelIndex& /* index */) const
{
   editor->setGeometry(option.rect);
}
