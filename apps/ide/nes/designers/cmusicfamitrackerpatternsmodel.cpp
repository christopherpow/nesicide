#include "cmusicfamitrackerpatternsmodel.h"

#include "stdio.h"

#include "famitracker/TrackerChannel.h"

static char modelStringBuffer [ 16 ];

CMusicFamiTrackerPatternsModel::CMusicFamiTrackerPatternsModel(CFamiTrackerDoc* pDoc,QObject*)
{
   m_pDoc = pDoc;
}

CMusicFamiTrackerPatternsModel::~CMusicFamiTrackerPatternsModel()
{
}

QVariant CMusicFamiTrackerPatternsModel::data(const QModelIndex& index, int role) const
{
   if (!index.isValid())
   {
      return QVariant();
   }

   if (role != Qt::DisplayRole)
   {
      return QVariant();
   }

   // CPTODO: return order data when music data is ported.
   sprintf(modelStringBuffer,"--- -- - ---");

   return QVariant(modelStringBuffer);
}

Qt::ItemFlags CMusicFamiTrackerPatternsModel::flags(const QModelIndex& /*index*/) const
{
   Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

   return flags;
}

QVariant CMusicFamiTrackerPatternsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
   if (role != Qt::DisplayRole)
   {
      return QVariant();
   }

   if ( orientation == Qt::Vertical )
   {
      sprintf ( modelStringBuffer, "%02X", section );
   }
   else
   {
      return QVariant((char*)m_pDoc->GetChannel(section)->GetChannelName());
   }

   return QVariant(modelStringBuffer);
}

bool CMusicFamiTrackerPatternsModel::setData ( const QModelIndex& index, const QVariant& value, int )
{
   unsigned int data;
   bool ok = false;

   data = value.toString().toInt(&ok,16);

   if ( ok )
   {
      // CPTODO: set order when music data is ported.
      emit dataChanged(index,index);
   }

   return ok;
}

QModelIndex CMusicFamiTrackerPatternsModel::index(int row, int column, const QModelIndex&) const
{
   if ( (row >= 0) && (column >= 0) )
   {
      // CPTODO: return orders row data here when music data is ported.
      return createIndex(row,column,row);
   }

   return QModelIndex();
}

int CMusicFamiTrackerPatternsModel::rowCount(const QModelIndex&) const
{
   return m_pDoc->GetPatternLength();
}

int CMusicFamiTrackerPatternsModel::columnCount(const QModelIndex& /*parent*/) const
{
   return m_pDoc->GetAvailableChannels();
}

void CMusicFamiTrackerPatternsModel::update()
{
   emit layoutChanged();
//   emit dataChanged(QModelIndex(),QModelIndex());
}
