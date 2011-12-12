#include "cexecutionmarkerdisplaymodel.h"

#include <QColor>

#include "cmarker.h"
#include "emulator_core.h"

static char modelStringBuffer [ 2048 ];

static const char* MARKER_NOT_STARTED = "No start set";
static const char* MARKER_NOT_COMPLETED = "No end set";
static const char* MARKER_NO_DATA = "No data";

CExecutionMarkerDisplayModel::CExecutionMarkerDisplayModel(QObject *parent) :
    QAbstractTableModel(parent)
{
}

CExecutionMarkerDisplayModel::~CExecutionMarkerDisplayModel()
{
}

Qt::ItemFlags CExecutionMarkerDisplayModel::flags(const QModelIndex& index) const
{
   Qt::ItemFlags flags = Qt::ItemIsEnabled;
   if ( index.column() > ExecutionVisualizerCol_Color )
   {
      flags |= Qt::ItemIsSelectable;
   }
   return flags;
}

QVariant CExecutionMarkerDisplayModel::data(const QModelIndex& index, int role) const
{
   CMarker* pMarkers = nesGetExecutionMarkerDatabase();
   MarkerSetInfo* pMarker;

   if ( !index.isValid() )
   {
      return QVariant();
   }

   pMarker = pMarkers->GetMarker(index.row());
   if (role == Qt::BackgroundColorRole)
   {
      if ( index.column() == ExecutionVisualizerCol_Color )
      {
         return QColor(pMarker->red,pMarker->green,pMarker->blue);
      }
   }
   if (role != Qt::DisplayRole)
   {
      return QVariant();
   }

   // Get data for columns...
   switch ( index.column() )
   {
      case ExecutionVisualizerCol_Color:
         return QVariant();
         break;
      case ExecutionVisualizerCol_Cycles:
         if ( pMarker->state >= eMarkerSet_Complete )
         {
            if ( pMarker->endCpuCycle == MARKER_NOT_MARKED )
            {
               sprintf(modelStringBuffer,"%d",nesGetCPUCycle()-pMarker->startCpuCycle);
            }
            else
            {
               sprintf(modelStringBuffer,"%d",pMarker->endCpuCycle-pMarker->startCpuCycle);
            }
            return QVariant(modelStringBuffer);
         }
         else
         {
            return QVariant(MARKER_NO_DATA);
         }
         break;
      case ExecutionVisualizerCol_StartAddr:
         if ( pMarker->state >= eMarkerSet_Started )
         {
            nesGetPrintableAddressWithAbsolute(modelStringBuffer,
                                               pMarker->startAddr,
                                               pMarker->startAbsAddr);
            return QVariant(modelStringBuffer);
         }
         else
         {
            return QVariant(MARKER_NOT_STARTED);
         }
         break;
      case ExecutionVisualizerCol_EndAddr:
         if ( pMarker->state >= eMarkerSet_Complete )
         {
            nesGetPrintableAddressWithAbsolute(modelStringBuffer,
                                               pMarker->endAddr,
                                               pMarker->endAbsAddr);
            return QVariant(modelStringBuffer);
         }
         else
         {
            return QVariant(MARKER_NOT_COMPLETED);
         }
         break;
      case ExecutionVisualizerCol_Status:
         if ( pMarker->endCpuCycle == MARKER_NOT_MARKED )
         {
            sprintf(modelStringBuffer,"IN PROGRESS");
         }
         else
         {
            sprintf(modelStringBuffer,"COMPLETE");
         }
         return QVariant(modelStringBuffer);
         break;
   }
   return QVariant();
}

QVariant CExecutionMarkerDisplayModel::headerData(int section, Qt::Orientation orientation, int role) const
{
   if (role != Qt::DisplayRole)
   {
      return QVariant();
   }

   if ( orientation == Qt::Horizontal )
   {
      switch ( section )
      {
      case ExecutionVisualizerCol_Color:
         return QString("Color");
         break;
      case ExecutionVisualizerCol_Cycles:
         return QString("CPU Cycles");
         break;
      case ExecutionVisualizerCol_StartAddr:
         return QString("Start");
         break;
      case ExecutionVisualizerCol_EndAddr:
         return QString("End");
         break;
      case ExecutionVisualizerCol_Status:
         return QString("Status");
         break;
      }
   }
   return QVariant();
}

int CExecutionMarkerDisplayModel::rowCount(const QModelIndex&) const
{
   CMarker* pMarkers = nesGetExecutionMarkerDatabase();
   return pMarkers->GetNumMarkers();
}

int CExecutionMarkerDisplayModel::columnCount(const QModelIndex&) const
{
   return ExecutionVisualizerCol_MAX;
}

void CExecutionMarkerDisplayModel::update()
{
   emit layoutChanged();
}
