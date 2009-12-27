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
   if (role != Qt::DisplayRole)
      return QVariant();
   switch ( section )
   {
      case eTracerCol_Cycle:
         return QString("Cycle");
      break;
      case eTracerCol_Source:
         return QString("Source");
      break;
      case eTracerCol_Type:
         return QString("Type");
      break;
      case eTracerCol_Target:
         return QString("Target");
      break;
      case eTracerCol_Addr:
         return QString("Address");
      break;
      case eTracerCol_Data:
         return QString("Data");
      break;
      case eTracerCol_Info:
         return QString("Info");
      break;
      case eTracerCol_CPU_A:
         return QString("A");
      break;
      case eTracerCol_CPU_X:
         return QString("X");
      break;
      case eTracerCol_CPU_Y:
         return QString("Y");
      break;
      case eTracerCol_CPU_SP:
         return QString("SP");
      break;
      case eTracerCol_CPU_F:
         return QString("Flags");
      break;
      case eTracerCol_CPU_EA:
         return QString("Effective Address");
      break;
   }

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
   if ( parent.isValid())
   {
      return 0;
   }
   return 13;
}

void CDebuggerExecutionTracerModel::layoutChangedEvent()
{
    this->layoutChanged();
}
