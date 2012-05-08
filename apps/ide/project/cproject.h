#ifndef CPROJECT_H
#define CPROJECT_H

#include "cprojectbase.h"
#include "cprojectprimitives.h"
#include "csources.h"
#include "cbinaryfiles.h"
#include "cgraphicsbanks.h"

class CProject : public CProjectBase
{
   Q_OBJECT
public:
   CProject(IProjectTreeViewItem* parent);
   virtual ~CProject();

   // Helper functions
   void initializeProject();
   void terminateProject();

   CProjectPrimitives* getProjectPrimitives();
   CSources* getSources();
   CBinaryFiles* getBinaryFiles();
   CGraphicsBanks* getGraphicsBanks();

   // IXMLSerializable Interface Implementation
   virtual bool serialize(QDomDocument& doc, QDomNode& node);
   virtual bool deserialize(QDomDocument& doc, QDomNode& node, QString& errors);

   // IProjectTreeViewItem Interface Implmentation
   QString caption() const;
   virtual void contextMenuEvent(QContextMenuEvent*, QTreeView*) {}
   virtual void openItemEvent(CProjectTabWidget*) {}

   // ICenterWidgetItem Interface Implementation

private:
   // Attributes

   // Contained children
   CProjectPrimitives* m_pProjectPrimitives;
   CSources* m_pSources;
   CBinaryFiles* m_pBinaryFiles;
   CGraphicsBanks* m_pGraphicsBanks;
};

#endif // CPROJECT_H
