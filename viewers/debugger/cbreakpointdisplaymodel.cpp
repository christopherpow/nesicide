#include "cbreakpointdisplaymodel.h"

#include "cnes.h"

#include <QIcon>

CBreakpointDisplayModel::CBreakpointDisplayModel(QObject*)
{
}

CBreakpointDisplayModel::~CBreakpointDisplayModel()
{
}

QVariant CBreakpointDisplayModel::data(const QModelIndex &index, int role) const
{
   char data [ 256 ];
   CBreakpointInfo* pBreakpoints = CNES::BREAKPOINTS();
   BreakpointStatus brkptStatus;

   if ((role == Qt::DecorationRole) && (index.column() == 0))
   {
      brkptStatus = pBreakpoints->GetStatus(index.row());
      if ( brkptStatus == Breakpoint_Idle )
      {
         return QIcon(":/resources/22_breakpoint.png");
      }
      else if ( brkptStatus == Breakpoint_Hit )
      {
         return QIcon(":/resources/22_execution_break.png");
      }
      else if ( brkptStatus == Breakpoint_Disabled )
      {
         return QIcon(":/resources/22_breakpoint_disabled.png");
      }
   }

   if (role != Qt::DisplayRole)
      return QVariant();

   // Get data for columns...
   switch ( index.column() )
   {
      case 0:
         return "";
      break;
      case 1:
         pBreakpoints->GetPrintable(index.row(),data);
      break;
   }

   return data;
}

int CBreakpointDisplayModel::rowCount(const QModelIndex &) const
{
   CBreakpointInfo* pBreakpoints = CNES::BREAKPOINTS();

   return pBreakpoints->GetNumBreakpoints();
}

int CBreakpointDisplayModel::columnCount(const QModelIndex &) const
{
   return 2;
}

void CBreakpointDisplayModel::layoutChangedEvent()
{
   this->layoutChanged();
}
