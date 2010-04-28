#include "cbreakpointdisplaymodel.h"

#include "cnes.h"

CBreakpointDisplayModel::CBreakpointDisplayModel(QObject* parent)
{
}

CBreakpointDisplayModel::~CBreakpointDisplayModel()
{
}

QVariant CBreakpointDisplayModel::data(const QModelIndex &index, int role) const
{
   char data [ 256 ];
   CBreakpointInfo* pBreakpoints = CNES::BREAKPOINTS();

   if (role != Qt::DisplayRole)
      return QVariant();

   switch ( index.column() )
   {
      case 0:
         pBreakpoints->GetStatus(index.row(),data);
      break;
      case 1:
         pBreakpoints->GetPrintable(index.row(),data);
      break;
   }

   return data;
}

int CBreakpointDisplayModel::rowCount(const QModelIndex &parent) const
{
   CBreakpointInfo* pBreakpoints = CNES::BREAKPOINTS();

   return pBreakpoints->GetNumBreakpoints();
}

int CBreakpointDisplayModel::columnCount(const QModelIndex &parent) const
{
   return 2;
}

void CBreakpointDisplayModel::layoutChangedEvent()
{
   this->layoutChanged();
}
