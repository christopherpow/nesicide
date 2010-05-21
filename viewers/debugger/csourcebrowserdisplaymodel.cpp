#include "csourcebrowserdisplaymodel.h"

#include "cnes.h"
#include "cnesrom.h"
#include "cnes6502.h"
#include "cnesicideproject.h"

#include "cbreakpointinfo.h"

#include "pasm_lib.h"

#include <QIcon>

CSourceBrowserDisplayModel::CSourceBrowserDisplayModel(QObject*)
{
}

CSourceBrowserDisplayModel::~CSourceBrowserDisplayModel()
{
}

QVariant CSourceBrowserDisplayModel::data(const QModelIndex &index, int role) const
{
   CBreakpointInfo* pBreakpoints = CNES::BREAKPOINTS();
   int idx;
   unsigned int addr;
   unsigned int absAddr;
   CMarker& markers = C6502::MARKERS();
   MarkerSetInfo* pMarker;

   if (!index.isValid())
      return QVariant();

   addr = pasm_get_source_addr_from_linenum(index.row()+1);

   if ( (role == Qt::BackgroundRole) && (index.column() == 0) )
   {
      absAddr = CROM::ABSADDR(addr);
      for ( idx = 0; idx < markers.GetNumMarkers(); idx++ )
      {
         pMarker = markers.GetMarker(idx);
         if ( (pMarker->state == eMarkerSet_Started) ||
              (pMarker->state == eMarkerSet_Complete) )
         {
            if ( (absAddr >= pMarker->startAbsAddr) &&
                 (absAddr <= pMarker->endAbsAddr) )
            {
               return QColor(pMarker->red,pMarker->green,pMarker->blue);
            }
         }
      }
      return QVariant();
   }

   if ((role == Qt::DecorationRole) && (index.column() == 0))
   {
      for ( idx = 0; idx < pBreakpoints->GetNumBreakpoints(); idx++ )
      {
         BreakpointInfo* pBreakpoint = pBreakpoints->GetBreakpoint(idx);
         if ( (pBreakpoint->enabled) &&
              (pBreakpoint->type == eBreakOnCPUExecution) &&
              ((UINT)pBreakpoint->item1 <= addr) &&
              ((UINT)pBreakpoint->item2 >= addr) )
         {
            if ( addr == C6502::__PC() )
            {
               return QIcon(":/resources/22_execution_break.png");
            }
            else
            {
               return QIcon(":/resources/22_breakpoint.png");
            }
         }
         else if ( (!pBreakpoint->enabled) &&
                   (pBreakpoint->type == eBreakOnCPUExecution) &&
                   ((UINT)pBreakpoint->item1 <= addr) &&
                   ((UINT)pBreakpoint->item2 >= addr) )
         {
            if ( addr == C6502::__PC() )
            {
               return QIcon(":/resources/22_execution_break_disabled.png");
            }
            else
            {
               return QIcon(":/resources/22_breakpoint_disabled.png");
            }
         }
      }
      if ( addr == C6502::__PC() )
      {
         return QIcon(":/resources/22_execution_pointer.png");
      }
   }

   if (role != Qt::DisplayRole)
      return QVariant();

   switch ( index.column() )
   {
      case 0:
         return QVariant();
      break;
      case 1:
         return m_source.at ( index.row() );
      break;
   }

   return QVariant();
}

Qt::ItemFlags CSourceBrowserDisplayModel::flags(const QModelIndex &) const
{
    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    return flags;
}

QVariant CSourceBrowserDisplayModel::headerData(int section, Qt::Orientation orientation, int role) const
{
   char buffer [ 16 ];

   if (role != Qt::DisplayRole)
      return QVariant();

   if ( orientation == Qt::Horizontal )
   {
      switch ( section )
      {
         case 0:
            return "!";
         break;
         case 1:
            return "Source Code";
         break;
      }
   }
   else
   {
      sprintf ( buffer, "%d", section+1 );
      return buffer;
   }

   return  QVariant();
}

QModelIndex CSourceBrowserDisplayModel::index(int row, int column, const QModelIndex &) const
{
   return createIndex(row, column, row);
}

int CSourceBrowserDisplayModel::rowCount(const QModelIndex &) const
{
   return m_source.count();
}

int CSourceBrowserDisplayModel::columnCount(const QModelIndex &parent) const
{
   if ( parent.isValid())
   {
      return 0;
   }
   return 2;
}

void CSourceBrowserDisplayModel::layoutChangedEvent()
{
   CSourceItem *sourceItem;

   if ( nesicideProject )
   {
      m_source.empty ();
      for (int sourceIndex = 0; sourceIndex < nesicideProject->getProject()->getSources()->childCount(); sourceIndex++)
      {
         sourceItem = (CSourceItem *)nesicideProject->getProject()->getSources()->child(sourceIndex);
         m_source = sourceItem->get_sourceCode().split ( QRegExp("[\r\n]") );
      }
   }

   this->layoutChanged();
}
