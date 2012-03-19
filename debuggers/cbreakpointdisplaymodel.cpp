#include "cbreakpointdisplaymodel.h"

#include "dbg_cnes.h"

#include <QIcon>
#include <QMimeData>

static char modelStringBuffer [ 2048 ];

static const char* CLICK_TO_ADD_OR_EDIT = "<click to add or edit>";

CBreakpointDisplayModel::CBreakpointDisplayModel(QObject*)
{
}

CBreakpointDisplayModel::~CBreakpointDisplayModel()
{
}

Qt::ItemFlags CBreakpointDisplayModel::flags(const QModelIndex& index) const
{
   Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

   return flags;
}

QVariant CBreakpointDisplayModel::data(const QModelIndex& index, int role) const
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   BreakpointStatus brkptStatus;

   if ( index.row() < pBreakpoints->GetNumBreakpoints() )
   {
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
            pBreakpoints->GetPrintable(index.row(),modelStringBuffer);
            return QVariant(modelStringBuffer);
            break;
      }
   }
   else
   {
      if (role != Qt::DisplayRole)
      {
         return QVariant();
      }

      // Get data for columns...
      switch ( index.column() )
      {
         case 1:
            return QVariant(CLICK_TO_ADD_OR_EDIT);
            break;
      }
   }
   return QVariant();
}

int CBreakpointDisplayModel::rowCount(const QModelIndex&) const
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();

   return pBreakpoints->GetNumBreakpoints()+1;
}

int CBreakpointDisplayModel::columnCount(const QModelIndex&) const
{
   return 2;
}

void CBreakpointDisplayModel::update()
{
   emit layoutChanged();
}
