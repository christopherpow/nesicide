#ifndef CSOURCEASSEMBLER_H
#define CSOURCEASSEMBLER_H

#include <QString>
#include "csources.h"
#include "csourceitem.h"
#include "cprgrombank.h"
#include "cbinaryfiles.h"
#include "cbinaryfile.h"

class CSourceAssembler
{
public:
   CSourceAssembler();
   bool assemble();
   void clean();
};

#endif // CSOURCEASSEMBLER_H
