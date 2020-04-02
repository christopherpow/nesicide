#include <QComboBox>
#include <QCompleter>

#include "cchrrombankitemdelegate.h"

#include "iprojecttreeviewitem.h"
#include "ichrrombankitem.h"

#include "cnesicideproject.h"

CChrRomBankItemDelegate::CChrRomBankItemDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

QWidget* CChrRomBankItemDelegate::createEditor(QWidget* parent,
      const QStyleOptionViewItem& /* option */,
      const QModelIndex& /* index */) const
{
   QComboBox* editor = new QComboBox(parent);

   //CP: I think originally I was going to let people type in objects but
   //    that's just confusing as the drop-down disappears on double click.
   //editor->setEditable(true);

   return editor;
}

void CChrRomBankItemDelegate::setEditorData(QWidget* editor,
      const QModelIndex& index) const
{
   QComboBox* edit = static_cast<QComboBox*>(editor);
   IProjectTreeViewItemIterator iter(CNesicideProject::instance()->getProject());
   QStringList choices;
   choices.append("<click to add>");
   edit->addItem("<click to add>");
   do
   {
      IChrRomBankItem* item = dynamic_cast<IChrRomBankItem*>(iter.current());
      if ( item )
      {
         choices.append(iter.current()->caption());
         edit->addItem(iter.current()->caption());
      }
      iter.next();
   } while ( iter.current() );
   QCompleter* completer = new QCompleter(choices);
   completer->setCompletionMode(QCompleter::PopupCompletion);
   completer->setCompletionPrefix(index.data(Qt::DisplayRole).toString());
   edit->setCompleter(completer);
   edit->showPopup();
   if ( index.data(Qt::DisplayRole).toString() == "<click to add>" )
   {
      edit->setEditText("");
   }
   else
   {
      edit->setEditText(index.data(Qt::DisplayRole).toString());
   }
}

void CChrRomBankItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
      const QModelIndex& index) const
{
   QComboBox* edit = static_cast<QComboBox*>(editor);

   if ( !edit->currentText().isEmpty() )
   {
      model->setData(index, edit->currentText(), Qt::EditRole);
   }
}

void CChrRomBankItemDelegate::updateEditorGeometry(QWidget* editor,
      const QStyleOptionViewItem& option, const QModelIndex& /* index */) const
{
   editor->setGeometry(option.rect);
}
