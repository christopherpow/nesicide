#include "cchrromitemlistdisplaymodel.h"

#include "iprojecttreeviewitem.h"

CChrRomItemListDisplayModel::CChrRomItemListDisplayModel(QObject* parent)
{
}

CChrRomItemListDisplayModel::~CChrRomItemListDisplayModel()
{
}

QModelIndex CChrRomItemListDisplayModel::index(int row, int column, const QModelIndex &/*parent*/) const
{
   if ( row < chrRomBankItems.count() )
   {
      return createIndex(row,column,chrRomBankItems.at(row));
   }
   return QModelIndex();
}

Qt::ItemFlags CChrRomItemListDisplayModel::flags(const QModelIndex& /*index*/) const
{
   Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

   return flags;
}

QVariant CChrRomItemListDisplayModel::data(const QModelIndex& index, int role) const
{
   if ( !index.isValid() )
   {
      return QVariant();
   }

   IChrRomBankItem* item = chrRomBankItems.at(index.row());

   if (role == Qt::DecorationRole)
   {
      return item->getChrRomBankItemImage();
   }

   if (role == Qt::SizeHintRole)
   {
      return QSize(item->getChrRomBankItemImage().width(),item->getChrRomBankItemImage().height()+6);
   }

   if (role != Qt::DisplayRole)
   {
      return QVariant();
   }

   IProjectTreeViewItem* ptvi = dynamic_cast<IProjectTreeViewItem*>(item);

   if (role == Qt::DisplayRole)
   {
      return QVariant(ptvi->caption());
   }

   return QVariant();
}

int CChrRomItemListDisplayModel::rowCount(const QModelIndex& /*parent*/) const
{
   int rows = chrRomBankItems.count();
   return rows;
}

int CChrRomItemListDisplayModel::columnCount(const QModelIndex& /*parent*/) const
{
   return 1;
}

void CChrRomItemListDisplayModel::update()
{
   emit dataChanged(QModelIndex(),QModelIndex());
}

