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

   QString path() { return m_path; }
   void setPath(QString path) { m_path = path; }
   QString absolutePath();

   // IProjectTreeViewItem Interface Implmentation
   QString caption() const { return m_name; }

protected:
   // Attributes
   QString m_name;
   QString m_path;
   bool    m_isModified;
};

IProjectTreeViewItem* findProjectItem(QString uuid);

#endif // CPROJECTBASE_H
