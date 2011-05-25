#include "cdebuggersymboldelegate.h"

//#include <QComboBox>
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
   QStringList symbols = CCC65Interface::getSymbolsForSourceFile("<CPTODO:fixme>");
   QCompleter* completer = new QCompleter(symbols);
   completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
   edit->setCompleter(completer);
}

void CDebuggerSymbolDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
      const QModelIndex& index) const
{
   QLineEdit* edit = static_cast<QLineEdit*>(editor);

   model->setData(index, edit->text(), Qt::EditRole);
}

void CDebuggerSymbolDelegate::updateEditorGeometry(QWidget* editor,
      const QStyleOptionViewItem& option, const QModelIndex& /* index */) const
{
   editor->setGeometry(option.rect);
}
