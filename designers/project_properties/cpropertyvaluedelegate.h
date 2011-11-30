#ifndef CPROPERTYVALUEDELEGATE_H
#define CPROPERTYVALUEDELEGATE_H

#include <QStyledItemDelegate>
#include <QComboBox>
#include <QLineEdit>

#include <cpropertyitem.h>

class CPropertyValueDelegate : public QStyledItemDelegate
{
   Q_OBJECT
public:
   CPropertyValueDelegate();

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

#endif // CPROPERTYVALUEDELEGATE_H
