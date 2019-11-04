#include "cbreakpointinfo.h"

CBreakpointInfo::CBreakpointInfo()
   : m_numBreakpoints(0)
{
}

CBreakpointInfo::~CBreakpointInfo()
{
}

void CBreakpointInfo::ToggleEnabled ( int bp )
{
   m_breakpoint [ bp ].enabled = !m_breakpoint [ bp ].enabled;
}

void CBreakpointInfo::SetEnabled ( int bp, bool enabled )
{
   m_breakpoint [ bp ].enabled = enabled;
}

int CBreakpointInfo::FindExactMatch ( int type, eBreakpointItemType itemType, int event, int item1, int item1Absolute, int item2, int mask, bool maskExclusive, eBreakpointConditionType conditionType, int condition, eBreakpointDataType dataType, int data )
{
   int bpfound = -1;
   int bp;

   for ( bp = 0; bp < m_numBreakpoints; bp++ )
   {
      if ( (m_breakpoint[bp].type == type) &&
           (m_breakpoint[bp].itemType == itemType) &&
           (m_breakpoint[bp].event == event) &&
           (m_breakpoint[bp].item1 == (uint32_t)item1) &&
           (m_breakpoint[bp].item1Absolute == (uint32_t)item1Absolute) &&
           (m_breakpoint[bp].item2 == (uint32_t)item2) &&
           (m_breakpoint[bp].itemMask == (uint32_t)mask) &&
           (m_breakpoint[bp].itemMaskExclusive == maskExclusive) &&
           (m_breakpoint[bp].conditionType == conditionType) &&
           (m_breakpoint[bp].condition == condition) &&
           (m_breakpoint[bp].dataType == dataType) &&
           (m_breakpoint[bp].data == data) )
      {
         bpfound = bp;
         break;
      }
   }

   return bpfound;
}

void CBreakpointInfo::ModifyBreakpoint ( int bp, BreakpointInfo* pBreakpoint )
{
   if ( bp < NUM_BREAKPOINTS )
   {
      ModifyBreakpoint(&(m_breakpoint[bp]),
                       pBreakpoint->type,
                       pBreakpoint->itemType,
                       pBreakpoint->event,
                       pBreakpoint->item1,
                       pBreakpoint->item1Absolute,
                       pBreakpoint->item2,
                       pBreakpoint->itemMask,
                       pBreakpoint->itemMaskExclusive,
                       pBreakpoint->conditionType,
                       pBreakpoint->condition,
                       pBreakpoint->dataType,
                       pBreakpoint->data,
                       pBreakpoint->enabled);
   }
}

void CBreakpointInfo::ConstructBreakpoint ( BreakpointInfo* pBreakpoint, int type, eBreakpointItemType itemType, int event, int item1, int item1Absolute, int item2, int mask, bool maskExclusive, eBreakpointConditionType conditionType, int condition, eBreakpointDataType dataType, int data, bool enabled )
{
   ModifyBreakpoint(pBreakpoint,type,itemType,event,item1,item1Absolute,item2,mask,maskExclusive,conditionType,condition,dataType,data,enabled);
}

int CBreakpointInfo::AddBreakpoint ( BreakpointInfo* pBreakpoint )
{
   int idx = m_numBreakpoints;
   if ( m_numBreakpoints < NUM_BREAKPOINTS )
   {
      ModifyBreakpoint ( &(m_breakpoint[m_numBreakpoints]),
                         pBreakpoint->type,
                         pBreakpoint->itemType,
                         pBreakpoint->event,
                         pBreakpoint->item1,
                         pBreakpoint->item1Absolute,
                         pBreakpoint->item2,
                         pBreakpoint->itemMask,
                         pBreakpoint->itemMaskExclusive,
                         pBreakpoint->conditionType,
                         pBreakpoint->condition,
                         pBreakpoint->dataType,
                         pBreakpoint->data,
                         pBreakpoint->enabled );
      m_numBreakpoints++;
   }
   else
   {
      idx = -1;
   }

   return idx;
}

int CBreakpointInfo::AddBreakpoint ( int type, eBreakpointItemType itemType, int event, int item1, int item1Absolute, int item2, int mask, bool maskExclusive, eBreakpointConditionType conditionType, int condition, eBreakpointDataType dataType, int data, bool enabled )
{
   int idx = m_numBreakpoints;
   if ( m_numBreakpoints < NUM_BREAKPOINTS )
   {
      ModifyBreakpoint ( &(m_breakpoint[m_numBreakpoints]),
                         type,
                         itemType,
                         event,
                         item1,
                         item1Absolute,
                         item2,
                         mask,
                         maskExclusive,
                         conditionType,
                         condition,
                         dataType,
                         data,
                         enabled );
      m_numBreakpoints++;
   }
   else
   {
      idx = -1;
   }

   return idx;
}

void CBreakpointInfo::RemoveBreakpoint ( int index )
{
   int idx;

   for ( idx = index; idx < NUM_BREAKPOINTS-1; idx++ )
   {
      m_breakpoint [ idx ].type = m_breakpoint [ idx+1 ].type;
      m_breakpoint [ idx ].enabled = m_breakpoint [ idx+1 ].enabled;
      m_breakpoint [ idx ].target = m_breakpoint [ idx+1 ].target;
      m_breakpoint [ idx ].itemType = m_breakpoint [ idx+1 ].itemType;
      m_breakpoint [ idx ].event = m_breakpoint [ idx+1 ].event;
      m_breakpoint [ idx ].pEvent = m_breakpoint [ idx+1 ].pEvent;
      m_breakpoint [ idx ].item1 = m_breakpoint [ idx+1 ].item1;
      m_breakpoint [ idx ].item1Absolute = m_breakpoint [ idx+1 ].item1Absolute;
      m_breakpoint [ idx ].item2 = m_breakpoint [ idx+1 ].item2;
      m_breakpoint [ idx ].itemMask = m_breakpoint [ idx+1 ].itemMask;
      m_breakpoint [ idx ].itemMaskExclusive = m_breakpoint [ idx+1 ].itemMaskExclusive;
      m_breakpoint [ idx ].itemActual = m_breakpoint [ idx+1 ].itemActual;
      m_breakpoint [ idx ].conditionType = m_breakpoint [ idx+1 ].conditionType;
      m_breakpoint [ idx ].condition = m_breakpoint [ idx+1 ].condition;
      m_breakpoint [ idx ].dataType = m_breakpoint [ idx+1 ].dataType;
      m_breakpoint [ idx ].data = m_breakpoint [ idx+1 ].data;
      m_breakpoint [ idx ].hit = m_breakpoint [ idx+1 ].hit;
   }

   m_numBreakpoints--;
}

BreakpointStatus CBreakpointInfo::GetStatus ( int idx )
{
   if ( (m_breakpoint[idx].enabled) && (m_breakpoint[idx].hit) )
   {
      return Breakpoint_Hit;
   }
   else if ( m_breakpoint[idx].enabled )
   {
      return Breakpoint_Idle;
   }
   else if ( !(m_breakpoint[idx].enabled) )
   {
      return Breakpoint_Disabled;
   }
   else
   {
      return Breakpoint_Disabled;
   }
}
