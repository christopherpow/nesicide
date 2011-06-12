#ifndef CCARTRIDGEBUILDER_H
#define CCARTRIDGEBUILDER_H

#include "cbuildertextlogger.h"
#include "csourceassembler.h"
#include "cgraphicsassembler.h"

class CCartridgeBuilder
{
public:
   CCartridgeBuilder();
   bool build();
   void clean();
private:
};

#endif // CCARTRIDGEBUILDER_H
