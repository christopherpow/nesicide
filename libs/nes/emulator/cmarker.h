#ifndef CMARKER_H
#define CMARKER_H

#include <stdint.h>

#define MAX_MARKER_SETS 8

#define MARKER_NOT_MARKED 0xFFFFFFFF

typedef enum
{
   eMarkerSet_Invalid = 0,
   eMarkerSet_Started,
   eMarkerSet_Complete
} eMarkerSet_State;

typedef struct _MarkerSetInfo
{
   eMarkerSet_State state;
   uint32_t         startAddr;
   uint32_t         startAbsAddr;
   uint32_t         endAddr;
   uint32_t         endAbsAddr;
   uint32_t         startCpuCycle;
   uint32_t         startPpuFrame;
   uint32_t         startPpuCycle;
   uint32_t         endCpuCycle;
   uint32_t         endPpuFrame;
   uint32_t         endPpuCycle;
   uint8_t          red;
   uint8_t          green;
   uint8_t          blue;
   uint32_t         minCpuCycles;
   uint32_t         maxCpuCycles;
   uint32_t         curCpuCycles;
} MarkerSetInfo;

class CMarker
{
public:
   CMarker();
   int32_t GetNumMarkers(void) const
   {
      return MAX_MARKER_SETS;
   }
   MarkerSetInfo* GetMarker(int32_t marker)
   {
      return m_marker+marker;
   }
   int AddMarker(uint32_t addr,uint32_t absAddr);
   int AddSpecificMarker(int32_t marker,uint32_t addr,uint32_t absAddr);
   int FindInProgressMarker(void);
   void RemoveMarker(int32_t marker);
   void RemoveAllMarkers(void);
   void ZeroMarker(int32_t marker);
   void ZeroAllMarkers(void);
   void CompleteMarker(int32_t marker,uint32_t addr,uint32_t absAddr);
   void UpdateMarkers(uint32_t absAddr,uint32_t cpuCycle,uint32_t ppuFrame,uint32_t ppuCycle);

protected:
   MarkerSetInfo m_marker [ MAX_MARKER_SETS ];
};

#endif // CMARKER_H
