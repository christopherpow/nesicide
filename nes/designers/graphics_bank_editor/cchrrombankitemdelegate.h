#ifndef CCHRROMBANKITEMDELEGATE_H
#define CCHRROMBANKITEMDELEGATE_H

#include <QStyledItemDelegate>

class CChrRomBankItemDelegate : public QStyledItemDelegate
{
   Q_OBJECT
public:
   explicit CChrRomBankItemDelegate(QObject *parent = 0);

   QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                       const QModelIndex& index) const;

   void setEditorData(QWidget* editor, const QModelIndex& index) const;
   void setModelData(QWidget* editor, QAbstractItemModel* model,
                   const QModelIndex& index) const;
   void updateEditorGeometry(QWidget* editor,
                           const QStyleOptionViewItem& option, const QModelIndex& index) const;
};

#endif // CCHRROMBANKITEMDELEGATE_H
