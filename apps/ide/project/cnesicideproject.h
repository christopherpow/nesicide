#ifndef CNESICIDEPROJECT_H
#define CNESICIDEPROJECT_H

#include "cprojectbase.h"
#include "ccartridge.h"
#include "cproject.h"
#include "cpropertylistmodel.h"

#include <QString>
#include <QStringList>
#include <QMessageBox>
#include <QDomDocument>

typedef QColor CPaletteEntry;

class CNesicideProject : public CProjectBase
{
   Q_OBJECT
public:
   static CNesicideProject *instance()
   {
      if ( !_instance )
      {
         _instance = new CNesicideProject();
      }
      return _instance;
   }
   // Global Constructor / Destructors
   virtual ~CNesicideProject();

   // Helper functions
   void initializeProject();
   void terminateProject();
   bool createProjectFromRom(QString fileName,bool silent=false);
   bool createRomFromProject(QString fileName);
   void setDirty(bool dirty) { m_isDirty = dirty; }
   bool isDirty() const { return m_isDirty; }

   // Member Getters
   bool isInitialized() const { return m_isInitialized; }
   QString getProjectFileName() { return m_projectFileName; }
   QString getProjectTarget() { return m_projectTarget; }
   QString getProjectTitle() { return m_projectTitle; }
   QStringList getProjectAddOns() { return m_projectAddOns; }
   QString getProjectOutputBasePath() { return m_projectOutputBasePath; }
   QString getProjectOutputName() { return m_projectOutputName; }
   QString getProjectHeaderFileName() { return m_projectHeaderFileName; }
   QString getProjectSourceFileName() { return m_projectSourceFileName; }
   QString getProjectLinkerOutputBasePath() { return m_projectLinkerOutputBasePath; }
   QString getProjectLinkerOutputName() { return m_projectLinkerOutputName; }
   QString getProjectDebugInfoName() { return m_projectDebugInfoName; }
   QString getProjectCHRROMOutputBasePath() { return m_projectCHRROMOutputBasePath; }
   QString getProjectCHRROMOutputName() { return m_projectCHRROMOutputName; }
   bool    getProjectUsesCHRROM() { return m_projectUsesCHRROM; }
   QString getProjectCartridgeOutputName() { return m_projectCartridgeOutputName; }
   QString getProjectCartridgeSaveStateName() { return m_projectCartridgeSaveStateName; }
   QString getCompilerDefinedSymbols() { return m_compilerDefinedSymbols; }
   QString getCompilerIncludePaths() { return m_compilerIncludePaths; }
   QString getCompilerAdditionalOptions() { return m_compilerAdditionalOptions; }
   QString getAssemblerDefinedSymbols() { return m_assemblerDefinedSymbols; }
   QString getAssemblerIncludePaths() { return m_assemblerIncludePaths; }
   QString getAssemblerAdditionalOptions() { return m_assemblerAdditionalOptions; }
   QString getLinkerConfigFile() { return m_linkerConfigFile; }
   QStringList getMakefileCustomRuleFiles() { return m_makefileCustomRuleFiles; }
   QString getMakefileCustomRules(QString rulesFile);
   QString getLinkerAdditionalOptions() { return m_linkerAdditionalOptions; }
   QString getLinkerAdditionalDependencies() { return m_linkerAdditionalDependencies; }
   QStringList getSourceSearchPaths() { return m_sourceSearchPaths; }
   QList<PropertyItem> getTileProperties() { return m_tileProperties; }

   QDomDocument& getSaveStateDoc() { return m_saveStateDoc; }

   QList<CPaletteEntry> *getProjectPaletteEntries() { return &m_projectPaletteEntries; }
   CCartridge* getCartridge() { return m_pCartridge; }
   CProject* getProject() { return m_pProject; }

   // Member Setters
   void setProjectFileName(QString value) { m_projectFileName = value; }
   void setProjectTarget(QString value) { m_projectTarget = value; }
   void setProjectTitle(QString value)
   {
      m_projectTitle = value;
      m_projectOutputName = m_projectTitle.toLower().replace(" ","_");
      if ( !m_projectTarget.compare("nes",Qt::CaseInsensitive) )
      {
         m_projectLinkerOutputName = m_projectTitle.toLower().replace(" ","_")+".prg";
         m_projectDebugInfoName = m_projectTitle.toLower().replace(" ","_")+".dbg";
         m_projectCHRROMOutputName = m_projectTitle.toLower().replace(" ","_")+".chr";
         m_projectCartridgeOutputName = m_projectTitle.toLower().replace(" ","_")+".nes";
         m_projectCartridgeSaveStateName = m_projectTitle.toLower().replace(" ","_")+".sav";
      }
      else if ( !m_projectTarget.contains("c64",Qt::CaseInsensitive) )
      {
         m_projectLinkerOutputName = m_projectTitle.toLower().replace(" ","_")+".c64";
         m_projectDebugInfoName = m_projectTitle.toLower().replace(" ","_")+".dbg";
         m_projectCHRROMOutputName = "";
         m_projectCartridgeOutputName = "";
         m_projectCartridgeSaveStateName = "";
      }
   }
   void setProjectAddOns(QStringList list) { m_projectAddOns = list; }
   void setProjectOutputBasePath(QString value)
   {
      if (!value.isEmpty()) m_projectOutputBasePath = value; else m_projectOutputBasePath=".";
      m_projectLinkerOutputBasePath = m_projectOutputBasePath;
      m_projectCHRROMOutputBasePath = m_projectOutputBasePath;
   }
   void setProjectOutputName(QString value) { m_projectOutputName = value; }
   void setProjectHeaderFileName(QString value) { m_projectHeaderFileName = value; }
   void setProjectSourceFileName(QString value) { m_projectSourceFileName = value; }
   void setProjectLinkerOutputBasePath(QString value) { m_projectLinkerOutputBasePath = value; }
   void setProjectLinkerOutputName(QString value) { m_projectLinkerOutputName = value; }
   void setProjectDebugInfoName(QString value) { m_projectDebugInfoName = value; }
   void setProjectCHRROMOutputBasePath(QString value) { m_projectCHRROMOutputBasePath = value; }
   void setProjectCHRROMOutputName(QString value) { m_projectCHRROMOutputName = value; }
   void setProjectUsesCHRROM(bool value) { m_projectUsesCHRROM = value; }
   void setProjectCartridgeOutputName(QString value) { m_projectCartridgeOutputName = value; }
   void setProjectCartridgeSaveStateName(QString value) { m_projectCartridgeSaveStateName = value; }
   void setCompilerDefinedSymbols(QString value) { m_compilerDefinedSymbols = value; }
   void setCompilerIncludePaths(QString value) { m_compilerIncludePaths = value; }
   void setCompilerAdditionalOptions(QString value) { m_compilerAdditionalOptions = value; }
   void setAssemblerDefinedSymbols(QString value) { m_assemblerDefinedSymbols = value; }
   void setAssemblerIncludePaths(QString value) { m_assemblerIncludePaths = value; }
   void setAssemblerAdditionalOptions(QString value) { m_assemblerAdditionalOptions = value; }
   void setLinkerConfigFile(QString value) { m_linkerConfigFile = value; }
   void setMakefileCustomRuleFiles(QStringList value) { m_makefileCustomRuleFiles = value; }
   void setLinkerAdditionalOptions(QString value) { m_linkerAdditionalOptions = value; }
   void setLinkerAdditionalDependencies(QString value) { m_linkerAdditionalDependencies = value; }
   void addSourceSearchPath(QString value);
   void removeSourceSearchPath(QString value);
   void setProjectPaletteEntries(QList<CPaletteEntry> *pProjectPaletteEntries);
   void setTileProperties(QList<PropertyItem> items) { m_tileProperties = items; }

   void setSaveStateDoc(QDomDocument doc) { m_saveStateDoc = doc; }

   // IXMLSerializable Interface Implementation
   virtual bool serialize(QDomDocument& doc, QDomNode& node);
   virtual bool deserialize(QDomDocument& doc, QDomNode& node, QString& errors);

   // IProjectTreeViewItem Interface Implmentation
   QString caption() const;
   virtual void openItemEvent(CProjectTabWidget*) {}
   virtual bool exportData();

   // ICenterWidgetItem Interface Implementation

private:
   static CNesicideProject *_instance;
   CNesicideProject();

   // Is the project initialized?
   bool m_isInitialized;
   bool m_isDirty;

   // Attributes
   // The visible title of the project
   QString m_projectFileName;
   QString m_projectTarget;
   QString m_projectTitle;
   QStringList m_projectAddOns;
   QString m_projectOutputBasePath;
   QString m_projectOutputName;
   QString m_projectHeaderFileName;
   QString m_projectSourceFileName;
   QString m_projectLinkerOutputBasePath;
   QString m_projectLinkerOutputName;
   QString m_projectDebugInfoName;
   QString m_projectCHRROMOutputBasePath;
   QString m_projectCHRROMOutputName;
   bool    m_projectUsesCHRROM;
   QString m_projectCartridgeOutputName;
   QString m_projectCartridgeSaveStateName;
   // The toolchain argument strings
   QString m_compilerDefinedSymbols;
   QString m_compilerIncludePaths;
   QString m_compilerAdditionalOptions;
   QString m_assemblerDefinedSymbols;
   QString m_assemblerIncludePaths;
   QString m_assemblerAdditionalOptions;
   QString m_linkerConfigFile;
   QStringList m_makefileCustomRuleFiles;
   QString m_linkerAdditionalOptions;
   QString m_linkerAdditionalDependencies;
   QStringList m_sourceSearchPaths;
   QList<PropertyItem> m_tileProperties;

   // Save state information
   QDomDocument m_saveStateDoc;

   // Contained children
   QList<CPaletteEntry> m_projectPaletteEntries;                 // List of palette entries for the emulator.
   CProject*            m_pProject;
   CCartridge*          m_pCartridge;

signals:
   void createTarget(QString target);
};

#endif // CNESICIDEPROJECT_H
