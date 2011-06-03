#include "cchrromitemlistdisplaymodel.h"

#include "iprojecttreeviewitem.h"

#include "main.h"

static const char* CLICK_TO_ADD_OR_EDIT = "<click to add or edit>";

CChrRomItemListDisplayModel::CChrRomItemListDisplayModel(QObject* parent)
{
}

CChrRomItemListDisplayModel::~CChrRomItemListDisplayModel()
{
}

Qt::ItemFlags CChrRomItemListDisplayModel::flags(const QModelIndex& index) const
{
   Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
   if ( index.column() == ChrRomBankItemCol_Name )
   {
      flags |= Qt::ItemIsEditable;
   }

   return flags;
}

QVariant CChrRomItemListDisplayModel::data(const QModelIndex& index, int role) const
{
   if (!index.isValid())
   {
      return QVariant();
   }

   if ( index.row() < chrRomBankItems.count() )
   {
      IChrRomBankItem* item = chrRomBankItems.at(index.row());

      if ((role == Qt::DecorationRole) && (index.column() == ChrRomBankItemCol_Icon))
      {
         return item->getChrRomBankItemIcon();
      }

      if (role != Qt::DisplayRole)
      {
         return QVariant();
      }

      IProjectTreeViewItem* ptvi = dynamic_cast<IProjectTreeViewItem*>(item);

      switch (index.column())
      {
         case ChrRomBankItemCol_Type:
            return QVariant(item->getItemType());
            break;
         case ChrRomBankItemCol_Name:
            if (ptvi)
            {
               return QVariant(ptvi->caption());
            }
            else
            {
               return QVariant();
            }
            break;
         case ChrRomBankItemCol_Size:
            return QVariant(item->getChrRomBankItemSize());
            break;
         default:
            return QVariant("");
            break;
      }
   }
   else
   {
      if (role != Qt::DisplayRole)
      {
         return QVariant();
      }

      if ( index.column() == ChrRomBankItemCol_Name )
      {
         return QVariant(CLICK_TO_ADD_OR_EDIT);
      }
   }
   return QVariant();
}

QVariant CChrRomItemListDisplayModel::headerData(int section, Qt::Orientation orientation, int role) const
{
   if (role != Qt::DisplayRole)
   {
      return QVariant();
   }

   if (orientation != Qt::Horizontal)
   {
      return QVariant();
   }

   switch (section)
   {
      case ChrRomBankItemCol_Type:
         return QVariant("Item Type");
         break;
      case ChrRomBankItemCol_Name:
         return QVariant("Name");
         break;
      case ChrRomBankItemCol_Size:
         return QVariant("Size");
         break;
      default:
         return QVariant();
         break;
   }
}

int CChrRomItemListDisplayModel::rowCount(const QModelIndex& parent) const
{
   return chrRomBankItems.count()+1;
}

int CChrRomItemListDisplayModel::columnCount(const QModelIndex& parent) const
{
   return ChrRomBankItemCol_MAX;
}

bool CChrRomItemListDisplayModel::setData ( const QModelIndex& index, const QVariant& value, int )
{
   bool ok = false;
   IChrRomBankItem* item;

   IProjectTreeViewItemIterator iter(nesicideProject->getProject());
   do
   {
      item = dynamic_cast<IChrRomBankItem*>(iter.current());
      if ( item && (iter.current()->caption() == value.toString()) )
      {
         ok = true;
         break;
      }
      iter.next();
   } while ( iter.current() );

   if ( ok )
   {
      switch ( index.column() )
      {
         case ChrRomBankItemCol_Name:
            if ( index.row() < chrRomBankItems.count() )
            {
               chrRomBankItems.replace(index.row(),item);
               emit dataChanged(index,index);
               ok = true;
            }
            else
            {
               if ( (!value.toString().isEmpty()) &&
                    (value != CLICK_TO_ADD_OR_EDIT) )
               {
                  chrRomBankItems.append(item);
                  emit dataChanged(index,index);
                  ok = true;
               }
            }
            break;
         default:
            ok = false;
            break;
      }
   }

   return ok;
}

void CChrRomItemListDisplayModel::removeRow(int row, const QModelIndex &parent)
{
   if ( row < chrRomBankItems.count() )
   {
      beginRemoveRows(parent,row,row);
      chrRomBankItems.removeAt(row);
      endRemoveRows();
   }
}

void CChrRomItemListDisplayModel::insertRow(IChrRomBankItem* item, const QModelIndex& parent)
{
   beginInsertRows(parent,chrRomBankItems.count(),chrRomBankItems.count());
   chrRomBankItems.append(item);
   endInsertRows();
}

void CChrRomItemListDisplayModel::update()
{
   emit layoutChanged();
}

