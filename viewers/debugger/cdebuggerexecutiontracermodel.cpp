#include "cdebuggerexecutiontracermodel.h"

CDebuggerExecutionTracerModel::CDebuggerExecutionTracerModel(QObject* parent, CTracer* pTracer)
   : m_pTracer(pTracer)
{
}

CDebuggerExecutionTracerModel::~CDebuggerExecutionTracerModel()
{
}

QVariant CDebuggerExecutionTracerModel::data(const QModelIndex &index, int role) const
{
   char data [ 64 ];

   if (!index.isValid())
      return QVariant();

   if (role != Qt::DisplayRole)
      return QVariant();

   TracerInfo* pSample = static_cast<TracerInfo*>(index.internalPointer());

   m_pTracer->GetPrintable(index.row(), index.column(), data);
   return data;
}

Qt::ItemFlags CDebuggerExecutionTracerModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    return flags;
}

QVariant CDebuggerExecutionTracerModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return  QString("Blah");
}

QModelIndex CDebuggerExecutionTracerModel::index(int row, int column, const QModelIndex &parent) const
{
   if (!m_pTracer)
      return QModelIndex();
   else
      return createIndex(row, column, m_pTracer->GetSample(row));
}

int CDebuggerExecutionTracerModel::rowCount(const QModelIndex &parent) const
{
   return m_pTracer->GetNumSamples();
}

int CDebuggerExecutionTracerModel::columnCount(const QModelIndex &parent) const
{
    return 13;
}

void CDebuggerExecutionTracerModel::layoutChangedEvent()
{
    this->layoutChanged();
}
