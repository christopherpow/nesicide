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
   QString getProjectOutputBasePath() { return m_projectOutputBasePath; }
   QString getProjectOutputName() { return m_projectOutputName; }
   QString getProjectLinkerOutputName() { return m_projectLinkerOutputName; }
   QString getProjectDebugInfoName() { return m_projectDebugInfoName; }
   QString getProjectCHRROMOutputName() { return m_projectCHRROMOutputName; }
   QString getProjectCartridgeOutputName() { return m_projectCartridgeOutputName; }
   QString getCompilerDefinedSymbols() { return m_compilerDefinedSymbols; }
   QString getCompilerIncludePaths() { return m_compilerIncludePaths; }
   QString getCompilerAdditionalOptions() { return m_compilerAdditionalOptions; }
   QString getLinkerConfigFile() { return m_linkerConfigFile; }
   QString getLinkerAdditionalOptions() { return m_linkerAdditionalOptions; }
   QList<CPaletteEntry> *getProjectPaletteEntries() { return &m_projectPaletteEntries; }
   CCartridge* getCartridge() { return m_pCartridge; }
   CProject* getProject() { return m_pProject; }

   // Member Setters
   void setProjectTitle(QString value)
   {
      m_projectTitle = value;
      m_projectOutputName = m_projectTitle.toLower().replace(" ","_");
      m_projectLinkerOutputName = m_projectTitle.toLower().replace(" ","_")+".prg";
      m_projectDebugInfoName = m_projectTitle.toLower().replace(" ","_")+".dbg";
      m_projectCHRROMOutputName = m_projectTitle.toLower().replace(" ","_")+".chr";
      m_projectCartridgeOutputName = m_projectTitle.toLower().replace(" ","_")+".nes";
   }
   void setProjectOutputBasePath(QString value) { m_projectOutputBasePath = value; }
   void setProjectOutputName(QString value) { m_projectOutputName = value; }
   void setProjectLinkerOutputName(QString value) { m_projectLinkerOutputName = value; }
   void setProjectDebugInfoName(QString value) { m_projectDebugInfoName = value; }
   void setProjectCHRROMOutputName(QString value) { m_projectCHRROMOutputName = value; }
   void setProjectCartridgeOutputName(QString value) { m_projectCartridgeOutputName = value; }
   void setCompilerDefinedSymbols(QString value) { m_compilerDefinedSymbols = value; }
   void setCompilerIncludePaths(QString value) { m_compilerIncludePaths = value; }
   void setCompilerAdditionalOptions(QString value) { m_compilerAdditionalOptions = value; }
   void setLinkerConfigFile(QString value) { m_linkerConfigFile = value; }
   void setLinkerAdditionalOptions(QString value) { m_linkerAdditionalOptions = value; }
   void setProjectPaletteEntries(QList<CPaletteEntry> *pProjectPaletteEntries);

   // IXMLSerializable Interface Implementation
   virtual bool serialize(QDomDocument& doc, QDomNode& node);
   virtual bool deserialize(QDomDocument& doc, QDomNode& node, QString& errors);

   // IProjectTreeViewItem Interface Implmentation
   QString caption() const;
   virtual void contextMenuEvent(QContextMenuEvent*, QTreeView*) {}
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
   // Is the project initialized?
   bool m_isInitialized;

   // Attributes
   // The visible title of the project
   QString m_projectTitle;
   QString m_projectOutputBasePath;
   QString m_projectOutputName;
   QString m_projectLinkerOutputName;
   QString m_projectDebugInfoName;
   QString m_projectCHRROMOutputName;
   QString m_projectCartridgeOutputName;
   // The toolchain argument strings
   QString m_compilerDefinedSymbols;
   QString m_compilerIncludePaths;
   QString m_compilerAdditionalOptions;
   QString m_linkerConfigFile;
   QString m_linkerAdditionalOptions;

   // Contained children
   QList<CPaletteEntry> m_projectPaletteEntries;                 // List of palette entries for the emulator.
   CProject*            m_pProject;
   CCartridge*          m_pCartridge;
};

#endif // CNESICIDEPROJECT_H
