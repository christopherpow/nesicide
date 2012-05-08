#ifndef CDEBUGGERBITFIELDCOMBOBOXDELEGATE_H
#define CDEBUGGERBITFIELDCOMBOBOXDELEGATE_H

#include <QStyledItemDelegate>
#include <QComboBox>
#include <QLineEdit>
#include "cregisterdata.h"

class CDebuggerBitfieldComboBoxDelegate : public QStyledItemDelegate
{
   Q_OBJECT
public:
   CDebuggerBitfieldComboBoxDelegate();

   void setBitfield ( CBitfieldData* pBitfield )
   {
      m_pBitfield = pBitfield;
   }
   QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                         const QModelIndex& index) const;

   void setEditorData(QWidget* editor, const QModelIndex& index) const;
   void setModelData(QWidget* editor, QAbstractItemModel* model,
                     const QModelIndex& index) const;
   void updateEditorGeometry(QWidget* editor,
                             const QStyleOptionViewItem& option, const QModelIndex& index) const;

private:
   CBitfieldData* m_pBitfield;
};

#endif // CDEBUGGERBITFIELDCOMBOBOXDELEGATE_H
