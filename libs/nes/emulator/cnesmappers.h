#if !defined ( MAPPERS_H )
#define MAPPERS_H

#include "nes_emulator_core.h"

typedef struct _Mapper001State
{
   uint8_t reg [ 4 ];
   uint8_t sr;
   uint8_t sel;
   uint8_t srCount;
   uint32_t          lastWriteAddr;
} Mapper001State;

typedef struct _Mapper004State
{
   uint8_t  reg [ 8 ];
   uint8_t  irqCounter;
   uint8_t  irqLatch;
   bool           irqEnable;
   uint8_t  prg [ 2 ];
   uint8_t  chr [ 8 ];
} Mapper004State;

typedef struct _Mapper009State
{
   uint8_t  reg [ 6 ];
   uint8_t  latch0;
   uint8_t  latch1;
   uint8_t  latch0FD;
   uint8_t  latch0FE;
   uint8_t  latch1FD;
   uint8_t  latch1FE;
} Mapper009State;

typedef struct _Mapper069State
{
   uint8_t  reg [ 4 ];
   uint16_t  irqCounter;
   bool           irqEnable;
   bool           irqCountEnable;
   uint8_t  prg [ 4 ];
   uint8_t  chr [ 8 ];
} Mapper069State;

typedef struct _Mapper075State
{
   uint8_t  reg [ 6 ];
   uint8_t  prg [ 4 ];
   uint8_t  chr [ 8 ];
} Mapper075State;

typedef struct _MapperState
{
   bool valid;
   uint32_t PRGROMOffset [ 4 ];
   uint32_t CHRMEMOffset [ 8 ];
   union
   {
      Mapper001State mapper001;
      Mapper004State mapper004;
      Mapper009State mapper009;
      Mapper069State mapper069;
      Mapper075State mapper075;
   } data;
} MapperState;

typedef void (*RESETFUNC)();
typedef uint32_t (*MAPPERRFUNC)(uint32_t addr);
typedef void (*MAPPERWFUNC)(uint32_t addr, uint8_t data);
typedef void (*SYNCPPUFUNC)(uint32_t ppuCycle, uint32_t ppuAddr);
typedef void (*SYNCCPUFUNC)(void);
typedef void (*LOADFUNC)(MapperState* data);
typedef void (*SAVEFUNC)(MapperState* data);

typedef struct _MapperFuncs
{
   RESETFUNC reset;
   MAPPERRFUNC highread;
   MAPPERWFUNC highwrite;
   MAPPERRFUNC lowread;
   MAPPERWFUNC lowwrite;
   SYNCPPUFUNC sync_ppu;
   SYNCCPUFUNC sync_cpu;
   LOADFUNC load;
   SAVEFUNC save;
   bool     remapPrg;
   bool     remapChr;
} MapperFuncs;

extern MapperFuncs mapperfunc[];

#endif
