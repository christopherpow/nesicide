#ifndef CBINARYFILES_H
#define CBINARYFILES_H

#include "cprojectbase.h"
#include "cbinaryfile.h"
#include "cprojecttreeviewmodel.h"

#include <QList>
#include <QMenu>
#include <QFileDialog>

class CBinaryFiles : public CProjectBase
{
   Q_OBJECT
public:
   CBinaryFiles(IProjectTreeViewItem* parent);
   virtual ~CBinaryFiles();

   // Helper functions
   void initializeProject();
   void terminateProject();

   QList<CBinaryFile*>& getBinaryFileList() { return m_binaryFiles; }

   // IXMLSerializable Interface Implementation
   virtual bool serialize(QDomDocument& doc, QDomNode& node);
   virtual bool deserialize(QDomDocument& doc, QDomNode& node, QString& errors);

   // IProjectTreeViewItem Interface Implmentation
   QString caption() const;
   virtual void contextMenuEvent(QContextMenuEvent* event, QTreeView* parent);
   virtual void openItemEvent(CProjectTabWidget*) {}
   virtual bool onCloseQuery()
   {
      return true;
   }
   virtual void onClose() {}
   virtual void saveItemEvent() {}
   virtual bool canChangeName()
   {
      return false;
   }
   virtual bool onNameChanged(QString)
   {
      return true;
   }

private:
   // Contained children
   QList<CBinaryFile*> m_binaryFiles;
};

#endif // CBINARYFILES_H
