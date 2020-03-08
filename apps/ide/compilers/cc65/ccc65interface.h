#ifndef CCC65INTERFACE_H
#define CCC65INTERFACE_H

#include <QProcess>
#include <QMutex>

#include "stdint.h"

#include "dbginfo.h"

class CCC65Interface : public QObject
{
   Q_OBJECT
public:
   static CCC65Interface *_instance;
   static CCC65Interface *instance()
   {
      if ( !_instance )
      {
         _instance = new CCC65Interface();
      }
      return _instance;
   }
   virtual ~CCC65Interface();
   void clear();

   // Makefile and target image APIs.
   bool createMakefile();
   void clean();
   bool assemble();
   bool captureDebugInfo();
   bool isBuildUpToDate();
   bool captureINESImage();
   QStringList getCLanguageSourcesFromProject();
   QStringList getAssemblerSourcesFromProject();
   QStringList getCustomSourcesFromProject();
   void updateTargetMachine(QString target);

   // Debug information parsing/extending APIs.
   QStringList getSourceFiles();
   uint32_t getSegmentBase(QString segment);
   unsigned int getSourceFileModificationTime(QString sourceFile);
   QStringList getSymbolsForSourceFile(QString sourceFile);
   int getSymbolMatchCount(QString symbol);
   cc65_symbol_type getSymbolType(QString symbol,int index = 0);
   unsigned int getSymbolAddress(QString symbol,int index = 0);
   unsigned int getSymbolSegment(QString symbol,int index = 0);
   QString getSymbolSegmentName(QString symbol, int index = 0);
   unsigned int getSymbolIndexFromSegment(QString symbol,int segment);
   unsigned int getSymbolSize(QString symbol,int index = 0);
   int getSourceLineFromFileAndSymbol(QString file,QString symbol);
   QString getSourceFileFromSymbol(QString symbol);
   int getLineMatchCount(QString file,int source_line);
   unsigned int getAddressFromFileAndLine(QString file,int source_line,int entry = -1);
   QStringList getErrors() { return errors; }
   bool isErrorOnLineOfFile(QString file,int source_line);
   bool isStringASymbol(QString string);

   // Target-independent launchpads.
   QString getSourceFileFromPhysicalAddress(uint32_t addr,uint32_t absAddr);
   int getSourceLineFromPhysicalAddress(uint32_t addr,uint32_t absAddr);
   unsigned int getPhysicalAddressFromFileAndLine(QString file,int source_line,int entry = -1);
   unsigned int getEndAddressFromPhysicalAddress(uint32_t addr,uint32_t absAddr);
   bool isPhysicalAddressAnOpcode(uint32_t absAddr);
   unsigned int getSymbolPhysicalAddress(QString symbol,int index = 0);

   // NES target-dependent APIs.
   QString nesGetSourceFileFromPhysicalAddress(uint32_t addr,uint32_t absAddr);
   int nesGetSourceLineFromPhysicalAddress(uint32_t addr,uint32_t absAddr);
   unsigned int nesGetPhysicalAddressFromFileAndLine(QString file,int source_line,int entry = -1);
   unsigned int nesGetEndAddressFromPhysicalAddress(uint32_t addr,uint32_t absAddr);
   bool nesIsPhysicalAddressAnOpcode(uint32_t absAddr);
   unsigned int nesGetSymbolPhysicalAddress(QString symbol,int index = 0);

   // C64 target-dependent APIs.
   QString c64GetSourceFileFromPhysicalAddress(uint32_t addr,uint32_t absAddr);
   int c64GetSourceLineFromPhysicalAddress(uint32_t addr,uint32_t absAddr);
   unsigned int c64GetPhysicalAddressFromFileAndLine(QString file,int source_line,int entry = -1);
   unsigned int c64GetEndAddressFromPhysicalAddress(uint32_t addr,uint32_t absAddr);
   bool c64IsPhysicalAddressAnOpcode(uint32_t absAddr);
   unsigned int c64GetSymbolPhysicalAddress(QString symbol,int index = 0);

public slots:
   void process_errorOccurred(QProcess::ProcessError error);
   void process_finished(int exitCode, QProcess::ExitStatus exitStatus);
   void process_readyReadStandardError();
   void process_readyReadStandardOutput();
   void process_started();
   void process_stateChanged(QProcess::ProcessState newState);

protected:
   cc65_dbginfo        dbgInfo;
   QStringList         errors;
   QString             targetMachine;
   QMutex              protect;
   QProcess            process;

private:
   CCC65Interface();
};

#endif // CCC65INTERFACE_H
