#include "cbreakpointinfo.h"
#include "cnesicidecommon.h"

#include "cnes6502.h"
#include "cnesppu.h"
#include "cnesapu.h"

CBreakpointInfo::CBreakpointInfo()
   : m_numBreakpoints(0)
{
}

void CBreakpointInfo::ToggleEnabled ( int bp )
{
   m_breakpoint [ bp ].enabled = !m_breakpoint [ bp ].enabled;
}

void CBreakpointInfo::ModifyBreakpoint ( int bp, eBreakpointType type, eBreakpointItemType itemType, int event, int item1, int item2, eBreakpointConditionType conditionType, int condition, eBreakpointDataType dataType, int data )
{
   if ( bp < NUM_BREAKPOINTS )
   {
      m_breakpoint [ bp ].hit = false;
      m_breakpoint [ bp ].enabled = true;
      m_breakpoint [ bp ].itemActual = -1;
      m_breakpoint [ bp ].type = (eBreakpointType)type;
      m_breakpoint [ bp ].pEvent = NULL;
      switch ( type )
      {
         case eBreakOnCPUExecution:
         case eBreakOnCPUMemoryAccess:
         case eBreakOnCPUMemoryRead:
         case eBreakOnCPUMemoryWrite:
         case eBreakOnCPUState:
         case eBreakOnCPUEvent:
            m_breakpoint [ bp ].target = eBreakInCPU;
         break;
         case eBreakOnPPUFetch:
         case eBreakOnOAMPortalAccess:
         case eBreakOnOAMPortalRead:
         case eBreakOnOAMPortalWrite:
         case eBreakOnPPUPortalAccess:
         case eBreakOnPPUPortalRead:
         case eBreakOnPPUPortalWrite:
         case eBreakOnPPUState:
         case eBreakOnPPUEvent:
            m_breakpoint [ bp ].target = eBreakInPPU;
         break;
         case eBreakOnAPUState:
         case eBreakOnAPUEvent:
            m_breakpoint [ bp ].target = eBreakInAPU;
         break;
         case eBreakOnMapperState:
         case eBreakOnMapperEvent:
            m_breakpoint [ bp ].target = eBreakInMapper;
         break;
      }
      m_breakpoint [ bp ].conditionType = conditionType;
      m_breakpoint [ bp ].condition = condition;
      m_breakpoint [ bp ].itemType = itemType;
      m_breakpoint [ bp ].event = event;
      if ( event >= 0 )
      {
         switch ( m_breakpoint [ bp ].target )
         {
            case eBreakInCPU:
            break;
            case eBreakInPPU:
               m_breakpoint [ bp ].pEvent = CPPU::BREAKPOINTEVENTS()[event];
            break;
            case eBreakInAPU:
            break;
            case eBreakInMapper:
            break;
         }
      }
      m_breakpoint [ bp ].item1 = item1;
      m_breakpoint [ bp ].item2 = item2;
      m_breakpoint [ bp ].dataType = dataType;
      m_breakpoint [ bp ].data = data;
   }
}

void CBreakpointInfo::AddBreakpoint ( eBreakpointType type, eBreakpointItemType itemType, int event, int item1, int item2, eBreakpointConditionType conditionType, int condition, eBreakpointDataType dataType, int data )
{
   if ( m_numBreakpoints < NUM_BREAKPOINTS )
   {
      ModifyBreakpoint ( m_numBreakpoints,
                         type,
                         itemType,
                         event,
                         item1,
                         item2,
                         conditionType,
                         condition,
                         dataType,
                         data );
      m_numBreakpoints++;
   }
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
      m_breakpoint [ idx ].item2 = m_breakpoint [ idx+1 ].item2;
      m_breakpoint [ idx ].itemActual = m_breakpoint [ idx+1 ].itemActual;
      m_breakpoint [ idx ].conditionType = m_breakpoint [ idx+1 ].conditionType;
      m_breakpoint [ idx ].condition = m_breakpoint [ idx+1 ].condition;
      m_breakpoint [ idx ].dataType = m_breakpoint [ idx+1 ].dataType;
      m_breakpoint [ idx ].data = m_breakpoint [ idx+1 ].data;
      m_breakpoint [ idx ].hit = m_breakpoint [ idx+1 ].hit;
   }
   m_numBreakpoints--;
}

void CBreakpointInfo::GetStatus ( int idx, char* msg )
{
   if ( (m_breakpoint[idx].enabled) && (m_breakpoint[idx].hit) )
   {
      sprintf ( msg, "*" );
   }
   else if ( m_breakpoint[idx].enabled )
   {
      sprintf ( msg, " " );
   }
   else if ( !(m_breakpoint[idx].enabled) )
   {
      sprintf ( msg, "X" );
   }
}

void CBreakpointInfo::GetPrintable ( int idx, char *msg )
{
   CRegisterData* pRegister;
   CBitfieldData* pBitfield;

   switch ( m_breakpoint[idx].type )
   {
      case eBreakOnCPUExecution:
         if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
         {
            sprintf ( msg, "Break if CPU executes at $%04X",
                      m_breakpoint[idx].item1 );
         }
         else
         {
            sprintf ( msg, "Break if CPU executes between $%04X and $%04X",
                      m_breakpoint[idx].item1,
                      m_breakpoint[idx].item2 );
         }
      break;
      case eBreakOnCPUMemoryAccess:
         switch ( m_breakpoint[idx].condition )
         {
            case eBreakIfAnything:
               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads or writes anything at address $%04X",
                                   m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads or writes anything between address $%04X and $%04X",
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }
            break;
            case eBreakIfEqual:
               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads or writes $%02X at address $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads or writes $%02X between address $%04X and $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }
            break;
            case eBreakIfNotEqual:
               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads or writes anything but $%02X at address $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads or writes anything but $%02X between address $%04X and $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }
            break;
            case eBreakIfGreaterThan:
               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads or writes greater than $%02X at address $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads or writes greater than $%02X between address $%04X and $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }
            break;
            case eBreakIfLessThan:
               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads or writes less than $%02X at address $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads or writes less than $%02X between address $%04X and $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }
            break;
         }
      break;
      case eBreakOnCPUMemoryRead:
         switch ( m_breakpoint[idx].condition )
         {
            case eBreakIfAnything:
               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads anything at address $%04X",
                                   m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads anything between address $%04X and $%04X",
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }
            break;
            case eBreakIfEqual:
               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads $%02X at address $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads $%02X between address $%04X and $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }
            break;
            case eBreakIfNotEqual:
               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads anything but $%02X at address $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads anything but $%02X between address $%04X and $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }
            break;
            case eBreakIfGreaterThan:
               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads greater than $%02X at address $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads greater than $%02X between address $%04X and $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }
            break;
            case eBreakIfLessThan:
               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads less than $%02X at address $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads less than $%02X between address $%04X and $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }
            break;
         }
      break;
      case eBreakOnCPUMemoryWrite:
         switch ( m_breakpoint[idx].condition )
         {
            case eBreakIfAnything:
               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU writes anything at address $%04X",
                                   m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU writes anything between address $%04X and $%04X",
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }
            break;
            case eBreakIfEqual:
               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU writes $%02X at address $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU writes $%02X between address $%04X and $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }
            break;
            case eBreakIfNotEqual:
               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU writes anything but $%02X at address $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU writes anything but $%02X between address $%04X and $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }
            break;
            case eBreakIfGreaterThan:
               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU writes greater than $%02X at address $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU writes greater than $%02X between address $%04X and $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }
            break;
            case eBreakIfLessThan:
               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU writes less than $%02X at address $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU writes less than $%02X between address $%04X and $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }
            break;
         }
      break;
      case eBreakOnPPUPortalAccess:
         switch ( m_breakpoint[idx].condition )
         {
            case eBreakIfAnything:
               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads or writes anything at PPU address $%04X",
                                   m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads or writes anything at PPU address between $%04X and $%04X",
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }
            break;
            case eBreakIfEqual:
               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads or writes $%02X at PPU address $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads or writes $%02X between PPU address $%04X and $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }
            break;
            case eBreakIfNotEqual:
               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads or writes anything but $%02X at PPU address $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads or writes anything but $%02X between PPU address $%04X and $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }
            break;
            case eBreakIfGreaterThan:
               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads or writes greater than $%02X at PPU address $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads or writes greater than $%02X between PPU address $%04X and $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }
            break;
            case eBreakIfLessThan:
               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads or writes less than $%02X at PPU address $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads or writes less than $%02X between PPU address $%04X and $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }
            break;
         }
      break;
      case eBreakOnPPUPortalRead:
         switch ( m_breakpoint[idx].condition )
         {
            case eBreakIfAnything:
               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads anything at PPU address $%04X",
                                   m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads anything between PPU address $%04X and $%04X",
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }
            break;
            case eBreakIfEqual:
               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads $%02X at PPU address $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads $%02X between PPU address $%04X and $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }
            break;
            case eBreakIfNotEqual:
               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads anything but $%02X at PPU address $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads anything but $%02X between PPU address $%04X and $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }
            break;
            case eBreakIfGreaterThan:
               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads greater than $%02X at PPU address $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads greater than $%02X between PPU address $%04X and $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }
            break;
            case eBreakIfLessThan:
               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads less than $%02X at PPU address $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads less than $%02X between PPU address $%04X and $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }
            break;
         }
      break;
      case eBreakOnPPUPortalWrite:
         switch ( m_breakpoint[idx].condition )
         {
            case eBreakIfAnything:
               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU writes anything at PPU address $%04X",
                                   m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU writes anything between PPU address $%04X and $%04X",
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }
            break;
            case eBreakIfEqual:
               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU writes $%02X at PPU address $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU writes $%02X between PPU address $%04X and $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }
            break;
            case eBreakIfNotEqual:
               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU writes anything but $%02X at PPU address $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU writes anything but $%02X between PPU address $%04X and $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }
            break;
            case eBreakIfGreaterThan:
               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU writes greater than $%02X at PPU address $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU writes greater than $%02X between PPU address $%04X and $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }
            break;
            case eBreakIfLessThan:
               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU writes less than $%02X at PPU address $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU writes less than $%02X between PPU address $%04X and $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }
            break;
         }
      break;
      case eBreakOnOAMPortalAccess:
         switch ( m_breakpoint[idx].condition )
         {
            case eBreakIfAnything:
               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads or writes anything at PPU OAM address $%04X",
                                   m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads or writes anything at PPU OAM address between $%04X and $%04X",
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }
            break;
            case eBreakIfEqual:
               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads or writes $%02X at PPU OAM address $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads or writes $%02X between PPU OAM address $%04X and $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }
            break;
            case eBreakIfNotEqual:
               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads or writes anything but $%02X at PPU OAM address $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads or writes anything but $%02X between PPU OAM address $%04X and $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }
            break;
            case eBreakIfGreaterThan:
               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads or writes greater than $%02X at PPU OAM address $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads or writes greater than $%02X between PPU OAM address $%04X and $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }
            break;
            case eBreakIfLessThan:
               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads or writes less than $%02X at PPU OAM address $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads or writes less than $%02X between PPU OAM address $%04X and $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }
            break;
         }
      break;
      case eBreakOnOAMPortalRead:
         switch ( m_breakpoint[idx].condition )
         {
            case eBreakIfAnything:
               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads anything at PPU OAM address $%04X",
                                   m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads anything between PPU OAM address $%04X and $%04X",
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }
            break;
            case eBreakIfEqual:
               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads $%02X at PPU OAM address $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads $%02X between PPU OAM address $%04X and $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }
            break;
            case eBreakIfNotEqual:
               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads anything but $%02X at PPU OAM address $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads anything but $%02X between PPU OAM address $%04X and $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }
            break;
            case eBreakIfGreaterThan:
               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads greater than $%02X at PPU OAM address $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads greater than $%02X between PPU OAM address $%04X and $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }
            break;
            case eBreakIfLessThan:
               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads less than $%02X at PPU OAM address $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads less than $%02X between PPU OAM address $%04X and $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }
            break;
         }
      break;
      case eBreakOnOAMPortalWrite:
         switch ( m_breakpoint[idx].condition )
         {
            case eBreakIfAnything:
               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU writes anything at PPU OAM address $%04X",
                                   m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU writes anything between PPU OAM address $%04X and $%04X",
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }
            break;
            case eBreakIfEqual:
               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU writes $%02X at PPU OAM address $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU writes $%02X between PPU OAM address $%04X and $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }
            break;
            case eBreakIfNotEqual:
               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU writes anything but $%02X at PPU OAM address $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU writes anything but $%02X between PPU OAM address $%04X and $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }
            break;
            case eBreakIfGreaterThan:
               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU writes greater than $%02X at PPU OAM address $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU writes greater than $%02X between PPU OAM address $%04X and $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }
            break;
            case eBreakIfLessThan:
               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU writes less than $%02X at PPU OAM address $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU writes less than $%02X between PPU OAM address $%04X and $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }
            break;
         }
      break;
      case eBreakOnCPUState:
         pRegister = C6502::REGISTERS()[m_breakpoint[idx].item1];
         pBitfield = pRegister->GetBitfield(m_breakpoint[idx].item2);
         switch ( m_breakpoint[idx].dataType )
         {
            case eBreakpointDataPure:
               switch ( m_breakpoint[idx].condition )
               {
                  case eBreakIfEqual:
                     msg += sprintf ( msg, "Break if %s::%s is $",
                               pRegister->GetName(),
                               pBitfield->GetName() );
                     sprintf ( msg, pBitfield->GetDisplayFormat(),
                               m_breakpoint[idx].data );
                  break;
                  case eBreakIfNotEqual:
                     msg += sprintf ( msg, "Break if %s::%s is not $",
                               pRegister->GetName(),
                               pBitfield->GetName() );
                     sprintf ( msg, pBitfield->GetDisplayFormat(),
                               m_breakpoint[idx].data );
                  break;
                  case eBreakIfGreaterThan:
                     msg += sprintf ( msg, "Break if %s::%s is greater than $",
                               pRegister->GetName(),
                               pBitfield->GetName() );
                     sprintf ( msg, pBitfield->GetDisplayFormat(),
                               m_breakpoint[idx].data );
                  break;
                  case eBreakIfLessThan:
                     msg += sprintf ( msg, "Break if %s::%s is less than $",
                               pRegister->GetName(),
                               pBitfield->GetName() );
                     sprintf ( msg, pBitfield->GetDisplayFormat(),
                               m_breakpoint[idx].data );
                  break;
               }
            break;
            case eBreakpointDataPick:
               switch ( m_breakpoint[idx].condition )
               {
                  case eBreakIfEqual:
                     sprintf ( msg, "Break if %s::%s is '%s'",
                               pRegister->GetName(),
                               pBitfield->GetName(),
                               pBitfield->GetValueByIndex(m_breakpoint[idx].data) );
                  break;
                  case eBreakIfNotEqual:
                     sprintf ( msg, "Break if %s::%s is not '%s'",
                               pRegister->GetName(),
                               pBitfield->GetName(),
                               pBitfield->GetValueByIndex(m_breakpoint[idx].data) );
                  break;
                  case eBreakIfGreaterThan:
                     sprintf ( msg, "Break if %s::%s is greater than $%X",
                               pRegister->GetName(),
                               pBitfield->GetName(),
                               m_breakpoint[idx].data );
                  break;
                  case eBreakIfLessThan:
                     sprintf ( msg, "Break if %s::%s is less than $%X",
                               pRegister->GetName(),
                               pBitfield->GetName(),
                               m_breakpoint[idx].data );
                  break;
               }
            break;
         }
      break;
      case eBreakOnCPUEvent:
      break;
      case eBreakOnPPUFetch:
         switch ( m_breakpoint[idx].condition )
         {
            case eBreakIfAnything:
               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if PPU fetches anything at $%04X",
                                   m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if PPU fetches anything between $%04X and $%04X",
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }
            break;
            case eBreakIfEqual:
               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if PPU fetches $%02X at address $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if PPU fetches $%02X between $%04X and $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }
            break;
            case eBreakIfNotEqual:
               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if PPU fetches anything but $%02X at address $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if PPU fetches anything but $%02X between $%04X and $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }
            break;
            case eBreakIfGreaterThan:
               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if PPU fetches greater than $%02X at address $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if PPU fetches greater than $%02X between $%04X and $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }
            break;
            case eBreakIfLessThan:
               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if PPU fetches less than $%02X at address $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if PPU fetches less than $%02X between $%04X and $%04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }
            break;
         }
      break;
      case eBreakOnPPUState:
         pRegister = CPPU::REGISTERS()[m_breakpoint[idx].item1];
         pBitfield = pRegister->GetBitfield(m_breakpoint[idx].item2);
         switch ( m_breakpoint[idx].dataType )
         {
            case eBreakpointDataPure:
               switch ( m_breakpoint[idx].condition )
               {
                  case eBreakIfAnything:
                     sprintf ( msg, "Break if %s::%s is accessed",
                               pRegister->GetName(),
                               pBitfield->GetName() );
                  break;
                  case eBreakIfEqual:
                     sprintf ( msg, "Break if %s::%s is $%02X",
                               pRegister->GetName(),
                               pBitfield->GetName(),
                               m_breakpoint[idx].data );
                  break;
                  case eBreakIfNotEqual:
                     sprintf ( msg, "Break if %s::%s is not $%02X",
                               pRegister->GetName(),
                               pBitfield->GetName(),
                               m_breakpoint[idx].data );
                  break;
                  case eBreakIfGreaterThan:
                     sprintf ( msg, "Break if %s::%s is greater than $%02X",
                               pRegister->GetName(),
                               pBitfield->GetName(),
                               m_breakpoint[idx].data );
                  break;
                  case eBreakIfLessThan:
                     sprintf ( msg, "Break if %s::%s is less than $%02X",
                               pRegister->GetName(),
                               pBitfield->GetName(),
                               m_breakpoint[idx].data );
                  break;
               }
            break;
            case eBreakpointDataPick:
               switch ( m_breakpoint[idx].condition )
               {
                  case eBreakIfAnything:
                     sprintf ( msg, "Break if %s::%s is accessed",
                               pRegister->GetName(),
                               pBitfield->GetName() );
                  break;
                  case eBreakIfEqual:
                     sprintf ( msg, "Break if %s::%s is '%s'",
                               pRegister->GetName(),
                               pBitfield->GetName(),
                               pBitfield->GetValueByIndex(m_breakpoint[idx].data) );
                  break;
                  case eBreakIfNotEqual:
                     sprintf ( msg, "Break if %s::%s is not '%s'",
                               pRegister->GetName(),
                               pBitfield->GetName(),
                               pBitfield->GetValueByIndex(m_breakpoint[idx].data) );
                  break;
                  case eBreakIfGreaterThan:
                     sprintf ( msg, "Break if %s::%s is greater than $%X",
                               pRegister->GetName(),
                               pBitfield->GetName(),
                               m_breakpoint[idx].data );
                  break;
                  case eBreakIfLessThan:
                     sprintf ( msg, "Break if %s::%s is less than $%X",
                               pRegister->GetName(),
                               pBitfield->GetName(),
                               m_breakpoint[idx].data );
                  break;
               }
            break;
         }
      break;
      case eBreakOnPPUEvent:
         sprintf ( msg, m_breakpoint[idx].pEvent->GetDisplayFormat(),
                      m_breakpoint[idx].item1,
                      m_breakpoint[idx].item2 );
      break;
      case eBreakOnAPUState:
         pRegister = CAPU::REGISTERS()[m_breakpoint[idx].item1];
         pBitfield = pRegister->GetBitfield(m_breakpoint[idx].item2);
         switch ( m_breakpoint[idx].dataType )
         {
            case eBreakpointDataPure:
               switch ( m_breakpoint[idx].condition )
               {
                  case eBreakIfAnything:
                     sprintf ( msg, "Break if %s::%s is accessed",
                               pRegister->GetName(),
                               pBitfield->GetName() );
                  break;
                  case eBreakIfEqual:
                     sprintf ( msg, "Break if %s::%s is $%02X",
                               pRegister->GetName(),
                               pBitfield->GetName(),
                               m_breakpoint[idx].data );
                  break;
                  case eBreakIfNotEqual:
                     sprintf ( msg, "Break if %s::%s is not $%02X",
                               pRegister->GetName(),
                               pBitfield->GetName(),
                               m_breakpoint[idx].data );
                  break;
                  case eBreakIfGreaterThan:
                     sprintf ( msg, "Break if %s::%s is greater than $%02X",
                               pRegister->GetName(),
                               pBitfield->GetName(),
                               m_breakpoint[idx].data );
                  break;
                  case eBreakIfLessThan:
                     sprintf ( msg, "Break if %s::%s is less than $%02X",
                               pRegister->GetName(),
                               pBitfield->GetName(),
                               m_breakpoint[idx].data );
                  break;
               }
            break;
            case eBreakpointDataPick:
               switch ( m_breakpoint[idx].condition )
               {
                  case eBreakIfAnything:
                     sprintf ( msg, "Break if %s::%s is accessed",
                               pRegister->GetName(),
                               pBitfield->GetName() );
                  break;
                  case eBreakIfEqual:
                     sprintf ( msg, "Break if %s::%s is '%s'",
                               pRegister->GetName(),
                               pBitfield->GetName(),
                               pBitfield->GetValueByIndex(m_breakpoint[idx].data) );
                  break;
                  case eBreakIfNotEqual:
                     sprintf ( msg, "Break if %s::%s is not '%s'",
                               pRegister->GetName(),
                               pBitfield->GetName(),
                               pBitfield->GetValueByIndex(m_breakpoint[idx].data) );
                  break;
                  case eBreakIfGreaterThan:
                     sprintf ( msg, "Break if %s::%s is greater than $%X",
                               pRegister->GetName(),
                               pBitfield->GetName(),
                               m_breakpoint[idx].data );
                  break;
                  case eBreakIfLessThan:
                     sprintf ( msg, "Break if %s::%s is less than $%X",
                               pRegister->GetName(),
                               pBitfield->GetName(),
                               m_breakpoint[idx].data );
                  break;
               }
            break;
         }
      break;
      case eBreakOnAPUEvent:
      break;
      case eBreakOnMapperState:
         if ( CROM::REGISTERS() )
         {
            pRegister = CROM::REGISTERS()[m_breakpoint[idx].item1];
            pBitfield = pRegister->GetBitfield(m_breakpoint[idx].item2);
            switch ( m_breakpoint[idx].dataType )
            {
               case eBreakpointDataPure:
                  switch ( m_breakpoint[idx].condition )
                  {
                     case eBreakIfAnything:
                        sprintf ( msg, "Break if %s::%s is accessed",
                                  pRegister->GetName(),
                                  pBitfield->GetName() );
                     break;
                     case eBreakIfEqual:
                        sprintf ( msg, "Break if %s::%s is $%02X",
                                  pRegister->GetName(),
                                  pBitfield->GetName(),
                                  m_breakpoint[idx].data );
                     break;
                     case eBreakIfNotEqual:
                        sprintf ( msg, "Break if %s::%s is not $%02X",
                                  pRegister->GetName(),
                                  pBitfield->GetName(),
                                  m_breakpoint[idx].data );
                     break;
                     case eBreakIfGreaterThan:
                        sprintf ( msg, "Break if %s::%s is greater than $%02X",
                                  pRegister->GetName(),
                                  pBitfield->GetName(),
                                  m_breakpoint[idx].data );
                     break;
                     case eBreakIfLessThan:
                        sprintf ( msg, "Break if %s::%s is less than $%02X",
                                  pRegister->GetName(),
                                  pBitfield->GetName(),
                                  m_breakpoint[idx].data );
                     break;
                  }
               break;
               case eBreakpointDataPick:
                  switch ( m_breakpoint[idx].condition )
                  {
                     case eBreakIfAnything:
                        sprintf ( msg, "Break if %s::%s is accessed",
                                  pRegister->GetName(),
                                  pBitfield->GetName() );
                     break;
                     case eBreakIfEqual:
                        sprintf ( msg, "Break if %s::%s is '%s'",
                                  pRegister->GetName(),
                                  pBitfield->GetName(),
                                  pBitfield->GetValueByIndex(m_breakpoint[idx].data) );
                     break;
                     case eBreakIfNotEqual:
                        sprintf ( msg, "Break if %s::%s is not '%s'",
                                  pRegister->GetName(),
                                  pBitfield->GetName(),
                                  pBitfield->GetValueByIndex(m_breakpoint[idx].data) );
                     break;
                     case eBreakIfGreaterThan:
                        sprintf ( msg, "Break if %s::%s is greater than $%X",
                                  pRegister->GetName(),
                                  pBitfield->GetName(),
                                  m_breakpoint[idx].data );
                     break;
                     case eBreakIfLessThan:
                        sprintf ( msg, "Break if %s::%s is less than $%X",
                                  pRegister->GetName(),
                                  pBitfield->GetName(),
                                  m_breakpoint[idx].data );
                     break;
                  }
               break;
            }
         }
      break;
      case eBreakOnMapperEvent:
      break;
   }
}
