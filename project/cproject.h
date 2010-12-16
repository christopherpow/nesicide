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
   CProject();
   virtual ~CProject();

   CProjectPrimitives* getProjectPrimitives();
   void setProjectPrimitives(CProjectPrimitives* newProjectPrimitives);
   CSourceItem* getMainSource();
   void setMainSource(CSourceItem* newSource);
   CSources* getSources();
   void setSources(CSources* newSources);
   CBinaryFiles* getBinaryFiles();
   void setBinaryFiles(CBinaryFiles* newBinaryFiles);
   CGraphicsBanks* getGraphicsBanks();
   void setGraphicsBanks(CGraphicsBanks* newGraphicsBanks);

   // IXMLSerializable Interface Implementation
   virtual bool serialize(QDomDocument& doc, QDomNode& node);
   virtual bool deserialize(QDomDocument& doc, QDomNode& node);

   // IProjectTreeViewItem Interface Implmentation
   QString caption() const;
   virtual void contextMenuEvent(QContextMenuEvent*, QTreeView*) {}
   virtual void openItemEvent(QTabWidget*) {}
   virtual bool onCloseQuery()
   {
      return true;
   }
   virtual void onClose() {}
   virtual int getTabIndex()
   {
      return -1;
   }
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
   CSources* m_pSources;
   CProjectPrimitives* m_pProjectPrimitives;
   CSourceItem* m_mainSource;
   CBinaryFiles* m_pBinaryFiles;
   CGraphicsBanks* m_pGraphicsBanks;
};

#endif // CPROJECT_H
