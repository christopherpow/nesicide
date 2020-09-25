#include "cbreakpointdisplaymodel.h"

#include "dbg_cnes.h"

#include "cobjectregistry.h"

#include <QIcon>
#include <QMimeData>

static const char* CLICK_TO_ADD_OR_EDIT = "<click to add or edit>";

CBreakpointDisplayModel::CBreakpointDisplayModel(CBreakpointInfo* pBreakpoints,QObject*)
{
   m_pBreakpoints = pBreakpoints;
   m_modelStringBuffer = new char[256];
}

CBreakpointDisplayModel::~CBreakpointDisplayModel()
{
   delete [] m_modelStringBuffer;
}

Qt::ItemFlags CBreakpointDisplayModel::flags(const QModelIndex& /*index*/) const
{
   Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

   return flags;
}

QVariant CBreakpointDisplayModel::data(const QModelIndex& index, int role) const
{
   BreakpointStatus brkptStatus;

   if ( index.row() < m_pBreakpoints->GetNumBreakpoints() )
   {
      if ((role == Qt::DecorationRole) && (index.column() == 0))
      {
         brkptStatus = m_pBreakpoints->GetStatus(index.row());

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
         m_pBreakpoints->GetPrintable(index.row(),m_modelStringBuffer);
         return QVariant(m_modelStringBuffer);
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
   if ( m_pBreakpoints )
   {
      return m_pBreakpoints->GetNumBreakpoints()+1;
   }
   return 0;
}

int CBreakpointDisplayModel::columnCount(const QModelIndex&) const
{
   return 2;
}

void CBreakpointDisplayModel::update()
{
   emit layoutChanged();
}
