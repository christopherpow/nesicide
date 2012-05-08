#include <QList>
#include <QStringList>

#include "cpropertyitem.h"
#include "cpropertyenumlistmodel.h"

QString getEnumRawString(QList<PropertyEnumItem> items)
{
   QString itemsStr;
   itemsStr = "";
   foreach ( PropertyEnumItem item, items )
   {
      itemsStr += QString::number((int)item.isSelected);
      itemsStr += ",";
      itemsStr += item.symbol;
      itemsStr += ",";
      itemsStr += item.value;
      itemsStr += ";";
   }
   return itemsStr;
}

QString getEnumValueString(QString itemsStr)
{
   QStringList itemsStrList = itemsStr.split(";",QString::SkipEmptyParts);
   QString valueStr;
   PropertyEnumItem item;
   bool first = true;

   valueStr = "";
   foreach ( QString itemStr, itemsStrList )
   {
      QStringList itemParts = itemStr.split(",");
      item.isSelected = itemParts.at(PropertyEnumCol_Default).toInt();
      item.symbol = itemParts.at(PropertyEnumCol_Symbol);
      item.value = itemParts.at(PropertyEnumCol_Value);
      if ( item.isSelected )
      {
         if ( !first )
         {
            valueStr += " | ";
         }
         first = false;
         valueStr += item.symbol;
      }
   }
   return valueStr;
}

QList<PropertyEnumItem> getEnumItems(QString itemsStr)
{
   QStringList itemsStrList = itemsStr.split(";",QString::SkipEmptyParts);
   QList<PropertyEnumItem> items;
   QString valueStr;
   PropertyEnumItem item;

   foreach ( QString itemStr, itemsStrList )
   {
      QStringList itemParts = itemStr.split(",");
      item.isSelected = itemParts.at(PropertyEnumCol_Default).toInt();
      item.symbol = itemParts.at(PropertyEnumCol_Symbol);
      item.value = itemParts.at(PropertyEnumCol_Value);
      items.append(item);
   }
   return items;
}
