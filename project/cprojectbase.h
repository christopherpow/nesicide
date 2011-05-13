#ifndef CPROJECTBASE_H
#define CPROJECTBASE_H

#include "ixmlserializable.h"
#include "iprojecttreeviewitem.h"

#include "cdesignereditorbase.h"

class CProjectBase : public IXMLSerializable, public IProjectTreeViewItem
{
public:
   CProjectBase();
   virtual ~CProjectBase();

   QString name() { return m_name; }
   void setName(QString name) { m_name = name; }

   QString path() { return m_path; }
   void setPath(QString path) { m_path = path; }

   void setEditor(CDesignerEditorBase* editor) { m_editor = editor; }
   CDesignerEditorBase* editor() { return m_editor; }

   // IProjectTreeViewItem Interface Implmentation
   virtual QString caption() const { return m_name; }
   virtual void closeItemEvent();

protected:
   // Attributes
   QString m_name;
   QString m_path;
   CDesignerEditorBase* m_editor;
};

IProjectTreeViewItem* findProjectItem(QString uuid);

#endif // CPROJECTBASE_H
