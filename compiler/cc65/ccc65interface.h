#ifndef CCC65INTERFACE_H
#define CCC65INTERFACE_H

#include <QProcess>

#include "iprojecttreeviewitem.h"

#include "dbginfo.h"

class CCC65Interface : public QObject
{
   Q_OBJECT
public:
   CCC65Interface();
   virtual ~CCC65Interface();
   static bool assemble();
   static bool captureDebugInfo();
   static bool captureINESImage();
   static QStringList& getSourceFiles();
   static QStringList& getSymbolsForSourceFile(QString& sourceFile);
   static QString getSourceFileFromAbsoluteAddress(uint32_t addr,uint32_t absAddr);
   static int getSourceLineFromAbsoluteAddress(uint32_t addr,uint32_t absAddr);
   static unsigned int getAddressFromFileAndLine(QString file,int line);
   static unsigned int getAbsoluteAddressFromFileAndLine(QString file,int line);

protected:
   static cc65_dbginfo        dbgInfo;
   static cc65_sourceinfo*    dbgSources;
   static cc65_segmentinfo*   dbgSegments;
   static cc65_lineinfo*      dbgLines;
   static cc65_symbolinfo*    dbgSymbols;

   static QStringList         files;
};

#endif // CCC65INTERFACE_H
