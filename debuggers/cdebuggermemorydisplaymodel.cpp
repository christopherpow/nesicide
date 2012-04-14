#include "cdebuggermemorydisplaymodel.h"

#include <QColor>
#include <QBrush>

static char modelStringBuffer [ 2048 ];

CDebuggerMemoryDisplayModel::CDebuggerMemoryDisplayModel(memDBFunc memDB,QObject*)
{
   m_memDB = memDB;
}

CDebuggerMemoryDisplayModel::~CDebuggerMemoryDisplayModel()
{
}

int CDebuggerMemoryDisplayModel::memoryType() const
{
   CMemoryDatabase* memDB = m_memDB();

   if ( memDB )
   {
      return memDB->GetType();
   }
   return 0;
}

int CDebuggerMemoryDisplayModel::memoryBottom() const
{
   CMemoryDatabase* memDB = m_memDB();

   if ( memDB )
   {
      return memDB->GetBase();
   }
   return 0;
}

int CDebuggerMemoryDisplayModel::memoryTop() const
{
   CMemoryDatabase* memDB = m_memDB();

   if ( memDB )
   {
      return memDB->GetBase()+memDB->GetSize();
   }
   return 0;
}

QVariant CDebuggerMemoryDisplayModel::data(const QModelIndex& index, int role) const
{
   CMemoryDatabase* memDB = m_memDB();

   if (!index.isValid())
   {
      return QVariant();
   }

   if ( memDB )
   {
      if (role == Qt::BackgroundRole)
      {
         return QBrush(QColor(memDB->GetCellRedComponent(memDB->Get((index.row()*memDB->GetNumColumns())+index.column())),
                              memDB->GetCellGreenComponent(memDB->Get((index.row()*memDB->GetNumColumns())+index.column())),
                              memDB->GetCellBlueComponent(memDB->Get((index.row()*memDB->GetNumColumns())+index.column()))));
      }

      if (role == Qt::ForegroundRole)
      {
         QColor col = QColor(memDB->GetCellRedComponent(memDB->Get((index.row()*memDB->GetNumColumns())+index.column())),
                             memDB->GetCellGreenComponent(memDB->Get((index.row()*memDB->GetNumColumns())+index.column())),
                             memDB->GetCellBlueComponent(memDB->Get((index.row()*memDB->GetNumColumns())+index.column())));

         if ((((double)col.red() +
               (double)col.green() +
               (double)col.blue()) / (double)3) < 200)
         {
            return QBrush(QColor(255, 255, 255));
         }
         else
         {
            return QBrush(QColor(0, 0, 0));
         }
      }
   }

   if (role != Qt::DisplayRole)
   {
      return QVariant();
   }

   if ( memDB )
   {
      sprintf(modelStringBuffer,"%02X",memDB->Get((index.row()*memDB->GetNumColumns())+index.column()));
   }

   return QVariant(modelStringBuffer);
}

Qt::ItemFlags CDebuggerMemoryDisplayModel::flags(const QModelIndex& index) const
{
   Qt::ItemFlags flags = Qt::ItemIsEnabled;

   if ( m_memDB() )
   {
      if ( m_memDB()->GetCellsEditable() )
      {
         flags |= Qt::ItemIsEditable;
      }
      if ( m_memDB()->GetCellsSelectable() )
      {
         flags |= Qt::ItemIsSelectable;
      }
   }

   return flags;
}

QVariant CDebuggerMemoryDisplayModel::headerData(int section, Qt::Orientation orientation, int role) const
{
   if (role != Qt::DisplayRole)
   {
      return QVariant();
   }

   if ( orientation == Qt::Horizontal )
   {
      sprintf ( modelStringBuffer, "x%1X", section );
   }
   else
   {
      if ( m_memDB() )
      {
         m_memDB()->GetRowHeading(modelStringBuffer,m_memDB()->GetBase()+(section*m_memDB()->GetNumColumns()));
      }
   }

   return QVariant(modelStringBuffer);
}

bool CDebuggerMemoryDisplayModel::setData ( const QModelIndex& index, const QVariant& value, int )
{
   unsigned int data;
   bool ok = false;

   if ( m_memDB() )
   {
      data = value.toString().toInt(&ok,16);

      if ( ok )
      {
         m_memDB()->Set((index.row()*m_memDB()->GetNumColumns())+index.column(),data);
         emit dataChanged(index,index);
      }
   }

   return ok;
}

QModelIndex CDebuggerMemoryDisplayModel::index(int row, int column, const QModelIndex&) const
{
   if ( m_memDB() )
   {
      if ( (row >= 0) && (column >= 0) )
      {
         return createIndex(row,column,m_memDB());
      }
   }

   return QModelIndex();
}

int CDebuggerMemoryDisplayModel::rowCount(const QModelIndex&) const
{
   if ( m_memDB() )
   {
      return m_memDB()->GetNumRows();
   }

   return 0;
}

int CDebuggerMemoryDisplayModel::columnCount(const QModelIndex& parent) const
{
   if ( m_memDB() )
   {
      return m_memDB()->GetNumColumns();
   }

   return 0;
}

void CDebuggerMemoryDisplayModel::update()
{
   emit dataChanged(QModelIndex(),QModelIndex());
}
