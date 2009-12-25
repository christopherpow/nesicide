#ifndef CDEBUGGEREXECUTIONTRACERMODEL_H
#define CDEBUGGEREXECUTIONTRACERMODEL_H

#include <QAbstractTableModel>
#include "ctracer.h"

class CDebuggerExecutionTracerModel : public QAbstractTableModel
{
public:
    CDebuggerExecutionTracerModel(QObject* parent, CTracer* pTracer);
    virtual ~CDebuggerExecutionTracerModel();
    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    void layoutChangedEvent();
private:
    CTracer *m_pTracer;
};

#endif // CDEBUGGEREXECUTIONTRACERMODEL_H
