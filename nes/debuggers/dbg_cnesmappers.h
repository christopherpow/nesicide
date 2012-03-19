#if !defined ( MAPPERS_H )
#define MAPPERS_H

#include "emulator_core.h"

typedef struct _MapperInfo
{
   const char* name;
   int32_t     id;
   int32_t     inspectorPage;
} MapperInfo;

extern MapperInfo  mapper[];
const char* mapperNameFromID ( int32_t id );
int32_t     mapperInspectorPageFromID ( int32_t id );
int mapperIndexFromID ( int32_t id );
int mapperIDFromIndex ( int32_t index );

#endif
