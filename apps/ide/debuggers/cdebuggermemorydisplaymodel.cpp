#include "cdebuggermemorydisplaymodel.h"

#include <QColor>
#include <QBrush>

CDebuggerMemoryDisplayModel::CDebuggerMemoryDisplayModel(memDBFunc memDB,QObject*)
{
   m_modelStringBuffer = new char[256];

   m_memDBFunc = memDB;
   m_memDB = memDB();
}

CDebuggerMemoryDisplayModel::~CDebuggerMemoryDisplayModel()
{
   delete [] m_modelStringBuffer;
}

int CDebuggerMemoryDisplayModel::memoryType() const
{
   if ( m_memDB )
   {
      return m_memDB->GetType();
   }
   return 0;
}

int CDebuggerMemoryDisplayModel::memoryBottom() const
{
   if ( m_memDB )
   {
      return m_memDB->GetBase();
   }
   return 0;
}

int CDebuggerMemoryDisplayModel::memoryTop() const
{
   if ( m_memDB )
   {
      return m_memDB->GetBase()+m_memDB->GetSize()-1;
   }
   return 0;
}

QVariant CDebuggerMemoryDisplayModel::data(const QModelIndex& index, int role) const
{
   if (!index.isValid())
   {
      return QVariant();
   }

   if ( m_memDB )
   {
      if (role == Qt::BackgroundRole)
      {
         return QBrush(QColor(m_memDB->GetCellRedComponent(m_memDB->Get((index.row()*m_memDB->GetNumColumns())+index.column())),
                              m_memDB->GetCellGreenComponent(m_memDB->Get((index.row()*m_memDB->GetNumColumns())+index.column())),
                              m_memDB->GetCellBlueComponent(m_memDB->Get((index.row()*m_memDB->GetNumColumns())+index.column()))));
      }

      if (role == Qt::ForegroundRole)
      {
         QColor col = QColor(m_memDB->GetCellRedComponent(m_memDB->Get((index.row()*m_memDB->GetNumColumns())+index.column())),
                             m_memDB->GetCellGreenComponent(m_memDB->Get((index.row()*m_memDB->GetNumColumns())+index.column())),
                             m_memDB->GetCellBlueComponent(m_memDB->Get((index.row()*m_memDB->GetNumColumns())+index.column())));

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

   if ( m_memDB )
   {
      sprintf(m_modelStringBuffer,"%02X",m_memDB->Get((index.row()*m_memDB->GetNumColumns())+index.column()));
   }

   return QVariant(m_modelStringBuffer);
}

Qt::ItemFlags CDebuggerMemoryDisplayModel::flags(const QModelIndex& /*index*/) const
{
   Qt::ItemFlags flags = Qt::ItemIsEnabled;

   if ( m_memDB )
   {
      if ( m_memDB->GetCellsEditable() )
      {
         flags |= Qt::ItemIsEditable;
      }
      if ( m_memDB->GetCellsSelectable() )
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
      sprintf ( m_modelStringBuffer, "x%1X", section );
   }
   else
   {
      if ( m_memDB )
      {
         m_memDB->GetRowHeading(m_modelStringBuffer,m_memDB->GetBase()+(section*m_memDB->GetNumColumns()));
      }
   }

   return QVariant(m_modelStringBuffer);
}

bool CDebuggerMemoryDisplayModel::setData ( const QModelIndex& index, const QVariant& value, int )
{
   unsigned int data;
   bool ok = false;

   if ( m_memDB )
   {
      data = value.toString().toInt(&ok,16);

      if ( ok )
      {
         m_memDB->Set((index.row()*m_memDB->GetNumColumns())+index.column(),data);
         emit dataChanged(index,index);
      }
   }

   return ok;
}

QModelIndex CDebuggerMemoryDisplayModel::index(int row, int column, const QModelIndex&) const
{
   if ( m_memDB )
   {
      if ( (row >= 0) && (column >= 0) )
      {
         return createIndex(row,column,m_memDB);
      }
   }

   return QModelIndex();
}

int CDebuggerMemoryDisplayModel::rowCount(const QModelIndex&) const
{
   if ( m_memDB )
   {
      return m_memDB->GetNumRows();
   }

   return 0;
}

int CDebuggerMemoryDisplayModel::columnCount(const QModelIndex& /*parent*/) const
{
   if ( m_memDB )
   {
      return m_memDB->GetNumColumns();
   }

   return 0;
}

void CDebuggerMemoryDisplayModel::update()
{
   m_memDB = m_memDBFunc();
   emit dataChanged(QModelIndex(),QModelIndex());
}
