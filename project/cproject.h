#ifndef CPROJECT_H
#define CPROJECT_H

#include "cprojectbase.h"
#include "cprojectprimitives.h"
#include "csources.h"
#include "cbinaryfiles.h"
#include "cgraphicsbanks.h"

class CProject : public CProjectBase
{
public:
   CProject(IProjectTreeViewItem* parent);
   virtual ~CProject();

   // Helper functions
   void initializeProject();
   void terminateProject();

   CProjectPrimitives* getProjectPrimitives();
   CSourceItem* getMainSource();
   void setMainSource(CSourceItem* newSource);
   CSources* getSources();
   CBinaryFiles* getBinaryFiles();
   CGraphicsBanks* getGraphicsBanks();

   // IXMLSerializable Interface Implementation
   virtual bool serialize(QDomDocument& doc, QDomNode& node);
   virtual bool deserialize(QDomDocument& doc, QDomNode& node, QString& errors);

   // IProjectTreeViewItem Interface Implmentation
   QString caption() const;
   virtual void contextMenuEvent(QContextMenuEvent*, QTreeView*) {}
   virtual void openItemEvent(QTabWidget*) {}
   virtual bool onCloseQuery()
   {
      return true;
   }
   virtual void onClose() {}
   virtual bool isDocumentSaveable()
   {
      return false;
   }
   virtual void onSaveDocument() {}
   virtual bool canChangeName()
   {
      return false;
   }
   virtual bool onNameChanged(QString)
   {
      return true;
   }

private:
   // Attributes
   CSourceItem* m_mainSource;

   // Contained children
   CProjectPrimitives* m_pProjectPrimitives;
   CSources* m_pSources;
   CBinaryFiles* m_pBinaryFiles;
   CGraphicsBanks* m_pGraphicsBanks;
};

#endif // CPROJECT_H
