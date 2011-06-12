#ifndef CGRAPHICSASSEMBLER_H
#define CGRAPHICSASSEMBLER_H

#include "cchrrombank.h"
#include "cbuildertextlogger.h"

class CGraphicsAssembler
{
public:
   CGraphicsAssembler();
   bool assemble();
   void clean();
};

#endif // CGRAPHICSASSEMBLER_H
