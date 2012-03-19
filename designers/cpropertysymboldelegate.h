#ifndef CPROPERTYSYMBOLDELEGATE_H
#define CPROPERTYSYMBOLDELEGATE_H

#include <QStyledItemDelegate>
#include <QComboBox>
#include <QLineEdit>

#include <cpropertyitem.h>

class CPropertySymbolDelegate : public QStyledItemDelegate
{
   Q_OBJECT
public:
   CPropertySymbolDelegate();

   void setItem(PropertyItem item) { m_item = item; }

   QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                         const QModelIndex& index) const;

   void setEditorData(QWidget* editor, const QModelIndex& index) const;
   void setModelData(QWidget* editor, QAbstractItemModel* model,
                     const QModelIndex& index) const;
   void updateEditorGeometry(QWidget* editor,
                             const QStyleOptionViewItem& option, const QModelIndex& index) const;

private:
   PropertyItem m_item;
};

#endif // CPROPERTYSYMBOLDELEGATE_H
