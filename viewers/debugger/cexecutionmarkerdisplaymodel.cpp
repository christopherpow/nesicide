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
   if ( index.column() > 0 )
   {
      flags |= Qt::ItemIsSelectable;
   }
   return flags;
}

QVariant CExecutionMarkerDisplayModel::data(const QModelIndex& index, int role) const
{
   CMarker* pMarkers = nesGetExecutionMarkerDatabase();
   MarkerSetInfo* pMarker;
   unsigned int addr;

   if ( !index.isValid() )
   {
      return QVariant();
   }

   pMarker = pMarkers->GetMarker(index.row());
   if (role == Qt::BackgroundColorRole)
   {
      if ( index.column() == 0 )
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
      case 0:
         return QVariant();
         break;
      case 1:
         if ( pMarker->state >= eMarkerSet_Complete )
         {
            sprintf(modelStringBuffer,"%d",pMarker->endCpuCycle-pMarker->startCpuCycle);
            return QVariant(modelStringBuffer);
         }
         else
         {
            return QVariant(MARKER_NO_DATA);
         }
         break;
      case 2:
         if ( pMarker->state >= eMarkerSet_Started )
         {
            sprintf(modelStringBuffer,"%02X:%04X(%04X)",
                    nesGetPhysicalPRGROMBank(pMarker->startAbsAddr),
                    pMarker->startAbsAddr&MASK_8KB,
                    0);
            return QVariant(modelStringBuffer);
         }
         else
         {
            return QVariant(MARKER_NOT_STARTED);
         }
         break;
      case 3:
         if ( pMarker->state >= eMarkerSet_Complete )
         {
            sprintf(modelStringBuffer,"%02X:%04X(%04X)",
                    nesGetPhysicalPRGROMBank(pMarker->endAbsAddr),
                    pMarker->endAbsAddr&MASK_8KB,
                    0);
            return QVariant(modelStringBuffer);
         }
         else
         {
            return QVariant(MARKER_NOT_COMPLETED);
         }
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
      case 0:
         return QString("Color");
         break;
      case 1:
         return QString("Cycles");
         break;
      case 2:
         return QString("Start");
         break;
      case 3:
         return QString("End");
         break;
      }
   }
}

int CExecutionMarkerDisplayModel::rowCount(const QModelIndex&) const
{
   return 8;
}

int CExecutionMarkerDisplayModel::columnCount(const QModelIndex&) const
{
   return 4;
}

void CExecutionMarkerDisplayModel::update()
{
   emit layoutChanged();
}
