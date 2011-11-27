#ifndef CPROPERTYITEM_H
#define CPROPERTYITEM_H

typedef enum
{
   propertyInteger,
   propertyBoolean,
   propertyString,
   propertyEnumeration
} propertyTypeEnum;

struct PropertyItem
{
   QString name;
   propertyTypeEnum type;
   QString value;
};

#endif // CPROPERTYITEM_H
