#ifndef CCC65INTERFACE_H
#define CCC65INTERFACE_H

#include <QProcess>

#include "stdint.h"

#include "dbginfo.h"

class CCC65Interface : public QObject
{
   Q_OBJECT
public:
   // Class maintenance.
   CCC65Interface();
   virtual ~CCC65Interface();
   static void clear();

   // Makefile and target image APIs.
   static bool createMakefile();
   static void clean();
   static bool assemble();
   static bool captureDebugInfo();
   static bool isBuildUpToDate();
   static bool captureINESImage();
   static QStringList getCLanguageSourcesFromProject();
   static QStringList getAssemblerSourcesFromProject();
   static QStringList getCustomSourcesFromProject();
   static void updateTargetMachine(QString target);

   // Debug information parsing/extending APIs.
   static QStringList getSourceFiles();
   static uint32_t getSegmentBase(QString segment);
   static unsigned int getSourceFileModificationTime(QString sourceFile);
   static QStringList getSymbolsForSourceFile(QString sourceFile);
   static int getSymbolMatchCount(QString symbol);
   static cc65_symbol_type getSymbolType(QString symbol,int index = 0);
   static unsigned int getSymbolAddress(QString symbol,int index = 0);
   static unsigned int getSymbolSegment(QString symbol,int index = 0);
   static QString getSymbolSegmentName(QString symbol, int index = 0);
   static unsigned int getSymbolIndexFromSegment(QString symbol,int segment);
   static unsigned int getSymbolSize(QString symbol,int index = 0);
   static int getSourceLineFromFileAndSymbol(QString file,QString symbol);
   static QString getSourceFileFromSymbol(QString symbol);
   static int getLineMatchCount(QString file,int source_line);
   static unsigned int getAddressFromFileAndLine(QString file,int source_line,int entry = -1);
   static QStringList getErrors() { return errors; }
   static bool isErrorOnLineOfFile(QString file,int source_line);
   static bool isStringASymbol(QString string);

   // Target-dependent launchpads.
   static QString getSourceFileFromAbsoluteAddress(uint32_t addr,uint32_t absAddr);
   static int getSourceLineFromAbsoluteAddress(uint32_t addr,uint32_t absAddr);
   static unsigned int getAbsoluteAddressFromFileAndLine(QString file,int source_line,int entry = -1);
   static unsigned int getEndAddressFromAbsoluteAddress(uint32_t addr,uint32_t absAddr);
   static bool isAbsoluteAddressAnOpcode(uint32_t absAddr);
   static unsigned int getSymbolAbsoluteAddress(QString symbol,int index = 0);

   // NES target-dependent APIs.
   static QString nesGetSourceFileFromAbsoluteAddress(uint32_t addr,uint32_t absAddr);
   static int nesGetSourceLineFromAbsoluteAddress(uint32_t addr,uint32_t absAddr);
   static unsigned int nesGetAbsoluteAddressFromFileAndLine(QString file,int source_line,int entry = -1);
   static unsigned int nesGetEndAddressFromAbsoluteAddress(uint32_t addr,uint32_t absAddr);
   static bool nesIsAbsoluteAddressAnOpcode(uint32_t absAddr);
   static unsigned int nesGetSymbolAbsoluteAddress(QString symbol,int index = 0);

   // C64 target-dependent APIs.
   static QString c64GetSourceFileFromAbsoluteAddress(uint32_t addr,uint32_t absAddr);
   static int c64GetSourceLineFromAbsoluteAddress(uint32_t addr,uint32_t absAddr);
   static unsigned int c64GetAbsoluteAddressFromFileAndLine(QString file,int source_line,int entry = -1);
   static unsigned int c64GetEndAddressFromAbsoluteAddress(uint32_t addr,uint32_t absAddr);
   static bool c64IsAbsoluteAddressAnOpcode(uint32_t absAddr);
   static unsigned int c64GetSymbolAbsoluteAddress(QString symbol,int index = 0);

protected:
   static cc65_dbginfo        dbgInfo;
   static QStringList         errors;
   static QString             targetMachine;
};

#endif // CCC65INTERFACE_H
