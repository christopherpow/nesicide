#include "cdebuggerregisterdisplaymodel.h"

static char modelStringBuffer [ 2048 ];

CDebuggerRegisterDisplayModel::CDebuggerRegisterDisplayModel(regDBFunc regDB,QObject*)
{
   m_regDB = regDB;
}

CDebuggerRegisterDisplayModel::~CDebuggerRegisterDisplayModel()
{
}

int CDebuggerRegisterDisplayModel::memoryType() const
{
   CRegisterDatabase* regDB = m_regDB();

   if ( regDB )
   {
      return regDB->GetType();
   }
   return 0;
}

bool CDebuggerRegisterDisplayModel::memoryContains(uint32_t addr) const
{
   CRegisterDatabase* regDB = m_regDB();

   if ( regDB )
   {
      return regDB->Contains(addr);
   }
   return false;
}

int CDebuggerRegisterDisplayModel::memoryBottom() const
{
   CRegisterDatabase* regDB = m_regDB();

   if ( regDB )
   {
      return regDB->GetRegister(0)->GetAddr();
   }
   return 0;
}

QVariant CDebuggerRegisterDisplayModel::data(const QModelIndex& index, int role) const
{
   CRegisterData* pRegister = (CRegisterData*)index.internalPointer();
   if (!index.isValid())
   {
      return QVariant();
   }

   if (role != Qt::DisplayRole)
   {
      return QVariant();
   }

   if ( pRegister )
   {
      sprintf ( modelStringBuffer, "%02X", (int)pRegister->Get() );
   }

   return QVariant(modelStringBuffer);
}

Qt::ItemFlags CDebuggerRegisterDisplayModel::flags(const QModelIndex& /*index*/) const
{
   Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;

   return flags;
}

QVariant CDebuggerRegisterDisplayModel::headerData(int section, Qt::Orientation orientation, int role) const
{
   if (role != Qt::DisplayRole)
   {
      return QVariant();
   }

   if ( m_regDB() )
   {
      if ( orientation == Qt::Horizontal )
      {
         sprintf(modelStringBuffer,m_regDB()->GetColumnHeading(section));
      }
      else
      {
         sprintf(modelStringBuffer,m_regDB()->GetRowHeading(section));
      }
   }

   return QVariant(modelStringBuffer);
}

bool CDebuggerRegisterDisplayModel::setData ( const QModelIndex& index, const QVariant& value, int )
{
   unsigned int data;
   bool ok = false;

   if ( m_regDB() )
   {
      data = value.toString().toInt(&ok,16);

      if ( ok )
      {
         m_regDB()->GetRegister((index.row()*m_regDB()->GetNumColumns())+index.column())->Set(data);
         emit dataChanged(index,index);
      }
   }

   return ok;
}

QModelIndex CDebuggerRegisterDisplayModel::index(int row, int column, const QModelIndex&) const
{
   if ( m_regDB() )
   {
      if ( (row >= 0) && (column >= 0) )
      {
         return createIndex(row,column,m_regDB()->GetRegister((row*m_regDB()->GetNumColumns())+column));
      }
   }

   return QModelIndex();
}

int CDebuggerRegisterDisplayModel::rowCount(const QModelIndex& /*parent*/) const
{
   if ( m_regDB() )
   {
      return m_regDB()->GetNumRows();
   }

   return 0;
}

int CDebuggerRegisterDisplayModel::columnCount(const QModelIndex& /*parent*/) const
{
   if ( m_regDB() )
   {
      return m_regDB()->GetNumColumns();
   }

   return 0;
}

void CDebuggerRegisterDisplayModel::update()
{
   emit layoutChanged();
}
