#include "cchrromitemlistdisplaymodel.h"

CChrRomItemListDisplayModel::CChrRomItemListDisplayModel(QObject* parent)
{
}

CChrRomItemListDisplayModel::~CChrRomItemListDisplayModel()
{
}

QVariant CChrRomItemListDisplayModel::data(const QModelIndex& index, int role) const
{
   if (!index.isValid())
   {
      return QVariant();
   }

   IChrRomBankItem* item = chrRomBankItems.at(index.row());

   if (!item)
   {
      return QVariant();
   }

   if ((role == Qt::DecorationRole) && (index.column() == 0))
   {
      return item->getChrRomBankItemIcon();
   }

   if (role != Qt::DisplayRole)
   {
      return QVariant();
   }


   switch (index.column())
   {
      case 1:
         return QVariant(item->getItemType());
      case 2:
      {
         IProjectTreeViewItem* ptvi = dynamic_cast<IProjectTreeViewItem*>(item);

         if (ptvi)
         {
            return QVariant(ptvi->caption());
         }
         else
         {
            return QVariant();
         }
      }
      case 3:
         return QVariant(item->getChrRomBankItemSize());
      default:
         return QVariant("");
   }


}

Qt::ItemFlags CChrRomItemListDisplayModel::flags(const QModelIndex& index) const
{
   Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
   return flags;
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
      case 1:
         return QVariant("Item Type");
      case 2:
         return QVariant("Name");
      case 3:
         return QVariant("Size");
      default:
         return QVariant();
   }
}

QModelIndex CChrRomItemListDisplayModel::index(int row, int column, const QModelIndex& parent) const
{
   if ((row >= 0) && (row < chrRomBankItems.count()))
   {
      return createIndex(row, column, chrRomBankItems.at(row));
   }

   return QModelIndex();
}

int CChrRomItemListDisplayModel::rowCount(const QModelIndex& parent) const
{
   return chrRomBankItems.count();
}

int CChrRomItemListDisplayModel::columnCount(const QModelIndex& parent) const
{
   return 4;
}

void CChrRomItemListDisplayModel::layoutChangedEvent()
{
   this->layoutChanged();
}

bool CChrRomItemListDisplayModel::setData ( const QModelIndex& index, const QVariant& value, int )
{
   return false;
}


