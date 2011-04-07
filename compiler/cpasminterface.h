#ifndef CPASMINTERFACE_H
#define CPASMINTERFACE_H

#include <QStringList>

#include "main.h"

class CPASMInterface : public QObject
{
   Q_OBJECT
public:
   CPASMInterface();
   virtual ~CPASMInterface();
   static bool assemble();
   static bool captureDebugInfo();
   static bool captureINESImage();
   static QStringList& getSourceFiles();
   static QStringList& getSymbolsForSourceFile(QString& sourceFile);

protected:
   static QStringList         files;
};

#endif // CPASMINTERFACE_H
