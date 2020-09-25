#ifndef CDEBUGGEREXECUTIONTRACERMODEL_H
#define CDEBUGGEREXECUTIONTRACERMODEL_H

#include <QAbstractTableModel>
#include "ctracer.h"

class CDebuggerExecutionTracerModel : public QAbstractTableModel
{
   Q_OBJECT
public:
   CDebuggerExecutionTracerModel(QObject* parent);
   virtual ~CDebuggerExecutionTracerModel();
   QVariant data(const QModelIndex& index, int role) const;
   Qt::ItemFlags flags(const QModelIndex& index) const;
   QVariant headerData(int section, Qt::Orientation orientation,
                       int role = Qt::DisplayRole) const;
   QModelIndex index(int row, int column,
                     const QModelIndex& parent = QModelIndex()) const;
   int rowCount(const QModelIndex& parent = QModelIndex()) const;
   int columnCount(const QModelIndex& parent = QModelIndex()) const;
   void showCPU ( bool show );
   void showPPU ( bool show );
   
public slots:
   void update();

private:
   CTracer *m_pTracer;
   bool     m_bShowCPU;
   bool     m_bShowPPU;
   char    *m_modelStringBuffer;
};

#endif // CDEBUGGEREXECUTIONTRACERMODEL_H
