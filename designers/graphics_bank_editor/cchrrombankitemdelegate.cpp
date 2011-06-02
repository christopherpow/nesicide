#include "cchrrombankitemdelegate.h"

#include <QLineEdit>
#include <QCompleter>

#include "iprojecttreeviewitem.h"

#include "main.h"

CChrRomBankItemDelegate::CChrRomBankItemDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

QWidget* CChrRomBankItemDelegate::createEditor(QWidget* parent,
      const QStyleOptionViewItem& /* option */,
      const QModelIndex& /* index */) const
{
   QLineEdit* editor = new QLineEdit(parent);

   return editor;
}

void CChrRomBankItemDelegate::setEditorData(QWidget* editor,
      const QModelIndex& index) const
{
   QLineEdit* edit = static_cast<QLineEdit*>(editor);
   IProjectTreeViewItemIterator iter(nesicideProject->getProject());
   QStringList choices;
   do
   {
      IChrRomBankItem* item = dynamic_cast<IChrRomBankItem*>(iter.current());
      if ( item )
      {
         choices.append(iter.current()->caption());
      }
      iter.next();
   } while ( iter.current() );
   QCompleter* completer = new QCompleter(choices);
   completer->setCompletionMode(QCompleter::PopupCompletion);
   completer->setCompletionPrefix(index.data(Qt::DisplayRole).toString());
   edit->setCompleter(completer);
   edit->setText(index.data(Qt::DisplayRole).toString());
}

void CChrRomBankItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
      const QModelIndex& index) const
{
   QLineEdit* edit = static_cast<QLineEdit*>(editor);

   if ( !edit->text().isEmpty() )
   {
      model->setData(index, edit->text(), Qt::EditRole);
   }
}

void CChrRomBankItemDelegate::updateEditorGeometry(QWidget* editor,
      const QStyleOptionViewItem& option, const QModelIndex& /* index */) const
{
   editor->setGeometry(option.rect);
}
