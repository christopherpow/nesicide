#ifndef CPROJECTBASE_H
#define CPROJECTBASE_H

#include "ixmlserializable.h"
#include "iprojecttreeviewitem.h"

#include "cdesignereditorbase.h"

class CProjectBase : public QObject, public IXMLSerializable, public IProjectTreeViewItem
{
   Q_OBJECT
public:
   CProjectBase();
   virtual ~CProjectBase();

   void setName(QString name) { m_name = name; }

   QString path() { return m_path; }
   void setPath(QString path) { m_path = path; }

   void setEditor(CDesignerEditorBase* editor) { m_editor = editor; }
   CDesignerEditorBase* editor() { return m_editor; }

   void setIncludeInBuild(bool include) { m_includeInBuild = include; }
   bool includeInBuild() { return m_includeInBuild; }

   void markForDeletion() { m_deleted = true; }

   // IProjectTreeViewItem Interface Implmentation
   virtual QString caption() const { return m_name; }
   virtual void closeItemEvent();
   virtual bool exportData() { return true; }
   virtual bool canChangeName() { return false; }
   virtual bool onNameChanged(QString /*newValue*/) { return true; }
   virtual void saveItemEvent() {}

protected:
   // Attributes
   QString m_name;
   QString m_path;
   bool    m_deleted;
   bool    m_includeInBuild;
   CDesignerEditorBase* m_editor;
};

IProjectTreeViewItem* findProjectItem(QString uuid);

#endif // CPROJECTBASE_H
