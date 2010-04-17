#include "ccodebrowserdisplaymodel.h"

#include "cnes6502.h"
#include "cnesrom.h"

CCodeBrowserDisplayModel::CCodeBrowserDisplayModel(QObject* parent)
{
}

CCodeBrowserDisplayModel::~CCodeBrowserDisplayModel()
{
}

QVariant CCodeBrowserDisplayModel::data(const QModelIndex &index, int role) const
{
   UINT addr = (UINT)index.internalPointer();
   char buffer [ 3 ];
   unsigned char opSize;

   if (!index.isValid())
      return QVariant();

   if (role != Qt::DisplayRole)
      return QVariant();

   switch ( index.column() )
   {
      case 0:
         sprintf ( buffer, "%02X", CROM::PRGROM(addr) );
         return buffer;
      break;
      case 1:
         opSize = C6502::OpcodeSize ( CROM::PRGROM(addr) );
         if ( 1 < opSize )
         {
            sprintf ( buffer, "%02X", CROM::PRGROM(addr+1) );
            return buffer;
         }
      break;
      case 2:
         opSize = C6502::OpcodeSize ( CROM::PRGROM(addr) );
         if ( 2 < opSize )
         {
            sprintf ( buffer, "%02X", CROM::PRGROM(addr+2) );
            return buffer;
         }
      break;
      case 3:
         return CROM::DISASSEMBLY(addr);
      break;
   }

   return QVariant();
}

Qt::ItemFlags CCodeBrowserDisplayModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    return flags;
}

QVariant CCodeBrowserDisplayModel::headerData(int section, Qt::Orientation orientation, int role) const
{
   char buffer [ 6 ];
   int addr;

   if (role != Qt::DisplayRole)
      return QVariant();

   if ( orientation == Qt::Horizontal )
   {
      switch ( section )
      {
         case 0:
            return "Op";
         break;
         case 1:
            return "Lo";
         break;
         case 2:
            return "Hi";
         break;
         case 3:
            return "Disassembly";
         break;
      }
   }
   else
   {
      addr = CROM::SLOC2ADDR(section);

      sprintf ( buffer, "$%04X", addr );
      return buffer;
   }

   return  QVariant();
}

QModelIndex CCodeBrowserDisplayModel::index(int row, int column, const QModelIndex &parent) const
{
   int addr;

   addr = CROM::SLOC2ADDR(row);

   return createIndex(row, column, addr);
}

int CCodeBrowserDisplayModel::rowCount(const QModelIndex &parent) const
{
   // Get the source-lines-of-code count from each disassembled ROM
   // bank that is currently visible to the CPU...
   int rows = CROM::SLOC(0x8000)+CROM::SLOC(0xA000)+CROM::SLOC(0xC000)+CROM::SLOC(0xE000);

   return rows;
}

int CCodeBrowserDisplayModel::columnCount(const QModelIndex &parent) const
{
   if ( parent.isValid())
   {
      return 0;
   }
   return 4;
}

void CCodeBrowserDisplayModel::layoutChangedEvent()
{
    this->layoutChanged();
}
