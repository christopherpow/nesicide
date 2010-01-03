#include "cdebuggerregisterdisplaymodel.h"

CDebuggerRegisterDisplayModel::CDebuggerRegisterDisplayModel(QObject* parent, eMemoryType display)
{
   m_display = display;
   m_register = 0;
   switch ( m_display )
   {
      case eMemory_PPUregs:
         m_offset = 0x2000;
         m_tblRegisters = tblPPURegisters;
      break;
      case eMemory_IOregs:
         m_offset = 0x4000;
         m_tblRegisters = tblAPURegisters;
      break;
      case eMemory_PPUoam:
      default:
         m_offset = 0;
         m_tblRegisters = NULL;
      break;
   }
}

CDebuggerRegisterDisplayModel::~CDebuggerRegisterDisplayModel()
{
}

QVariant CDebuggerRegisterDisplayModel::data(const QModelIndex &index, int role) const
{
   char data [ 64 ];

   if (!index.isValid())
      return QVariant();

   int regData = (int)index.internalPointer();
   if ( role == Qt::DisplayRole )
   {
      CBitfieldData* pBitfield = m_tblRegisters[m_register]->GetBitfield ( index.row() );
      if ( pBitfield->GetNumValues() )
      {
         sprintf ( data, "%s (%X)", pBitfield->GetValue(regData), (unsigned char)pBitfield->GetValueRaw(regData) );
      }
      else
      {
         sprintf ( data, "%02X", (unsigned char)pBitfield->GetValueRaw(regData) );
      }
      return QVariant(data);
   }
   else if ( role == Qt::EditRole )
   {
      sprintf ( data, "%02X", (unsigned char)regData );
      return QVariant(data);
   }
   return QVariant();
}

Qt::ItemFlags CDebuggerRegisterDisplayModel::flags(const QModelIndex &index) const
{
   Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
   return flags;
}

QVariant CDebuggerRegisterDisplayModel::headerData(int section, Qt::Orientation orientation, int role) const
{
   if (role != Qt::DisplayRole)
      return QVariant();
   char buffer [ 64 ];
   if ( orientation == Qt::Horizontal )
   {
      sprintf ( buffer, "Value" );
   }
   else
   {
      CBitfieldData* pBitfield = m_tblRegisters[m_register]->GetBitfield ( section );
      sprintf ( buffer, pBitfield->GetName() );
   }

   return  QString(buffer);
}

bool CDebuggerRegisterDisplayModel::setData ( const QModelIndex & index, const QVariant & value, int )
{
   unsigned char data;
   bool ok = false;

   data = value.toString().toInt(&ok,16);
   if ( ok && (data<256) )
   {
      switch ( m_display )
      {
         case eMemory_PPUregs:
            CPPU::_PPU(m_offset+m_register, data);
         break;
         case eMemory_IOregs:
            CAPU::_APU(m_offset+m_register, data);
         break;
         case eMemory_PPUoam:
            CPPU::OAM(index.column(), index.row(), data);
         break;
         default:
         break;
      }
      emit dataChanged(index,index);
   }
   return ok;
}

QModelIndex CDebuggerRegisterDisplayModel::index(int row, int column, const QModelIndex &parent) const
{
   switch ( m_display )
   {
      case eMemory_PPUregs:
         return createIndex(row, column, (int)CPPU::_PPU(m_offset+m_register));
      break;
      case eMemory_IOregs:
         return createIndex(row, column, (int)CAPU::_APU(m_offset+m_register));
      break;
      case eMemory_PPUoam:
         return createIndex(row, column, (int)CPPU::OAM(column,row));
      break;
      default:
         return QModelIndex();
      break;
   }
   return QModelIndex();
}

int CDebuggerRegisterDisplayModel::rowCount(const QModelIndex &parent) const
{
   return m_tblRegisters[m_register]->GetNumBitfields();
}

int CDebuggerRegisterDisplayModel::columnCount(const QModelIndex &parent) const
{
   return 1;
}

void CDebuggerRegisterDisplayModel::layoutChangedEvent()
{
    this->layoutChanged();
}
