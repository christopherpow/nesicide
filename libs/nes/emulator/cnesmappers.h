#if !defined ( MAPPERS_H )
#define MAPPERS_H

#include "nes_emulator_core.h"

typedef void (*RESETFUNC)(bool soft);
typedef uint32_t (*MAPPERRFUNC)(uint32_t addr);
typedef void (*MAPPERWFUNC)(uint32_t addr, uint8_t data);
typedef void (*SYNCPPUFUNC)(uint32_t ppuCycle, uint32_t ppuAddr);
typedef void (*SYNCCPUFUNC)(void);

typedef struct _MapperFuncs
{
   RESETFUNC reset;
   MAPPERRFUNC highread;
   MAPPERWFUNC highwrite;
   MAPPERRFUNC lowread;
   MAPPERWFUNC lowwrite;
   SYNCPPUFUNC sync_ppu;
   SYNCCPUFUNC sync_cpu;
   MAPPERRFUNC debuginfo;
   bool     remapPrg;
   bool     remapChr;
} MapperFuncs;

extern MapperFuncs mapperfunc[];

#endif
