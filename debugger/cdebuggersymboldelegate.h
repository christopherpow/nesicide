#ifndef CDEBUGGERSYMBOLDELEGATE_H
#define CDEBUGGERSYMBOLDELEGATE_H

#include <QStyledItemDelegate>

class CDebuggerSymbolDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit CDebuggerSymbolDelegate(QObject *parent = 0);

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                          const QModelIndex& index) const;

    void setEditorData(QWidget* editor, const QModelIndex& index) const;
    void setModelData(QWidget* editor, QAbstractItemModel* model,
                      const QModelIndex& index) const;
    void updateEditorGeometry(QWidget* editor,
                              const QStyleOptionViewItem& option, const QModelIndex& index) const;
};

#endif // CDEBUGGERSYMBOLDELEGATE_H
