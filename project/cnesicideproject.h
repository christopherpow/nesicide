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
   bool createProjectFromRom(QString fileName);
   void terminateProject();

   // Member Getters
   QString getProjectTitle();
   QList<CPaletteEntry> *getProjectPaletteEntries();
   bool isInitialized();
   CCartridge* getCartridge();
   CProject* getProject();

   // Member Setters
   void setProjectTitle(QString value);
   void setProjectPaletteEntries(QList<CPaletteEntry> *pProjectPaletteEntries);
   void setCartridge(CCartridge* pCartridge);
   void setProject(CProject* project);

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
   void initializeNodes();

   bool m_isInitialized;                                           // Is the project initialized?

   QString m_projectTitle;                                         // The visible title of the project
   QList<CPaletteEntry> *m_pProjectPaletteEntries;                 // List of palette entries for the emulator.
   CProject* m_pProject;
   CCartridge* m_pCartridge;
};

#endif // CNESICIDEPROJECT_H
