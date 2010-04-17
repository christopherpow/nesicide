#include "csourcebrowserdisplaymodel.h"

#include "cnesrom.h"
#include "cnesicideproject.h"

CSourceBrowserDisplayModel::CSourceBrowserDisplayModel(QObject* parent)
{
}

CSourceBrowserDisplayModel::~CSourceBrowserDisplayModel()
{
}

QVariant CSourceBrowserDisplayModel::data(const QModelIndex &index, int role) const
{
   if (!index.isValid())
      return QVariant();

   if (role != Qt::DisplayRole)
      return QVariant();

   switch ( index.column() )
   {
      case 0:
         return m_source.at ( index.row() );
      break;
   }

   return QVariant();
}

Qt::ItemFlags CSourceBrowserDisplayModel::flags(const QModelIndex &index) const
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

QModelIndex CSourceBrowserDisplayModel::index(int row, int column, const QModelIndex &parent) const
{
   return createIndex(row, column, row);
}

int CSourceBrowserDisplayModel::rowCount(const QModelIndex &parent) const
{
   return m_source.count();
}

int CSourceBrowserDisplayModel::columnCount(const QModelIndex &parent) const
{
   if ( parent.isValid())
   {
      return 0;
   }
   return 1;
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
