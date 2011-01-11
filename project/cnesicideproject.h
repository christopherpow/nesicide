#ifndef CNESICIDEPROJECT_H
#define CNESICIDEPROJECT_H

#include "cprojectbase.h"
#include "ccartridge.h"
#include "cproject.h"

#include <QString>
#include <QMessageBox>

typedef QColor CPaletteEntry;

class CNesicideProject : public CProjectBase
{

public:
   // Global Constructor / Destructors
   CNesicideProject();
   virtual ~CNesicideProject();

   // Search functions
   int findSource ( char* objname, char** objdata, int* size );

   // Helper functions
   void initializeProject();
   void terminateProject();
   bool createProjectFromRom(QString fileName);

   // Member Getters
   bool isInitialized();
   QString getProjectTitle();
   QList<CPaletteEntry> *getProjectPaletteEntries() { return &m_projectPaletteEntries; }
   CCartridge* getCartridge() { return m_pCartridge; }
   CProject* getProject() { return m_pProject; }

   // Member Setters
   void setProjectTitle(QString value);
   void setProjectPaletteEntries(QList<CPaletteEntry> *pProjectPaletteEntries);

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
   bool m_isInitialized;                                           // Is the project initialized?

   // Attributes
   QString m_projectTitle;                                         // The visible title of the project
   
   // Contained children
   QList<CPaletteEntry> m_projectPaletteEntries;                 // List of palette entries for the emulator.
   CProject*            m_pProject;
   CCartridge*          m_pCartridge;
};

#endif // CNESICIDEPROJECT_H
