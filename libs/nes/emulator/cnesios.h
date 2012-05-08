#if !defined ( IOS_H )
#define IOS_H

#include "nes_emulator_core.h"

typedef uint32_t (*IORFUNC)(uint32_t addr);
typedef void (*IOWFUNC)(uint32_t addr, uint8_t data);
typedef void (*SPECIALFUNC)(int32_t port,int32_t special);

typedef struct _IOFuncs
{
   IORFUNC      emuread;
   IOWFUNC      emuwrite;
   IORFUNC      dbgread;
   IOWFUNC      dbgwrite;
   SPECIALFUNC  special;
} IOFuncs;

extern IOFuncs iofunc[];

#endif // #if !defined ( IOS_H )
