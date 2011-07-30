#include <QInputDialog>

#include "csymbolwatchmodel.h"

#include "ccc65interface.h"
#include "emulator_core.h"

static char modelStringBuffer [ 2048 ];

static const char* CLICK_TO_ADD_OR_EDIT = "<click to add or edit>";

CSymbolWatchModel::CSymbolWatchModel(QObject *parent) :
    QAbstractTableModel(parent)
{
}

CSymbolWatchModel::~CSymbolWatchModel()
{
}

Qt::ItemFlags CSymbolWatchModel::flags(const QModelIndex& index) const
{
   Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
   if ( index.column() != 1 )
   {
      flags |= Qt::ItemIsEditable;
   }
   if ( (index.column() == 0) &&
        (index.row() < m_symbols.count()) )
   {
      flags |= Qt::ItemIsDragEnabled;
   }
   return flags;
}

QVariant CSymbolWatchModel::data(const QModelIndex& index, int role) const
{
   unsigned int addr;
   unsigned int absAddr;
   QString data;
   int count;
   int idx;

   if (role != Qt::DisplayRole)
   {
      return QVariant();
   }

   // Get data for columns...
   if ( index.row() < m_symbols.count() )
   {
      // Get symbol's index in debug information from its segment.
      count = CCC65Interface::getSymbolMatchCount(m_symbols.at(index.row()));
      for ( idx = 0; idx < count; idx++ )
      {
         if ( m_segments.at(index.row()) == CCC65Interface::getSymbolSegment(m_symbols.at(index.row()),idx) )
         {
            break;
         }
      }

      switch ( index.column() )
      {
         case 0:
            return m_symbols.at(index.row());
            break;
         case 1:
            // Get symbol's information based on its name and index.
            addr = CCC65Interface::getSymbolAddress(m_symbols.at(index.row()),idx);
            absAddr = CCC65Interface::getSymbolAbsoluteAddress(m_symbols.at(index.row()),idx);
            if ( addr != 0xFFFFFFFF )
            {
               nesGetPrintableAddressWithAbsolute(modelStringBuffer,addr,absAddr);
               return QVariant(modelStringBuffer);
            }
            else
            {
               return QVariant("ERROR: Unresolved");
            }
            break;
         case 2:
            addr = CCC65Interface::getSymbolAddress(m_symbols.at(index.row()),idx);
            if ( addr != 0xFFFFFFFF )
            {
               sprintf(modelStringBuffer,"%02X",nesGetMemory(addr));
               return QVariant(modelStringBuffer);
            }
            else
            {
               return QVariant("ERROR: Unresolved");
            }
            break;
      }
   }
   else
   {
      switch ( index.column() )
      {
         case 0:
            return QVariant(CLICK_TO_ADD_OR_EDIT);
            break;
      }
   }
   return QVariant();
}

bool CSymbolWatchModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
   bool ok = false;
   unsigned int addr;
   unsigned int absAddr;
   int count;
   int idx;
   QString symbol;
   QStringList symbols;
   QString selStr;
   int selIdx = 0;

   switch ( index.column() )
   {
      case 0:
         if ( index.row() < m_symbols.count() )
         {
            m_symbols.replace(index.row(),value.toString());
            m_segments.replace(index.row(),resolveSymbol(value.toString()));
            emit layoutChanged();
            ok = true;
         }
         else
         {
            if ( (!value.toString().isEmpty()) &&
                 (value != CLICK_TO_ADD_OR_EDIT) )
            {
               beginInsertRows(QModelIndex(),m_symbols.count()+1,m_symbols.count()+1);
               m_symbols.append(value.toString());
               m_segments.append(resolveSymbol(value.toString()));
               endInsertRows();

               ok = true;
            }
         }
         break;
      case 1:
         ok = false;
         break;
      case 2:
         if ( index.row() < m_symbols.count() )
         {
            addr = CCC65Interface::getSymbolAddress(m_symbols.at(index.row()));
            if ( addr != 0xFFFFFFFF )
            {
               nesSetCPUMemory(addr,value.toString().toInt(&ok,16));
            }
            emit dataChanged(index,index);
         }
         break;
   }

   return ok;
}

QVariant CSymbolWatchModel::headerData(int section, Qt::Orientation orientation, int role) const
{
   if (role != Qt::DisplayRole)
   {
      return QVariant();
   }

   if ( orientation == Qt::Horizontal )
   {
      switch ( section )
      {
      case 0:
         return QString("Symbol");
         break;
      case 1:
         return QString("Address");
         break;
      case 2:
         return QString("Value");
         break;
      }
   }
   return QVariant();
}

int CSymbolWatchModel::rowCount(const QModelIndex&) const
{
   return m_symbols.count()+1;
}

int CSymbolWatchModel::columnCount(const QModelIndex&) const
{
   return 3;
}

void CSymbolWatchModel::update()
{
   emit layoutChanged();
}

void CSymbolWatchModel::removeRow(int row, const QModelIndex &parent)
{
   if ( row < m_symbols.count() )
   {
      beginRemoveRows(parent,row,row);
      m_symbols.removeAt(row);
      endRemoveRows();
   }
}

void CSymbolWatchModel::insertRow(QString text, const QModelIndex& parent)
{
   beginInsertRows(parent,m_symbols.count(),m_symbols.count());
   m_symbols.append(text);
   m_segments.append(resolveSymbol(text));
   endInsertRows();
}

int CSymbolWatchModel::resolveSymbol(QString text)
{
   unsigned int addr;
   unsigned int absAddr;
   int count;
   int idx;
   QString symbol;
   QStringList symbols;
   QString selStr;
   int selIdx = 0;

   count = CCC65Interface::getSymbolMatchCount(text);
   if ( count > 1 )
   {
      for ( idx = 0; idx < count; idx++ )
      {
         addr = CCC65Interface::getSymbolAddress(text,idx);
         absAddr = CCC65Interface::getSymbolAbsoluteAddress(text,idx);

         symbol = text;
         symbol += " @";
         nesGetPrintableAddressWithAbsolute(modelStringBuffer,addr,absAddr);
         symbol += modelStringBuffer;
         symbols.append(symbol);
      }
      selStr = QInputDialog::getItem(0,"Help!","Symbol has multiple possible matches, pick one:",symbols,0,false);
      for ( selIdx = 0; selIdx < count; selIdx++ )
      {
         if ( symbols.at(selIdx) == selStr )
         {
            break;
         }
      }
   }
   return CCC65Interface::getSymbolSegment(text,selIdx);
}
