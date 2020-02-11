#include "cbreakpointinfo.h"

#include "c64_emulator_core.h"

#include "cc646502.h"
#include "cc64sid.h"

CC64BreakpointInfo::CC64BreakpointInfo()
{
}

void CC64BreakpointInfo::ModifyBreakpoint ( BreakpointInfo* pBreakpoint, int type, eBreakpointItemType itemType, int event, int item1, int item1Physical, int item2, int mask, bool maskExclusive, eBreakpointConditionType conditionType, int condition, eBreakpointDataType dataType, int data, bool enabled )
{
   pBreakpoint->hit = false;
   pBreakpoint->enabled = enabled;
   pBreakpoint->itemActual = -1;
   pBreakpoint->type = (eBreakpointType)type;
   pBreakpoint->pEvent = NULL;

   switch ( type )
   {
      case eBreakOnCPUExecution:
      case eBreakOnCPUMemoryAccess:
      case eBreakOnCPUMemoryRead:
      case eBreakOnCPUMemoryWrite:
         pBreakpoint->target = eBreakInCPU;
         break;
   }

   pBreakpoint->conditionType = conditionType;
   pBreakpoint->condition = condition;
   pBreakpoint->itemType = itemType;
   pBreakpoint->event = event;

   // Events not supported yet.
   pBreakpoint->pEvent = NULL;

   pBreakpoint->item1 = item1;
   pBreakpoint->item1Physical = item1Physical;
   pBreakpoint->item2 = item2;
   pBreakpoint->itemMask = mask;
   pBreakpoint->itemMaskExclusive = maskExclusive;
   pBreakpoint->dataType = dataType;
   pBreakpoint->data = data;
}

void CC64BreakpointInfo::GetPrintable ( int idx, char* msg )
{
   char printableAddress[32];

   switch ( m_breakpoint[idx].type )
   {
      case eBreakOnCPUExecution:
         c64GetPrintablePhysicalAddress(printableAddress,
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
   }
}

void CC64BreakpointInfo::GetHitPrintable ( int idx, char* hmsg )
{
   char*          msg = hmsg;

   msg += sprintf ( msg, "BREAK: " );
   GetPrintable(idx,msg);
}
