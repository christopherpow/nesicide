#ifndef CDEBUGGEREXECUTIONTRACERMODEL_H
#define CDEBUGGEREXECUTIONTRACERMODEL_H

#include <QAbstractTableModel>
#include "ctracer.h"
#include "cnes6502.h"
#include "cnesppu.h"

class CDebuggerExecutionTracerModel : public QAbstractTableModel
{
   Q_OBJECT
public:
    CDebuggerExecutionTracerModel(QObject* parent);
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
    void showCPU ( bool show ) { m_bShowCPU = show; }
    void showPPU ( bool show ) { m_bShowPPU = show; }
private:
    CTracer *m_pCPUTracer;
    CTracer *m_pPPUTracer;
    bool    m_bShowCPU;
    bool    m_bShowPPU;
};

#endif // CDEBUGGEREXECUTIONTRACERMODEL_H
