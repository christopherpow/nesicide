#ifndef CC64BREAKPOINTINFO_H
#define CC64BREAKPOINTINFO_H

#include "cbreakpointinfo.h"

class CC64BreakpointInfo : public CBreakpointInfo
{
public:
   CC64BreakpointInfo();
   void GetPrintable ( int idx, char* msg );
   void GetHitPrintable ( int idx, char* hmsg );

protected:
   void ModifyBreakpoint ( BreakpointInfo* pBreakpoint, int type, eBreakpointItemType itemType, int event, int item1, int item1Physical, int item2, int mask, bool maskExclusive, eBreakpointConditionType conditionType, int condition, eBreakpointDataType dataType, int data, bool enabled );
};

#endif // CBREAKPOINTINFO_H
