#include "cchrromitemtabledisplaymodel.h"

#include "iprojecttreeviewitem.h"

#include "cnesicideproject.h"

static const char* CLICK_TO_ADD = "<click to add>";

CChrRomItemTableDisplayModel::CChrRomItemTableDisplayModel(bool editable,QObject* /*parent*/)
{
   m_editable = editable;
}

CChrRomItemTableDisplayModel::~CChrRomItemTableDisplayModel()
{
}

QModelIndex CChrRomItemTableDisplayModel::index(int row, int column, const QModelIndex &/*parent*/) const
{
   if ( row < chrRomBankItems.count() )
   {
      return createIndex(row,column,chrRomBankItems.at(row));
   }
   else
   {
      return createIndex(row,column);
   }
}

Qt::ItemFlags CChrRomItemTableDisplayModel::flags(const QModelIndex& index) const
{
   Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
   if ( (m_editable) && (index.column() == ChrRomBankItemCol_Name) )
   {
      flags |= Qt::ItemIsEditable;
   }

   return flags;
}

QVariant CChrRomItemTableDisplayModel::data(const QModelIndex& index, int role) const
{
   if (!index.isValid())
   {
      return QVariant();
   }

   if ( index.row() < chrRomBankItems.count() )
   {
      IChrRomBankItem* item = chrRomBankItems.at(index.row());

      if ((role == Qt::DecorationRole) && (index.column() == ChrRomBankItemCol_Image))
      {
         return item->getChrRomBankItemImage();
      }

      if ((role == Qt::SizeHintRole))
      {
         return QSize(item->getChrRomBankItemImage().width(),item->getChrRomBankItemImage().height()+6);
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
         return QVariant(CLICK_TO_ADD);
      }
   }
   return QVariant();
}

QVariant CChrRomItemTableDisplayModel::headerData(int section, Qt::Orientation orientation, int role) const
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
   case ChrRomBankItemCol_Image:
      return QVariant("Image");
      break;
   default:
      return QVariant();
      break;
   }
}

int CChrRomItemTableDisplayModel::rowCount(const QModelIndex& /*parent*/) const
{
   int rows = chrRomBankItems.count();
   if ( m_editable )
   {
      rows += 1;
   }
   return rows;
}

int CChrRomItemTableDisplayModel::columnCount(const QModelIndex& /*parent*/) const
{
   return ChrRomBankItemCol_MAX;
}

bool CChrRomItemTableDisplayModel::setData ( const QModelIndex& index, const QVariant& value, int )
{
   bool ok = false;
   IChrRomBankItem* item;

   IProjectTreeViewItemIterator iter(CNesicideProject::instance()->getProject());
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
               emit layoutChanged();
               ok = true;
            }
            else
            {
               if ( (!value.toString().isEmpty()) &&
                    (value != CLICK_TO_ADD) )
               {
                  beginInsertRows(QModelIndex(),chrRomBankItems.count()+1,chrRomBankItems.count()+1);
                  chrRomBankItems.append(item);
                  endInsertRows();
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

void CChrRomItemTableDisplayModel::removeRow(int row, const QModelIndex &parent)
{
   if ( row < chrRomBankItems.count() )
   {
      beginRemoveRows(parent,row,row);
      chrRomBankItems.removeAt(row);
      endRemoveRows();
   }
}

void CChrRomItemTableDisplayModel::insertRow(IChrRomBankItem* item, const QModelIndex& parent)
{
   beginInsertRows(parent,chrRomBankItems.count(),chrRomBankItems.count());
   chrRomBankItems.append(item);
   endInsertRows();
}

void CChrRomItemTableDisplayModel::update()
{
   emit layoutChanged();
}

