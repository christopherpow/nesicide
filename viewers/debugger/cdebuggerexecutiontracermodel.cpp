#include "cdebuggerexecutiontracermodel.h"

CDebuggerExecutionTracerModel::CDebuggerExecutionTracerModel(QObject* parent)
{
   m_pCPUTracer = C6502::TRACER();
   m_pPPUTracer = CPPU::TRACER();
   m_bShowCPU = true;
   m_bShowPPU = false;
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

   if ( (m_bShowCPU) && (m_bShowPPU) )
   {
      if ( index.row()%4 )
      {
         m_pPPUTracer->GetPrintable(index.row()-((index.row()/4)+1), index.column(), data);
      }
      else
      {
         m_pCPUTracer->GetPrintable(index.row()/4, index.column(), data);
      }
   }
   else if ( m_bShowCPU )
   {
      m_pCPUTracer->GetPrintable(index.row(), index.column(), data);
   }
   else if ( m_bShowPPU )
   {
      m_pPPUTracer->GetPrintable(index.row(), index.column(), data);
   }

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

   return  QVariant();
}

QModelIndex CDebuggerExecutionTracerModel::index(int row, int column, const QModelIndex &parent) const
{
   if ( (m_bShowCPU) && (m_bShowPPU) )
   {
      if ( row%4 )
      {
         return createIndex(row, column, m_pPPUTracer->GetSample(row-((row/4)+1)));
      }
      else
      {
         return createIndex(row, column, m_pCPUTracer->GetSample(row/4));
      }
   }
   else if ( m_bShowCPU )
   {
      return createIndex(row, column, m_pCPUTracer->GetSample(row));
   }
   else if ( m_bShowPPU )
   {
      return createIndex(row, column, m_pPPUTracer->GetSample(row));
   }
   return QModelIndex();
}

int CDebuggerExecutionTracerModel::rowCount(const QModelIndex &parent) const
{
   int rows = 0;
   if ( m_bShowCPU )
   {
      rows += m_pCPUTracer->GetNumSamples();
   }
   if ( m_bShowPPU )
   {
      rows += m_pPPUTracer->GetNumSamples();
   }
   rows--; // otherwise a repeat of the first row shows up...
   return rows;
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
