#ifndef CMACHINEIMAGEBUILDER_H
#define CMACHINEIMAGEBUILDER_H

#include "cbuildertextlogger.h"
#include "csourceassembler.h"

class CMachineImageBuilder
{
public:
   CMachineImageBuilder();
   bool build();
   void clean();
private:
};

#endif // CMACHINEIMAGEBUILDER_H
