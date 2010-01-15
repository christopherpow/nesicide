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

   pBreakpoints->GetPrintable(index.row(),data);

   return data;
}

int CBreakpointDisplayModel::rowCount(const QModelIndex &parent) const
{
   CBreakpointInfo* pBreakpoints = CNES::BREAKPOINTS();

   return pBreakpoints->GetNumBreakpoints();
}

void CBreakpointDisplayModel::layoutChangedEvent()
{
   this->layoutChanged();
}
