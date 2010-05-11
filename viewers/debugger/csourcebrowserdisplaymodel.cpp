#include "csourcebrowserdisplaymodel.h"

#include "cnes.h"
#include "cnesrom.h"
#include "cnes6502.h"
#include "cnesicideproject.h"

#include "cbreakpointinfo.h"

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

   if (!index.isValid())
      return QVariant();

   if ((role == Qt::DecorationRole) && (index.column() == 0))
   {
      return QIcon(":/resources/22_execution_pointer.png");
   }

   if (role != Qt::DisplayRole)
      return QVariant();

   switch ( index.column() )
   {
      case 0:
         return "";
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
