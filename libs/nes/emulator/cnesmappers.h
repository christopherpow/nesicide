#if !defined ( MAPPERS_H )
#define MAPPERS_H

#include "nes_emulator_core.h"

typedef void (*RESETFUNC)(bool soft);
typedef uint32_t (*MAPPERRFUNC)(uint32_t addr);
typedef void (*MAPPERWFUNC)(uint32_t addr, uint8_t data);
typedef void (*SYNCPPUFUNC)(uint32_t ppuCycle, uint32_t ppuAddr);
typedef void (*SYNCCPUFUNC)(void);
typedef uint16_t (*SOUNDFUNC)(void);
typedef void (*SOUNDENAFUNC)(uint32_t mask);

typedef struct _MapperFuncs
{
   RESETFUNC    reset;
   MAPPERRFUNC  highread;
   MAPPERWFUNC  highwrite;
   MAPPERRFUNC  lowread;
   MAPPERWFUNC  lowwrite;
   SYNCPPUFUNC  sync_ppu;
   SYNCCPUFUNC  sync_cpu;
   MAPPERRFUNC  debuginfo;
   SOUNDFUNC    amplitude;
   SOUNDENAFUNC soundenable;
   bool     remapPrg;
   bool     remapChr;
   bool     remapVram;
   uint32_t vramSize;
} MapperFuncs;

extern MapperFuncs _mapperfunc[];

extern MapperFuncs* MAPPERFUNC;

#endif
