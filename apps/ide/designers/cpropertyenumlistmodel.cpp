#include <QInputDialog>

#include "cpropertyenumlistmodel.h"
#include "cpropertyitem.h"

static const char* CLICK_TO_ADD_OR_EDIT = "<click to add or edit>";

CPropertyEnumListModel::CPropertyEnumListModel(bool editable,QObject *parent) :
    QAbstractTableModel(parent)
{
   m_editable = editable;
}

CPropertyEnumListModel::~CPropertyEnumListModel()
{
}

Qt::ItemFlags CPropertyEnumListModel::flags(const QModelIndex& index) const
{
   Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
   if ( m_editable )
   {
      if ( index.column() != PropertyEnumCol_Default )
      {
         flags |= Qt::ItemIsEditable;
      }
      if ( (index.column() == PropertyEnumCol_Default) &&
           (index.row() < m_items.count()) )
      {
         flags |= Qt::ItemIsUserCheckable;
      }
   }
   return flags;
}

QVariant CPropertyEnumListModel::data(const QModelIndex& index, int role) const
{
   QString data;

   // Get data for columns...
   if ( index.row() < m_items.count() )
   {
      switch ( index.column() )
      {
      case PropertyEnumCol_Default:
         if (role == Qt::CheckStateRole)
         {
            return QVariant(m_items.at(index.row()).isSelected?Qt::Checked:Qt::Unchecked);
         }
         else
         {
            return QVariant();
         }
         break;
      case PropertyEnumCol_Symbol:
         if (role == Qt::DisplayRole)
         {
            return QVariant(m_items.at(index.row()).symbol);
         }
         else
         {
            return QVariant();
         }
         break;
      case PropertyEnumCol_Value:
         if (role == Qt::DisplayRole)
         {
            return QVariant(m_items.at(index.row()).value);
         }
         else
         {
            return QVariant();
         }
         break;
      }
   }
   else
   {
      switch ( index.column() )
      {
         case PropertyEnumCol_Symbol:
            if (role == Qt::DisplayRole)
            {
               return QVariant(CLICK_TO_ADD_OR_EDIT);
            }
            else
            {
               return QVariant();
            }
            break;
      }
   }
   return QVariant();
}

bool CPropertyEnumListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
   bool ok = false;
   unsigned int addr;
   unsigned int absAddr;
   int count;
   int idx;
   QString symbol;
   QStringList symbols;
   PropertyEnumItem item;

   switch ( index.column() )
   {
   case PropertyEnumCol_Default:
      if ( m_editable )
      {
         if ( index.row() < m_items.count() )
         {
            if ( role == Qt::CheckStateRole )
            {
               item = m_items.at(index.row());
               item.isSelected = value.toBool();
               m_items.replace(index.row(),item);
               emit layoutChanged();
               return true;
            }
         }
      }
      break;
   case PropertyEnumCol_Symbol:
      if ( m_editable )
      {
         if ( index.row() < m_items.count() )
         {
            item = m_items.at(index.row());
            item.symbol = value.toString();
            m_items.replace(index.row(),item);
            emit layoutChanged();
            ok = true;
         }
         else
         {
            if ( (!value.toString().isEmpty()) &&
                 (value != CLICK_TO_ADD_OR_EDIT) )
            {
               beginInsertRows(QModelIndex(),m_items.count()+1,m_items.count()+1);
               item.symbol = value.toString();
               item.isSelected = false;
               item.value = "0";
               m_items.append(item);
               endInsertRows();

               ok = true;
            }
         }
      }
      break;
   default:
      ok = false;
      break;
   case PropertyEnumCol_Value:
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

QVariant CPropertyEnumListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
   if (role != Qt::DisplayRole)
   {
      return QVariant();
   }

   if ( orientation == Qt::Horizontal )
   {
      switch ( section )
      {
      case PropertyEnumCol_Default:
         return QString("Default");
         break;
      case PropertyEnumCol_Symbol:
         return QString("Symbol");
         break;
      case PropertyEnumCol_Value:
         return QString("Value");
         break;
      }
   }
   return QVariant();
}

int CPropertyEnumListModel::rowCount(const QModelIndex&) const
{
   if ( m_editable )
   {
      return m_items.count()+1;
   }
   else
   {
      return m_items.count();
   }
}

int CPropertyEnumListModel::columnCount(const QModelIndex&) const
{
   return PropertyEnumCol_MAX;
}

void CPropertyEnumListModel::update()
{
   emit layoutChanged();
}

void CPropertyEnumListModel::removeRow(int row, const QModelIndex &parent)
{
   if ( row < m_items.count() )
   {
      beginRemoveRows(parent,row,row);
      m_items.removeAt(row);
      endRemoveRows();
   }
}

bool CPropertyEnumListModel::removeRows(int row, int count, const QModelIndex &parent)
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

void CPropertyEnumListModel::insertRow(QString text, const QModelIndex& parent)
{
   PropertyEnumItem item;

   beginInsertRows(parent,m_items.count(),m_items.count());
   item.symbol = text;
   item.value = "0";
   item.isSelected = false;
   m_items.append(item);
   endInsertRows();
}

QString CPropertyEnumListModel::getItems()
{
   return getEnumRawString(m_items);
}

void CPropertyEnumListModel::setItems(QString itemsStr)
{
   QStringList itemsStrList = itemsStr.split(";",QString::SkipEmptyParts);
   PropertyEnumItem item;

   foreach ( QString itemStr, itemsStrList )
   {
      QStringList itemParts = itemStr.split(",");
      item.isSelected = itemParts.at(PropertyEnumCol_Default).toInt();
      item.symbol = itemParts.at(PropertyEnumCol_Symbol);
      item.value = itemParts.at(PropertyEnumCol_Value);
      m_items.append(item);
   }
}
