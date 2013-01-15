#include "cmusicfamitrackerinstrumentsmodel.h"

#include "stdio.h"

static char modelStringBuffer [ 16 ];

CMusicFamiTrackerInstrumentsModel::CMusicFamiTrackerInstrumentsModel(CFamiTrackerDoc* pDoc,QObject*)
{
   m_pDocument = pDoc;
}

CMusicFamiTrackerInstrumentsModel::~CMusicFamiTrackerInstrumentsModel()
{
}

#include <QIcon>
QVariant CMusicFamiTrackerInstrumentsModel::data(const QModelIndex& index, int role) const
{
   if (!index.isValid())
   {
      return QVariant();
   }

   if (role == Qt::DecorationRole)
   {
      return QIcon(":/resources/Inst_2A03inv.ico");
   }
   if (role != Qt::DisplayRole)
   {
      return QVariant();
   }
   return QVariant(m_pDocument->GetInstrument(index.row())->GetName());
}

Qt::ItemFlags CMusicFamiTrackerInstrumentsModel::flags(const QModelIndex& /*index*/) const
{
   Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

   return flags;
}

QModelIndex CMusicFamiTrackerInstrumentsModel::index(int row, int column, const QModelIndex&) const
{
   if ( (row >= 0) && (column >= 0) )
   {
      // CPTODO: return instrument data here when music data is ported.
      return createIndex(row,column,row);
   }

   return QModelIndex();
}

int CMusicFamiTrackerInstrumentsModel::rowCount(const QModelIndex&) const
{
   return m_pDocument->GetInstrumentCount();
}

void CMusicFamiTrackerInstrumentsModel::update()
{
   emit layoutChanged();
}
