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
   QString getProjectTitle() { return m_projectTitle; }
   QString getProjectBasePath() { return m_projectBasePath; }
   QString getProjectSourceBasePath() { return m_projectSourceBasePath; }
   QString getProjectOutputBasePath() { return m_projectOutputBasePath; }
   QString getProjectOutputName() { return m_projectOutputName; }
   QString getCompilerToolchain() { return m_compilerToolchain; }
   QString getCompilerDefinedSymbols() { return m_compilerDefinedSymbols; }
   QString getCompilerUndefinedSymbols() { return m_compilerUndefinedSymbols; }
   QString getCompilerIncludePaths() { return m_compilerIncludePaths; }
   QList<CPaletteEntry> *getProjectPaletteEntries() { return &m_projectPaletteEntries; }
   CCartridge* getCartridge() { return m_pCartridge; }
   CProject* getProject() { return m_pProject; }

   // Member Setters
   void setProjectTitle(QString value) { m_projectTitle = value; m_projectOutputName = m_projectTitle.toLower().replace(" ","_"); }
   void setProjectBasePath(QString value) { m_projectBasePath = value; }
   void setProjectSourceBasePath(QString value) { m_projectSourceBasePath = value; }
   void setProjectOutputBasePath(QString value) { m_projectOutputBasePath = value; }
   void setProjectOutputName(QString value) { m_projectOutputName = value; }
   void setCompilerToolchain(QString value) { m_compilerToolchain = value; }
   void setCompilerDefinedSymbols(QString value) { m_compilerDefinedSymbols = value; }
   void setCompilerUndefinedSymbols(QString value) { m_compilerUndefinedSymbols = value; }
   void setCompilerIncludePaths(QString value) { m_compilerIncludePaths = value; }
   void setProjectPaletteEntries(QList<CPaletteEntry> *pProjectPaletteEntries);

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
   // Is the project initialized?
   bool m_isInitialized;

   // Attributes
   // The visible title of the project
   QString m_projectTitle;
   QString m_projectBasePath;
   QString m_projectSourceBasePath;
   QString m_projectOutputBasePath;
   QString m_projectOutputName;
   // The toolchain used to compile the project
   QString m_compilerToolchain;
   // The toolchain argument strings
   QString m_compilerDefinedSymbols;
   QString m_compilerUndefinedSymbols;
   QString m_compilerIncludePaths;

   // Contained children
   QList<CPaletteEntry> m_projectPaletteEntries;                 // List of palette entries for the emulator.
   CProject*            m_pProject;
   CCartridge*          m_pCartridge;
};

#endif // CNESICIDEPROJECT_H
