#ifndef CDEBUGGERREGISTERCOMBOBOXDELEGATE_H
#define CDEBUGGERREGISTERCOMBOBOXDELEGATE_H

#include <QStyledItemDelegate>
#include <QComboBox>
#include <QLineEdit>
#include "cregisterdata.h"

class CDebuggerRegisterComboBoxDelegate : public QStyledItemDelegate {
   Q_OBJECT
public:
   CDebuggerRegisterComboBoxDelegate();

   void setBitfield ( CBitfieldData* pBitfield ) { m_pBitfield = pBitfield; }
   QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                       const QModelIndex &index) const;

   void setEditorData(QWidget *editor, const QModelIndex &index) const;
   void setModelData(QWidget *editor, QAbstractItemModel *model,
                   const QModelIndex &index) const;
   void updateEditorGeometry(QWidget *editor,
     const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
   CBitfieldData* m_pBitfield;
};

#endif // CDEBUGGERREGISTERCOMBOBOXDELEGATE_H
