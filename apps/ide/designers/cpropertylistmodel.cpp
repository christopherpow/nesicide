#include <QInputDialog>

#include "cpropertylistmodel.h"

static const char* ADD_MESSAGE = "<add new Tile Properties in Project Properties>";

CPropertyListModel::CPropertyListModel(bool editable,QObject *parent) :
    QAbstractTableModel(parent)
{
   m_currentSortColumn = PropertyCol_Name;
   m_currentSortOrder = Qt::AscendingOrder;
   m_currentItemCount = 0;
   m_editable = editable;
}

CPropertyListModel::~CPropertyListModel()
{
}

Qt::ItemFlags CPropertyListModel::flags(const QModelIndex& index) const
{
   Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
   if ( m_editable )
   {
      if ( index.column() == PropertyCol_Value )
      {
         flags |= Qt::ItemIsEditable;
      }
   }
   return flags;
}

bool CPropertyListModel::setData(const QModelIndex &index, const QVariant &value, int /*role*/)
{
   bool ok = false;
   PropertyItem item;

   switch ( index.column() )
   {
      default:
         ok = false;
         break;
      case PropertyCol_Value:
         if ( index.row() < m_items.count() )
         {
            item = m_items.at(index.row());
            item.value = value.toString();
            m_items.replace(index.row(),item);
            emit dataChanged(index,index);
         }
         break;
   }

   return ok;
}

QVariant CPropertyListModel::data(const QModelIndex& index, int role) const
{
   QString data;

   if (role != Qt::DisplayRole)
   {
      return QVariant();
   }

   // Get data for columns...
   if ( index.row() < m_items.count() )
   {
      switch ( index.column() )
      {
      case PropertyCol_Name:
         return m_items.at(index.row()).name;
         break;
      case PropertyCol_Type:
         switch ( m_items.at(index.row()).type )
         {
         case propertyInteger:
            return QVariant("Integer");
            break;
         case propertyBoolean:
            return QVariant("Boolean");
            break;
         case propertyString:
            return QVariant("String");
            break;
         case propertyEnumeration:
            return QVariant("Enumeration");
            break;
         }
         break;
      case PropertyCol_Value:
         switch ( m_items.at(index.row()).type )
         {
         case propertyInteger:
         case propertyBoolean:
         case propertyString:
            return m_items.at(index.row()).value;
            break;
         case propertyEnumeration:
            return getEnumValueString(m_items.at(index.row()).value);
            break;
         }
         break;
      }
   }
   else
   {
      switch ( index.column() )
      {
      case PropertyCol_Name:
         return ADD_MESSAGE;
         break;
      }
   }
   return QVariant();
}

QVariant CPropertyListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
   if (role != Qt::DisplayRole)
   {
      return QVariant();
   }

   if ( orientation == Qt::Horizontal )
   {
      switch ( section )
      {
      case PropertyCol_Name:
         return QString("Name");
         break;
      case PropertyCol_Type:
         return QString("Type");
         break;
      case PropertyCol_Value:
         return QString("Default Value");
         break;
      }
   }
   return QVariant();
}

int CPropertyListModel::rowCount(const QModelIndex&) const
{
   int rows = m_items.count();
   if ( m_editable && (rows == 0) )
   {
      rows = 1;
   }
   return rows;
}

int CPropertyListModel::columnCount(const QModelIndex&) const
{
   return PropertyCol_MAX;
}

void CPropertyListModel::update()
{
   sort(m_currentSortColumn,m_currentSortOrder);
}

void CPropertyListModel::removeRow(int row, const QModelIndex &parent)
{
   if ( row < m_items.count() )
   {
      beginRemoveRows(parent,row,row);
      m_items.removeAt(row);
      endRemoveRows();
   }
}

bool CPropertyListModel::removeRows(int row, int count, const QModelIndex &parent)
{
   int idx;

   if ( row < m_items.count() )
   {
      beginRemoveRows(parent,row,row+count-1);
      for ( idx = row+count-1; idx >= row; idx-- )
      {
         m_items.removeAt(idx);
      }
      endRemoveRows();
      return true;
   }
   return false;
}

void CPropertyListModel::insertRow(QString text, const QModelIndex& parent)
{
   PropertyItem item;

   beginInsertRows(parent,m_items.count(),m_items.count());
   item.name = text;
   m_items.append(item);
   endInsertRows();
}

void CPropertyListModel::sort(int column, Qt::SortOrder order)
{
   int idx1;
   int idx2;
   QString strData1;
   QString strData2;
   unsigned int uiData1;
   unsigned int uiData2;

   if ( (m_currentSortColumn != column) ||
        (m_currentSortOrder != order) ||
        (m_currentItemCount != m_items.count()) )
   {
      for ( idx1 = 0; idx1 < m_items.count(); idx1++ )
      {
         for ( idx2 = idx1; idx2 < m_items.count(); idx2++ )
         {
            // Every other column can use string sorting.
            strData1 = data(index(idx1,column),Qt::DisplayRole).toString();
            strData2 = data(index(idx2,column),Qt::DisplayRole).toString();
            switch ( order )
            {
            case Qt::AscendingOrder:
               if ( strData1 > strData2 )
               {
                  m_items.swap(idx1,idx2);
               }
               break;
            case Qt::DescendingOrder:
               if ( strData2 > strData1 )
               {
                  m_items.swap(idx1,idx2);
               }
               break;
            }
         }
      }
   }

   m_currentSortColumn = column;
   m_currentSortOrder = order;
   m_currentItemCount = m_items.count();

   emit layoutChanged();
}
