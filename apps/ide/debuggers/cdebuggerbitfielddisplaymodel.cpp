#include "cdebuggerbitfielddisplaymodel.h"

static char modelStringBuffer [ 2048 ];

CDebuggerBitfieldDisplayModel::CDebuggerBitfieldDisplayModel(regDBFunc regDB,QObject*)
{
   m_regDB = regDB;
   m_register = 0;
}

CDebuggerBitfieldDisplayModel::~CDebuggerBitfieldDisplayModel()
{
}

QVariant CDebuggerBitfieldDisplayModel::data(const QModelIndex& index, int role) const
{
   char* pValues = modelStringBuffer;
   int value;

   if (!index.isValid())
   {
      return QVariant();
   }

   CRegisterData* pRegister = (CRegisterData*)index.internalPointer();
   CBitfieldData* pBitfield = pRegister->GetBitfield ( index.row() );
   uint32_t regData = pRegister->Get();

   if ( role == Qt::ToolTipRole )
   {
      if ( pBitfield->GetNumValues() )
      {
         pValues += sprintf ( pValues, "<pre>" );

         for ( value = 0; value < pBitfield->GetNumValues(); value++ )
         {
            if ( value == pBitfield->GetValueRaw(regData) )
            {
               pValues += sprintf ( pValues, "<b>%s</b>", pBitfield->GetValueByIndex(value) );
            }
            else
            {
               pValues += sprintf ( pValues, "%s", pBitfield->GetValueByIndex(value) );
            }

            if ( value < pBitfield->GetNumValues()-1 )
            {
               pValues += sprintf ( pValues, "\n" );
            }
         }

         pValues += sprintf ( pValues, "</pre>" );
         return QVariant(modelStringBuffer);
      }
   }

   if ( role == Qt::DisplayRole )
   {
      if ( pBitfield->GetNumValues() )
      {
         sprintf ( modelStringBuffer, "%s", pBitfield->GetValue(regData) );
      }
      else
      {
         sprintf ( modelStringBuffer, pBitfield->GetDisplayFormat(), pBitfield->GetValueRaw(regData) );
      }

      return QVariant(modelStringBuffer);
   }
   else if ( role == Qt::EditRole )
   {
      sprintf ( modelStringBuffer, pBitfield->GetDisplayFormat(), regData );
      return QVariant(modelStringBuffer);
   }

   return QVariant();
}

Qt::ItemFlags CDebuggerBitfieldDisplayModel::flags(const QModelIndex&) const
{
   Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
   return flags;
}

QVariant CDebuggerBitfieldDisplayModel::headerData(int section, Qt::Orientation orientation, int role) const
{
   if (role != Qt::DisplayRole)
   {
      return QVariant();
   }

   if ( orientation == Qt::Horizontal )
   {
      sprintf ( modelStringBuffer, "Value" );
   }
   else
   {
      if ( m_regDB() )
      {
         CBitfieldData* pBitfield = m_regDB()->GetRegister(m_register)->GetBitfield ( section );

         if ( pBitfield->GetWidth() == 1 )
         {
            sprintf ( modelStringBuffer, "[%d] %s", pBitfield->GetLsb(), pBitfield->GetName() );
         }
         else
         {
            sprintf ( modelStringBuffer, "[%d:%d] %s", pBitfield->GetMsb(), pBitfield->GetLsb(), pBitfield->GetName() );
         }
      }
   }

   return QVariant(modelStringBuffer);
}

bool CDebuggerBitfieldDisplayModel::setData ( const QModelIndex& index, const QVariant& value, int )
{
   int data;
   bool ok = false;

   if ( m_regDB() )
   {
      data = value.toString().toInt(&ok,16);

      if ( ok )
      {
         m_regDB()->GetRegister(m_register)->Set(data);
         emit dataChanged(index,index);
      }
   }

   return ok;
}

QModelIndex CDebuggerBitfieldDisplayModel::index(int row, int column, const QModelIndex&) const
{
   if ( (row >= 0) && (column >= 0) )
   {
      if ( m_regDB() )
      {
         return createIndex(row,column,m_regDB()->GetRegister(m_register));
      }
   }

   return QModelIndex();
}

int CDebuggerBitfieldDisplayModel::rowCount(const QModelIndex&) const
{
   if ( m_regDB() )
   {
      return m_regDB()->GetRegister(m_register)->GetNumBitfields();
   }

   // Nothing to display here...
   return 0;
}

int CDebuggerBitfieldDisplayModel::columnCount(const QModelIndex&) const
{
   if ( m_regDB() )
   {
      return 1;
   }

   // Nothing to display here...
   return 0;
}

void CDebuggerBitfieldDisplayModel::update()
{
   emit layoutChanged();
}
