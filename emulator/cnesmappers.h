#if !defined ( MAPPERS_H )
#define MAPPERS_H

#include "cnesicidecommon.h"

typedef struct _Mapper001State
{
   unsigned char reg [ 4 ];
   unsigned char sr;
   unsigned char sel;
   unsigned char srCount;
   UINT          lastWriteAddr;
} Mapper001State;

typedef struct _Mapper004State
{
   unsigned char  reg [ 8 ];
   unsigned char  irqCounter;
   unsigned char  irqLatch;
   bool           irqEnable;
   unsigned char  prg [ 2 ];
   unsigned char  chr [ 8 ];
} Mapper004State;

typedef struct _Mapper009State
{
   unsigned char  reg [ 6 ];
   unsigned char  latch0;
   unsigned char  latch1;
   unsigned char  latch0FD;
   unsigned char  latch0FE;
   unsigned char  latch1FD;
   unsigned char  latch1FE;
} Mapper009State;

typedef struct _MapperState
{
   bool valid;
   UINT PRGROMOffset [ 4 ];
   UINT CHRMEMOffset [ 8 ];
   union 
   {
      Mapper001State mapper001;
      Mapper004State mapper004;
      Mapper009State mapper009;
   } data;
} MapperState;

typedef void (*RESETFUNC)();
typedef UINT (*MAPPERRFUNC)(UINT addr);
typedef void (*MAPPERWFUNC)(UINT addr, unsigned char data);
typedef bool (*SYNCHFUNC)(int raster);
typedef bool (*SYNCVFUNC)(void);
typedef void (*LOADFUNC)(MapperState* data);
typedef void (*SAVEFUNC)(MapperState* data);
typedef void (*LATCHFUNC)(UINT addr);
typedef void (*DISPLAYFUNC)(char* sz);

typedef struct _MapperInfo
{
   const char* name;
   int   id;
} MapperInfo;

typedef struct _MapperFuncs
{
   RESETFUNC reset;
   MAPPERRFUNC highread;
   MAPPERWFUNC highwrite;
   MAPPERRFUNC lowread;
   MAPPERWFUNC lowwrite;
   SYNCHFUNC synch;
   SYNCVFUNC syncv;
   LATCHFUNC latch;
   LOADFUNC load;
   SAVEFUNC save;
   DISPLAYFUNC display;
   MAPPERRFUNC internalread;
} MapperFuncs;

extern MapperInfo  mapper[];
extern MapperFuncs mapperfunc[];

#endif
