#ifndef CPROJECTBASE_H
#define CPROJECTBASE_H

#include "ixmlserializable.h"
#include "iprojecttreeviewitem.h"

class CProjectBase : public IXMLSerializable, public IProjectTreeViewItem
{
public:
   CProjectBase();
   
   QString name() { return m_name; }
   void setName(QString name) { m_name = name; }
   
   // IProjectTreeViewItem Interface Implmentation
   QString caption() const { return m_name; }

protected:
   // Attributes
   QString m_name;
   bool    m_isModified;
};

IProjectTreeViewItem* findProjectItem(QString uuid);

#endif // CPROJECTBASE_H
