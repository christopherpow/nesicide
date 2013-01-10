#include "cmusicfamitrackerframesmodel.h"

#include "stdio.h"

static char modelStringBuffer [ 16 ];

CMusicFamiTrackerFramesModel::CMusicFamiTrackerFramesModel(CFamiTrackerDoc* pDoc,QObject*)
{
   m_pDoc = pDoc;
}

CMusicFamiTrackerFramesModel::~CMusicFamiTrackerFramesModel()
{
}

QVariant CMusicFamiTrackerFramesModel::data(const QModelIndex& index, int role) const
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
   sprintf(modelStringBuffer,"00");

   return QVariant(modelStringBuffer);
}

Qt::ItemFlags CMusicFamiTrackerFramesModel::flags(const QModelIndex& /*index*/) const
{
   Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;

   return flags;
}

QVariant CMusicFamiTrackerFramesModel::headerData(int section, Qt::Orientation orientation, int role) const
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
      return QVariant();
   }

   return QVariant(modelStringBuffer);
}

bool CMusicFamiTrackerFramesModel::setData ( const QModelIndex& index, const QVariant& value, int )
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

QModelIndex CMusicFamiTrackerFramesModel::index(int row, int column, const QModelIndex&) const
{
   if ( (row >= 0) && (column >= 0) )
   {
      // CPTODO: return orders row data here when music data is ported.
      return createIndex(row,column,row);
   }

   return QModelIndex();
}

int CMusicFamiTrackerFramesModel::rowCount(const QModelIndex&) const
{
   return m_pDoc->GetFrameCount();
}

int CMusicFamiTrackerFramesModel::columnCount(const QModelIndex& /*parent*/) const
{
   return m_pDoc->GetAvailableChannels();
}

void CMusicFamiTrackerFramesModel::update()
{
   emit layoutChanged();
//   emit dataChanged(QModelIndex(),QModelIndex());
}
