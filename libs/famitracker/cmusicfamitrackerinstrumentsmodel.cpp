#include "cmusicfamitrackerinstrumentsmodel.h"

#include "stdio.h"

#include <QIcon>
#include <QFont>

static char modelStringBuffer [ 256 ];

CMusicFamiTrackerInstrumentsModel::CMusicFamiTrackerInstrumentsModel(QObject*)
   : m_pDocument(NULL)
{
}

CMusicFamiTrackerInstrumentsModel::~CMusicFamiTrackerInstrumentsModel()
{
}

void CMusicFamiTrackerInstrumentsModel::setDocument(CFamiTrackerDoc* pDoc)
{
   m_pDocument = pDoc;
   emit layoutChanged();
}

QVariant CMusicFamiTrackerInstrumentsModel::data(const QModelIndex& index, int role) const
{
   CInstrument* pInst = (CInstrument*)m_pDocument->GetInstrument((int)index.internalPointer());
   if (!index.isValid())
   {
      return QVariant();
   }

   if (role == Qt::FontRole)
   {
#ifdef Q_WS_MAC
      return QFont("Monaco",9);
#endif
#ifdef Q_WS_X11
      return QFont("Monospace",8);
#endif
#ifdef Q_WS_WIN
      return QFont("Consolas",9);
#endif
   }

   if ( pInst )
   {
      if (role == Qt::DecorationRole)
      {
         switch ( pInst->GetType() )
         {
         case INST_2A03:
            return QIcon(":/resources/Inst_2A03inv.ico");
            break;
         case INST_VRC6:
            return QIcon(":/resources/ico00001.ico");
            break;
         case INST_VRC7:
            return QIcon(":/resources/ico00003.ico");
            break;
         case INST_N163:
            return QIcon(":/resources/ico00004.ico");
            break;
         case INST_FDS:
            return QIcon(":/resources/icon2.ico");
            break;
         case INST_S5B:
            return QIcon(":/resources/ico00005.ico");
            break;
//         case INST_2A07:
//            return QIcon(":/resources/icon1.ico");
//            break;
         }
      }
      
      if (role != Qt::DisplayRole)
      {
         return QVariant();
      }
      
      sprintf(modelStringBuffer,"%02X - %s",(int)index.internalPointer(),pInst->GetName());

      return QVariant(modelStringBuffer);
   }   

   return QVariant();
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
      return createIndex(row,column,m_instrumentMap.value(row));
   }
   
   return QModelIndex();
}

int CMusicFamiTrackerInstrumentsModel::rowCount(const QModelIndex&) const
{
   return m_instrumentMap.count();
}

void CMusicFamiTrackerInstrumentsModel::update()
{
   int idx;
   int instrument = 0;
   m_instrumentMap.clear();
   if ( m_pDocument )
   {
      for ( idx = 0; idx < MAX_INSTRUMENTS; idx++ )
      {
         if ( m_pDocument->IsInstrumentUsed(idx) )
         {
            m_instrumentMap.insert(instrument,idx);
            instrument++;
         }
      }
   }
   emit layoutChanged();
}
