#include "cbreakpointinfo.h"

#include "nes_emulator_core.h"

#include "cnes6502.h"
#include "cnesppu.h"
#include "cnesapu.h"
#include "cnesrom.h"

CNESBreakpointInfo::CNESBreakpointInfo()
{
}

CNESBreakpointInfo::~CNESBreakpointInfo()
{
}

void CNESBreakpointInfo::ModifyBreakpoint ( BreakpointInfo* pBreakpoint, int type, eBreakpointItemType itemType, int event, int item1, int item1Physical, int item2, int mask, bool maskExclusive, eBreakpointConditionType conditionType, int condition, eBreakpointDataType dataType, int data, bool enabled )
{
   pBreakpoint->hit = false;
   pBreakpoint->enabled = enabled;
   pBreakpoint->itemActual = -1;
   pBreakpoint->type = (eBreakpointType)type;
   pBreakpoint->pEvent = NULL;

   switch ( type )
   {
      case eBreakOnPPUCycle:
         // Do nothing, this is not a user settable breakpoint.
         // Just preventing compilation warning.
         break;
      case eBreakOnCPUExecution:
      case eBreakOnCPUMemoryAccess:
      case eBreakOnCPUMemoryRead:
      case eBreakOnCPUMemoryWrite:
      case eBreakOnCPUState:
      case eBreakOnCPUEvent:
         pBreakpoint->target = eBreakInCPU;
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
         pBreakpoint->target = eBreakInPPU;
         break;
      case eBreakOnAPUState:
      case eBreakOnAPUEvent:
         pBreakpoint->target = eBreakInAPU;
         break;
      case eBreakOnMapperState:
      case eBreakOnMapperEvent:
         pBreakpoint->target = eBreakInMapper;
         break;
   }

   pBreakpoint->conditionType = conditionType;
   pBreakpoint->condition = condition;
   pBreakpoint->itemType = itemType;
   pBreakpoint->event = event;

   if ( event >= 0 )
   {
      switch ( pBreakpoint->target )
      {
         case eBreakInCPU:
            pBreakpoint->pEvent = nesGetCpuBreakpointEventDatabase()[event];
            break;
         case eBreakInPPU:
            pBreakpoint->pEvent = nesGetPpuBreakpointEventDatabase()[event];
            break;
         case eBreakInAPU:
            pBreakpoint->pEvent = nesGetApuBreakpointEventDatabase()[event];
            break;
         case eBreakInMapper:
            pBreakpoint->pEvent = nesGetCartridgeBreakpointEventDatabase()[event];
            break;
      }
   }

   pBreakpoint->item1 = item1;
   pBreakpoint->item1Physical = item1Physical;
   pBreakpoint->item2 = item2;
   pBreakpoint->itemMask = mask;
   pBreakpoint->itemMaskExclusive = maskExclusive;
   pBreakpoint->dataType = dataType;
   pBreakpoint->data = data;
}

void CNESBreakpointInfo::GetPrintable ( int idx, char* msg )
{
   CRegisterData* pRegister;
   CBitfieldData* pBitfield;
   char printableAddress[32];

   switch ( m_breakpoint[idx].type )
   {
      case eBreakOnPPUCycle:
         // Do nothing, this is not a user settable breakpoint.
         // Just preventing compilation warning.
         break;
      case eBreakOnCPUExecution:
         nesGetPrintablePhysicalAddress(printableAddress,
                                            m_breakpoint[idx].item1,
                                            m_breakpoint[idx].item1Physical);

         if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
         {
            sprintf ( msg, "Break if CPU executes at %s",
                      printableAddress );
         }
         else
         {
            sprintf ( msg, "Break if CPU executes between address %s and %04X",
                      printableAddress,
                      m_breakpoint[idx].item2 );
         }
         break;
      case eBreakOnCPUMemoryAccess:

         switch ( m_breakpoint[idx].condition )
         {
            case eBreakIfAnything:

               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads or writes anything at address %04X",
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads or writes anything between address %04X and %04X",
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }

               break;
            case eBreakIfEqual:

               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads or writes %02X at address %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads or writes %02X between address %04X and %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }

               break;
            case eBreakIfNotEqual:

               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads or writes anything but %02X at address %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads or writes anything but %02X between address %04X and %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }

               break;
            case eBreakIfGreaterThan:

               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads or writes greater than %02X at address %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads or writes greater than %02X between address %04X and %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }

               break;
            case eBreakIfLessThan:

               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads or writes less than %02X at address %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads or writes less than %02X between address %04X and %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }

               break;
         case eBreakIfExclusiveMask:

            if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
            {
               sprintf ( msg, "Break if CPU reads or writes anything in the exclusive mask %02X at address %04X",
                         m_breakpoint[idx].data,
                         m_breakpoint[idx].item1 );
            }
            else
            {
               sprintf ( msg, "Break if CPU reads or writes anything in the exclusive mask %02X between address %04X and %04X",
                         m_breakpoint[idx].data,
                         m_breakpoint[idx].item1,
                         m_breakpoint[idx].item2 );
            }

            break;
         case eBreakIfInclusiveMask:

            if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
            {
               sprintf ( msg, "Break if CPU reads or writes anything in the inclusive mask %02X at address %04X",
                         m_breakpoint[idx].data,
                         m_breakpoint[idx].item1 );
            }
            else
            {
               sprintf ( msg, "Break if CPU reads or writes anything in the inclusive mask %02X between address %04X and %04X",
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
                  sprintf ( msg, "Break if CPU reads anything at address %04X",
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads anything between address %04X and %04X",
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }

               break;
            case eBreakIfEqual:

               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads %02X at address %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads %02X between address %04X and %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }

               break;
            case eBreakIfNotEqual:

               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads anything but %02X at address %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads anything but %02X between address %04X and %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }

               break;
            case eBreakIfGreaterThan:

               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads greater than %02X at address %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads greater than %02X between address %04X and %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }

               break;
            case eBreakIfLessThan:

               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads less than %02X at address %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads less than %02X between address %04X and %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }

               break;
         case eBreakIfExclusiveMask:

            if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
            {
               sprintf ( msg, "Break if CPU reads anything in the exclusive mask %02X at address %04X",
                         m_breakpoint[idx].data,
                         m_breakpoint[idx].item1 );
            }
            else
            {
               sprintf ( msg, "Break if CPU reads anything in the exclusive mask %02X between address %04X and %04X",
                         m_breakpoint[idx].data,
                         m_breakpoint[idx].item1,
                         m_breakpoint[idx].item2 );
            }

            break;
         case eBreakIfInclusiveMask:

            if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
            {
               sprintf ( msg, "Break if CPU reads anything in the inclusive mask %02X at address %04X",
                         m_breakpoint[idx].data,
                         m_breakpoint[idx].item1 );
            }
            else
            {
               sprintf ( msg, "Break if CPU reads anything in the inclusive mask %02X between address %04X and %04X",
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
                  sprintf ( msg, "Break if CPU writes anything at address %04X",
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU writes anything between address %04X and %04X",
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }

               break;
            case eBreakIfEqual:

               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU writes %02X at address %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU writes %02X between address %04X and %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }

               break;
            case eBreakIfNotEqual:

               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU writes anything but %02X at address %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU writes anything but %02X between address %04X and %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }

               break;
            case eBreakIfGreaterThan:

               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU writes greater than %02X at address %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU writes greater than %02X between address %04X and %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }

               break;
            case eBreakIfLessThan:

               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU writes less than %02X at address %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU writes less than %02X between address %04X and %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }

               break;
         case eBreakIfExclusiveMask:

            if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
            {
               sprintf ( msg, "Break if CPU writes anything in the exclusive mask %02X at address %04X",
                         m_breakpoint[idx].data,
                         m_breakpoint[idx].item1 );
            }
            else
            {
               sprintf ( msg, "Break if CPU writes anything in the exclusive mask %02X between address %04X and %04X",
                         m_breakpoint[idx].data,
                         m_breakpoint[idx].item1,
                         m_breakpoint[idx].item2 );
            }

            break;
         case eBreakIfInclusiveMask:

            if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
            {
               sprintf ( msg, "Break if CPU writes anything in the inclusive mask %02X at address %04X",
                         m_breakpoint[idx].data,
                         m_breakpoint[idx].item1 );
            }
            else
            {
               sprintf ( msg, "Break if CPU writes anything in the inclusive mask %02X between address %04X and %04X",
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
                  sprintf ( msg, "Break if CPU reads or writes anything at PPU address %04X",
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads or writes anything at PPU address between %04X and %04X",
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }

               break;
            case eBreakIfEqual:

               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads or writes %02X at PPU address %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads or writes %02X between PPU address %04X and %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }

               break;
            case eBreakIfNotEqual:

               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads or writes anything but %02X at PPU address %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads or writes anything but %02X between PPU address %04X and %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }

               break;
            case eBreakIfGreaterThan:

               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads or writes greater than %02X at PPU address %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads or writes greater than %02X between PPU address %04X and %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }

               break;
            case eBreakIfLessThan:

               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads or writes less than %02X at PPU address %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads or writes less than %02X between PPU address %04X and %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }

               break;
         case eBreakIfExclusiveMask:

            if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
            {
               sprintf ( msg, "Break if CPU reads or writes anything in the exclusive mask %02X at PPU address %04X",
                         m_breakpoint[idx].data,
                         m_breakpoint[idx].item1 );
            }
            else
            {
               sprintf ( msg, "Break if CPU reads or writes anything in the exclusive mask %02X between PPU address %04X and %04X",
                         m_breakpoint[idx].data,
                         m_breakpoint[idx].item1,
                         m_breakpoint[idx].item2 );
            }

            break;
         case eBreakIfInclusiveMask:

            if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
            {
               sprintf ( msg, "Break if CPU reads or writes anything in the inclusive mask %02X at PPU address %04X",
                         m_breakpoint[idx].data,
                         m_breakpoint[idx].item1 );
            }
            else
            {
               sprintf ( msg, "Break if CPU reads or writes anything in the inclusive mask %02X between PPU address %04X and %04X",
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
                  sprintf ( msg, "Break if CPU reads anything at PPU address %04X",
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads anything between PPU address %04X and %04X",
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }

               break;
            case eBreakIfEqual:

               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads %02X at PPU address %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads %02X between PPU address %04X and %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }

               break;
            case eBreakIfNotEqual:

               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads anything but %02X at PPU address %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads anything but %02X between PPU address %04X and %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }

               break;
            case eBreakIfGreaterThan:

               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads greater than %02X at PPU address %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads greater than %02X between PPU address %04X and %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }

               break;
            case eBreakIfLessThan:

               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads less than %02X at PPU address %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads less than %02X between PPU address %04X and %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }

               break;
         case eBreakIfExclusiveMask:

            if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
            {
               sprintf ( msg, "Break if CPU reads anything in the exclusive mask %02X at PPU address %04X",
                         m_breakpoint[idx].data,
                         m_breakpoint[idx].item1 );
            }
            else
            {
               sprintf ( msg, "Break if CPU reads anything in the exclusive mask %02X between PPU address %04X and %04X",
                         m_breakpoint[idx].data,
                         m_breakpoint[idx].item1,
                         m_breakpoint[idx].item2 );
            }

            break;
         case eBreakIfInclusiveMask:

            if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
            {
               sprintf ( msg, "Break if CPU reads anything in the inclusive mask %02X at PPU address %04X",
                         m_breakpoint[idx].data,
                         m_breakpoint[idx].item1 );
            }
            else
            {
               sprintf ( msg, "Break if CPU reads anything in the inclusive mask %02X between PPU address %04X and %04X",
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
                  sprintf ( msg, "Break if CPU writes anything at PPU address %04X",
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU writes anything between PPU address %04X and %04X",
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }

               break;
            case eBreakIfEqual:

               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU writes %02X at PPU address %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU writes %02X between PPU address %04X and %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }

               break;
            case eBreakIfNotEqual:

               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU writes anything but %02X at PPU address %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU writes anything but %02X between PPU address %04X and %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }

               break;
            case eBreakIfGreaterThan:

               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU writes greater than %02X at PPU address %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU writes greater than %02X between PPU address %04X and %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }

               break;
            case eBreakIfLessThan:

               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU writes less than %02X at PPU address %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU writes less than %02X between PPU address %04X and %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }

               break;
         case eBreakIfExclusiveMask:

            if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
            {
               sprintf ( msg, "Break if CPU writes anything in the exclusive mask %02X at PPU address %04X",
                         m_breakpoint[idx].data,
                         m_breakpoint[idx].item1 );
            }
            else
            {
               sprintf ( msg, "Break if CPU writes anything in the exclusive mask %02X between PPU address %04X and %04X",
                         m_breakpoint[idx].data,
                         m_breakpoint[idx].item1,
                         m_breakpoint[idx].item2 );
            }

            break;
         case eBreakIfInclusiveMask:

            if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
            {
               sprintf ( msg, "Break if CPU writes anything in the inclusive mask %02X at PPU address %04X",
                         m_breakpoint[idx].data,
                         m_breakpoint[idx].item1 );
            }
            else
            {
               sprintf ( msg, "Break if CPU writes anything in the inclusive mask %02X between PPU address %04X and %04X",
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
                  sprintf ( msg, "Break if CPU reads or writes anything at PPU OAM address %04X",
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads or writes anything at PPU OAM address between %04X and %04X",
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }

               break;
            case eBreakIfEqual:

               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads or writes %02X at PPU OAM address %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads or writes %02X between PPU OAM address %04X and %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }

               break;
            case eBreakIfNotEqual:

               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads or writes anything but %02X at PPU OAM address %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads or writes anything but %02X between PPU OAM address %04X and %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }

               break;
            case eBreakIfGreaterThan:

               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads or writes greater than %02X at PPU OAM address %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads or writes greater than %02X between PPU OAM address %04X and %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }

               break;
            case eBreakIfLessThan:

               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads or writes less than %02X at PPU OAM address %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads or writes less than %02X between PPU OAM address %04X and %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }

               break;
         case eBreakIfExclusiveMask:

            if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
            {
               sprintf ( msg, "Break if CPU reads or writes anything in the exclusive mask %02X at PPU OAM address %04X",
                         m_breakpoint[idx].data,
                         m_breakpoint[idx].item1 );
            }
            else
            {
               sprintf ( msg, "Break if CPU reads or writes anything in the exclusive mask %02X between PPU OAM address %04X and %04X",
                         m_breakpoint[idx].data,
                         m_breakpoint[idx].item1,
                         m_breakpoint[idx].item2 );
            }

            break;
         case eBreakIfInclusiveMask:

            if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
            {
               sprintf ( msg, "Break if CPU reads or writes anything in the inclusive mask %02X at PPU OAM address %04X",
                         m_breakpoint[idx].data,
                         m_breakpoint[idx].item1 );
            }
            else
            {
               sprintf ( msg, "Break if CPU reads or writes anything in the inclusive mask %02X between PPU OAM address %04X and %04X",
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
                  sprintf ( msg, "Break if CPU reads anything at PPU OAM address %04X",
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads anything between PPU OAM address %04X and %04X",
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }

               break;
            case eBreakIfEqual:

               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads %02X at PPU OAM address %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads %02X between PPU OAM address %04X and %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }

               break;
            case eBreakIfNotEqual:

               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads anything but %02X at PPU OAM address %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads anything but %02X between PPU OAM address %04X and %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }

               break;
            case eBreakIfGreaterThan:

               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads greater than %02X at PPU OAM address %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads greater than %02X between PPU OAM address %04X and %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }

               break;
            case eBreakIfLessThan:

               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU reads less than %02X at PPU OAM address %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU reads less than %02X between PPU OAM address %04X and %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }

               break;
         case eBreakIfExclusiveMask:

            if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
            {
               sprintf ( msg, "Break if CPU reads anything in the exclusive mask %02X at PPU OAM address %04X",
                         m_breakpoint[idx].data,
                         m_breakpoint[idx].item1 );
            }
            else
            {
               sprintf ( msg, "Break if CPU reads anything in the exclusive mask %02X between PPU OAM address %04X and %04X",
                         m_breakpoint[idx].data,
                         m_breakpoint[idx].item1,
                         m_breakpoint[idx].item2 );
            }

            break;
         case eBreakIfInclusiveMask:

            if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
            {
               sprintf ( msg, "Break if CPU reads anything in the inclusive mask %02X at PPU OAM address %04X",
                         m_breakpoint[idx].data,
                         m_breakpoint[idx].item1 );
            }
            else
            {
               sprintf ( msg, "Break if CPU reads anything in the inclusive mask %02X between PPU OAM address %04X and %04X",
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
                  sprintf ( msg, "Break if CPU writes anything at PPU OAM address %04X",
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU writes anything between PPU OAM address %04X and %04X",
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }

               break;
            case eBreakIfEqual:

               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU writes %02X at PPU OAM address %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU writes %02X between PPU OAM address %04X and %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }

               break;
            case eBreakIfNotEqual:

               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU writes anything but %02X at PPU OAM address %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU writes anything but %02X between PPU OAM address %04X and %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }

               break;
            case eBreakIfGreaterThan:

               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU writes greater than %02X at PPU OAM address %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU writes greater than %02X between PPU OAM address %04X and %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }

               break;
            case eBreakIfLessThan:

               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if CPU writes less than %02X at PPU OAM address %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if CPU writes less than %02X between PPU OAM address %04X and %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }

               break;
         case eBreakIfExclusiveMask:

            if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
            {
               sprintf ( msg, "Break if CPU writes anything in the exclusive mask %02X at PPU OAM address %04X",
                         m_breakpoint[idx].data,
                         m_breakpoint[idx].item1 );
            }
            else
            {
               sprintf ( msg, "Break if CPU writes anything in the exclusive mask %02X between PPU OAM address %04X and %04X",
                         m_breakpoint[idx].data,
                         m_breakpoint[idx].item1,
                         m_breakpoint[idx].item2 );
            }

            break;
         case eBreakIfInclusiveMask:

            if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
            {
               sprintf ( msg, "Break if CPU writes anything in the inclusive mask %02X at PPU OAM address %04X",
                         m_breakpoint[idx].data,
                         m_breakpoint[idx].item1 );
            }
            else
            {
               sprintf ( msg, "Break if CPU writes anything in the inclusive mask %02X between PPU OAM address %04X and %04X",
                         m_breakpoint[idx].data,
                         m_breakpoint[idx].item1,
                         m_breakpoint[idx].item2 );
            }

            break;
         }

         break;
      case eBreakOnCPUState:
         pRegister = nesGetCpuRegisterDatabase()->GetRegister(m_breakpoint[idx].item1);
         pBitfield = pRegister->GetBitfield(m_breakpoint[idx].item2);

         switch ( m_breakpoint[idx].dataType )
         {
            case eBreakpointDataPure:

            switch ( m_breakpoint[idx].condition )
            {
               case eBreakIfAnything:
                  sprintf ( msg, "Break if %s::%s is '%s'",
                            pRegister->GetName(),
                            pBitfield->GetName(),
                            pBitfield->GetValueByIndex(m_breakpoint[idx].data) );
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
                  sprintf ( msg, "Break if %s::%s is greater than %X",
                            pRegister->GetName(),
                            pBitfield->GetName(),
                            m_breakpoint[idx].data );
                  break;
               case eBreakIfLessThan:
                  sprintf ( msg, "Break if %s::%s is less than %X",
                            pRegister->GetName(),
                            pBitfield->GetName(),
                            m_breakpoint[idx].data );
                  break;
            case eBreakIfExclusiveMask:
               msg += sprintf ( msg, "Break if %s::%s contains anything exclusively in the mask %02X",
                                pRegister->GetName(),
                                pBitfield->GetName(),
                                m_breakpoint[idx].data );
               break;
            case eBreakIfInclusiveMask:
               msg += sprintf ( msg, "Break if %s::%s contains anything inclusively in the mask %02X",
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
                  msg += sprintf ( msg, "Break if %s::%s is ",
                                   pRegister->GetName(),
                                   pBitfield->GetName() );
                  sprintf ( msg, pBitfield->GetDisplayFormat(),
                            m_breakpoint[idx].data );
                  break;
               case eBreakIfNotEqual:
                  msg += sprintf ( msg, "Break if %s::%s is not ",
                                   pRegister->GetName(),
                                   pBitfield->GetName() );
                  sprintf ( msg, pBitfield->GetDisplayFormat(),
                            m_breakpoint[idx].data );
                  break;
               case eBreakIfGreaterThan:
                  msg += sprintf ( msg, "Break if %s::%s is greater than ",
                                   pRegister->GetName(),
                                   pBitfield->GetName() );
                  sprintf ( msg, pBitfield->GetDisplayFormat(),
                            m_breakpoint[idx].data );
                  break;
               case eBreakIfLessThan:
                  msg += sprintf ( msg, "Break if %s::%s is less than ",
                                   pRegister->GetName(),
                                   pBitfield->GetName() );
                  sprintf ( msg, pBitfield->GetDisplayFormat(),
                            m_breakpoint[idx].data );
                  break;
            case eBreakIfExclusiveMask:
               msg += sprintf ( msg, "Break if %s::%s contains anything exclusively in the mask ",
                                pRegister->GetName(),
                                pBitfield->GetName() );
               sprintf ( msg, pBitfield->GetDisplayFormat(),
                         m_breakpoint[idx].data );
               break;
            case eBreakIfInclusiveMask:
               msg += sprintf ( msg, "Break if %s::%s contains anything inclusively in the mask ",
                                pRegister->GetName(),
                                pBitfield->GetName() );
               sprintf ( msg, pBitfield->GetDisplayFormat(),
                         m_breakpoint[idx].data );
               break;
            }

               break;
            default:
               break;
         }

         break;
      case eBreakOnCPUEvent:
         sprintf ( msg, m_breakpoint[idx].pEvent->GetDisplayFormat(),
                   m_breakpoint[idx].item1,
                   m_breakpoint[idx].item2 );
         break;
      case eBreakOnPPUFetch:

         switch ( m_breakpoint[idx].condition )
         {
            case eBreakIfAnything:

               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if PPU fetches anything at %04X",
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if PPU fetches anything between %04X and %04X",
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }

               break;
            case eBreakIfEqual:

               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if PPU fetches %02X at address %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if PPU fetches %02X between %04X and %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }

               break;
            case eBreakIfNotEqual:

               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if PPU fetches anything but %02X at address %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if PPU fetches anything but %02X between %04X and %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }

               break;
            case eBreakIfGreaterThan:

               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if PPU fetches greater than %02X at address %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if PPU fetches greater than %02X between %04X and %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }

               break;
            case eBreakIfLessThan:

               if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
               {
                  sprintf ( msg, "Break if PPU fetches less than %02X at address %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1 );
               }
               else
               {
                  sprintf ( msg, "Break if PPU fetches less than %02X between %04X and %04X",
                            m_breakpoint[idx].data,
                            m_breakpoint[idx].item1,
                            m_breakpoint[idx].item2 );
               }

               break;
         case eBreakIfExclusiveMask:

            if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
            {
               sprintf ( msg, "Break if PPU fetches anything in the exclusive mask %02X at address %04X",
                         m_breakpoint[idx].data,
                         m_breakpoint[idx].item1 );
            }
            else
            {
               sprintf ( msg, "Break if PPU fetches anything in the exclusive mask %02X between %04X and %04X",
                         m_breakpoint[idx].data,
                         m_breakpoint[idx].item1,
                         m_breakpoint[idx].item2 );
            }

            break;
         case eBreakIfInclusiveMask:

            if ( m_breakpoint[idx].item1 == m_breakpoint[idx].item2 )
            {
               sprintf ( msg, "Break if PPU fetches anything in the inclusive mask %02X at address %04X",
                         m_breakpoint[idx].data,
                         m_breakpoint[idx].item1 );
            }
            else
            {
               sprintf ( msg, "Break if PPU fetches anything in the inclusive mask %02X between %04X and %04X",
                         m_breakpoint[idx].data,
                         m_breakpoint[idx].item1,
                         m_breakpoint[idx].item2 );
            }

            break;
            default:
               break;
         }

         break;
      case eBreakOnPPUState:
         pRegister = nesGetPpuRegisterDatabase()->GetRegister(m_breakpoint[idx].item1);
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
                  sprintf ( msg, "Break if %s::%s is greater than %X",
                            pRegister->GetName(),
                            pBitfield->GetName(),
                            m_breakpoint[idx].data );
                  break;
               case eBreakIfLessThan:
                  sprintf ( msg, "Break if %s::%s is less than %X",
                            pRegister->GetName(),
                            pBitfield->GetName(),
                            m_breakpoint[idx].data );
                  break;
            case eBreakIfExclusiveMask:
               msg += sprintf ( msg, "Break if %s::%s contains anything exclusively in the mask %02X",
                                pRegister->GetName(),
                                pBitfield->GetName(),
                                m_breakpoint[idx].data );
               break;
            case eBreakIfInclusiveMask:
               msg += sprintf ( msg, "Break if %s::%s contains anything inclusively in the mask %02X",
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
                  msg += sprintf ( msg, "Break if %s::%s is ",
                                   pRegister->GetName(),
                                   pBitfield->GetName() );
                  sprintf ( msg, pBitfield->GetDisplayFormat(),
                            m_breakpoint[idx].data );
                  break;
               case eBreakIfNotEqual:
                  msg += sprintf ( msg, "Break if %s::%s is not ",
                                   pRegister->GetName(),
                                   pBitfield->GetName() );
                  sprintf ( msg, pBitfield->GetDisplayFormat(),
                            m_breakpoint[idx].data );
                  break;
               case eBreakIfGreaterThan:
                  msg += sprintf ( msg, "Break if %s::%s is greater than ",
                                   pRegister->GetName(),
                                   pBitfield->GetName() );
                  sprintf ( msg, pBitfield->GetDisplayFormat(),
                            m_breakpoint[idx].data );
                  break;
               case eBreakIfLessThan:
                  msg += sprintf ( msg, "Break if %s::%s is less than ",
                                   pRegister->GetName(),
                                   pBitfield->GetName() );
                  sprintf ( msg, pBitfield->GetDisplayFormat(),
                            m_breakpoint[idx].data );
                  break;
            case eBreakIfExclusiveMask:
               msg += sprintf ( msg, "Break if %s::%s contains anything exclusively in the mask ",
                                pRegister->GetName(),
                                pBitfield->GetName() );
               sprintf ( msg, pBitfield->GetDisplayFormat(),
                         m_breakpoint[idx].data );
               break;
            case eBreakIfInclusiveMask:
               msg += sprintf ( msg, "Break if %s::%s contains anything inclusively in the mask ",
                                pRegister->GetName(),
                                pBitfield->GetName() );
               sprintf ( msg, pBitfield->GetDisplayFormat(),
                         m_breakpoint[idx].data );
               break;
            }

               break;
            default:
               break;
         }

         break;
      case eBreakOnPPUEvent:
         sprintf ( msg, m_breakpoint[idx].pEvent->GetDisplayFormat(),
                   m_breakpoint[idx].item1,
                   m_breakpoint[idx].item2 );
         break;
      case eBreakOnAPUState:
         pRegister = nesGetApuRegisterDatabase()->GetRegister(m_breakpoint[idx].item1);
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
                  sprintf ( msg, "Break if %s::%s is greater than %X",
                            pRegister->GetName(),
                            pBitfield->GetName(),
                            m_breakpoint[idx].data );
                  break;
               case eBreakIfLessThan:
                  sprintf ( msg, "Break if %s::%s is less than %X",
                            pRegister->GetName(),
                            pBitfield->GetName(),
                            m_breakpoint[idx].data );
                  break;
            case eBreakIfExclusiveMask:
               sprintf ( msg, "Break if %s::%s contains anything exclusively in the mask %02X",
                         pRegister->GetName(),
                         pBitfield->GetName(),
                         m_breakpoint[idx].data );
               break;
            case eBreakIfInclusiveMask:
               sprintf ( msg, "Break if %s::%s contains anything inclusively in the mask %02X",
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
                  msg += sprintf ( msg, "Break if %s::%s is ",
                                   pRegister->GetName(),
                                   pBitfield->GetName() );
                  sprintf ( msg, pBitfield->GetDisplayFormat(),
                            m_breakpoint[idx].data );
                  break;
               case eBreakIfNotEqual:
                  msg += sprintf ( msg, "Break if %s::%s is not ",
                                   pRegister->GetName(),
                                   pBitfield->GetName() );
                  sprintf ( msg, pBitfield->GetDisplayFormat(),
                            m_breakpoint[idx].data );
                  break;
               case eBreakIfGreaterThan:
                  msg += sprintf ( msg, "Break if %s::%s is greater than ",
                                   pRegister->GetName(),
                                   pBitfield->GetName() );
                  sprintf ( msg, pBitfield->GetDisplayFormat(),
                            m_breakpoint[idx].data );
                  break;
               case eBreakIfLessThan:
                  msg += sprintf ( msg, "Break if %s::%s is less than ",
                                   pRegister->GetName(),
                                   pBitfield->GetName() );
                  sprintf ( msg, pBitfield->GetDisplayFormat(),
                            m_breakpoint[idx].data );
                  break;
            case eBreakIfExclusiveMask:
               msg += sprintf ( msg, "Break if %s::%s contains anything exclusively in the mask ",
                                pRegister->GetName(),
                                pBitfield->GetName() );
               sprintf ( msg, pBitfield->GetDisplayFormat(),
                         m_breakpoint[idx].data );
               break;
            case eBreakIfInclusiveMask:
               msg += sprintf ( msg, "Break if %s::%s contains anything inclusively in the mask ",
                                pRegister->GetName(),
                                pBitfield->GetName() );
               sprintf ( msg, pBitfield->GetDisplayFormat(),
                         m_breakpoint[idx].data );
               break;
            }

               break;
            default:
               break;
         }

         break;
      case eBreakOnAPUEvent:
         sprintf ( msg, m_breakpoint[idx].pEvent->GetDisplayFormat(),
                   m_breakpoint[idx].item1,
                   m_breakpoint[idx].item2 );
         break;
      case eBreakOnMapperState:

         if ( nesGetCartridgeRegisterDatabase() )
         {
            pRegister = nesGetCartridgeRegisterDatabase()->GetRegister(m_breakpoint[idx].item1);
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
                     sprintf ( msg, "Break if %s::%s is greater than %X",
                               pRegister->GetName(),
                               pBitfield->GetName(),
                               m_breakpoint[idx].data );
                     break;
                  case eBreakIfLessThan:
                     sprintf ( msg, "Break if %s::%s is less than %X",
                               pRegister->GetName(),
                               pBitfield->GetName(),
                               m_breakpoint[idx].data );
                     break;
               case eBreakIfExclusiveMask:
                  sprintf ( msg, "Break if %s::%s contains anything exclusively in the mask %02X",
                            pRegister->GetName(),
                            pBitfield->GetName(),
                            m_breakpoint[idx].data );
                  break;
               case eBreakIfInclusiveMask:
                  sprintf ( msg, "Break if %s::%s contains anything inclusively in the mask %02X",
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
                     msg += sprintf ( msg, "Break if %s::%s is ",
                                      pRegister->GetName(),
                                      pBitfield->GetName() );
                     sprintf ( msg, pBitfield->GetDisplayFormat(),
                               m_breakpoint[idx].data );
                     break;
                  case eBreakIfNotEqual:
                     msg += sprintf ( msg, "Break if %s::%s is not ",
                                      pRegister->GetName(),
                                      pBitfield->GetName() );
                     sprintf ( msg, pBitfield->GetDisplayFormat(),
                               m_breakpoint[idx].data );
                     break;
                  case eBreakIfGreaterThan:
                     msg += sprintf ( msg, "Break if %s::%s is greater than ",
                                      pRegister->GetName(),
                                      pBitfield->GetName() );
                     sprintf ( msg, pBitfield->GetDisplayFormat(),
                               m_breakpoint[idx].data );
                     break;
                  case eBreakIfLessThan:
                     msg += sprintf ( msg, "Break if %s::%s is less than ",
                                      pRegister->GetName(),
                                      pBitfield->GetName() );
                     sprintf ( msg, pBitfield->GetDisplayFormat(),
                               m_breakpoint[idx].data );
                     break;
               case eBreakIfExclusiveMask:
                  msg += sprintf ( msg, "Break if %s::%s contains anything exclusively in the mask ",
                                   pRegister->GetName(),
                                   pBitfield->GetName() );
                  sprintf ( msg, pBitfield->GetDisplayFormat(),
                            m_breakpoint[idx].data );
                  break;
               case eBreakIfInclusiveMask:
                  msg += sprintf ( msg, "Break if %s::%s contains anything inclusively in the mask ",
                                   pRegister->GetName(),
                                   pBitfield->GetName() );
                  sprintf ( msg, pBitfield->GetDisplayFormat(),
                            m_breakpoint[idx].data );
                  break;
               }

                  break;
               default:
                  break;
            }
         }

         break;
      case eBreakOnMapperEvent:
         sprintf ( msg, m_breakpoint[idx].pEvent->GetDisplayFormat(),
                   m_breakpoint[idx].item1,
                   m_breakpoint[idx].item2 );
         break;
   }
}

void CNESBreakpointInfo::GetHitPrintable ( int idx, char* hmsg )
{
   char*          msg = hmsg;

   msg += sprintf ( msg, "[PPU(frame=%d,cycle=%d),CPU(cycle=%d),APU(cycle=%d)] BREAK: ", CNES::NES()->PPU()->_FRAME(), CNES::NES()->PPU()->_CYCLES(), CNES::NES()->CPU()->_CYCLES(), CNES::NES()->CPU()->APU()->CYCLES() );
   GetPrintable(idx,msg);
}
