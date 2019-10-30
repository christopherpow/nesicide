#ifndef CNESBREAKPOINTINFO_H
#define CNESBREAKPOINTINFO_H

#include "cbreakpointinfo.h"

class CNESBreakpointInfo : public CBreakpointInfo
{
public:
   CNESBreakpointInfo();
   virtual ~CNESBreakpointInfo();
   void GetPrintable ( int idx, char* msg );
   void GetHitPrintable ( int idx, char* hmsg );

protected:
   void ModifyBreakpoint ( BreakpointInfo* pBreakpoint, int type, eBreakpointItemType itemType, int event, int item1, int item1Absolute, int item2, int mask, bool maskExclusive, eBreakpointConditionType conditionType, int condition, eBreakpointDataType dataType, int data, bool enabled );
};

#endif // CBREAKPOINTINFO_H
