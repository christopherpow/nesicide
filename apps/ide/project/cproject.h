#ifndef CPROJECT_H
#define CPROJECT_H

#include "cprojectbase.h"
#include "cprojectprimitives.h"
#include "csources.h"
#include "cbinaryfiles.h"
#include "cgraphicsbanks.h"
#include "csounds.h"

class CProject : public CProjectBase
{
   Q_OBJECT
public:
   CProject(IProjectTreeViewItem* parent);
   virtual ~CProject();

   // Helper functions
   void initializeProject();
   void terminateProject();

   CProjectPrimitives* getProjectPrimitives() { return m_pProjectPrimitives; }
   CSources* getSources() { return m_pSources; }
   CBinaryFiles* getBinaryFiles() { return m_pBinaryFiles; }
   CGraphicsBanks* getGraphicsBanks() { return m_pGraphicsBanks; }
   CSounds* getSounds() { return m_pSounds; }

   // IXMLSerializable Interface Implementation
   virtual bool serialize(QDomDocument& doc, QDomNode& node);
   virtual bool deserialize(QDomDocument& doc, QDomNode& node, QString& errors);

   // IProjectTreeViewItem Interface Implmentation
   QString caption() const;
   virtual void openItemEvent(CProjectTabWidget*) {}

   // ICenterWidgetItem Interface Implementation

private:
   // Attributes

   // Contained children
   CProjectPrimitives* m_pProjectPrimitives;
   CSources* m_pSources;
   CBinaryFiles* m_pBinaryFiles;
   CGraphicsBanks* m_pGraphicsBanks;
   CSounds* m_pSounds;
};

#endif // CPROJECT_H
