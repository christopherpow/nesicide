#ifndef CC64SID_H
#define CC64SID_H

#include "c64_emulator_core.h"

#include "cregisterdata.h"

class CSID
{
public:
   CSID();
   virtual ~CSID();

   // Interface to retrieve the database defining the registers
   // of the CPU core in a "human readable" form that is used by
   // the Register-type debugger inspector.  The hexadecimal world
   // of the CPU core registers is transformed into textual description
   // and manipulatable bitfield texts by means of this database.
   // CPU registers are declared in the source file.
   static CRegisterDatabase* REGISTERS()
   {
      return m_dbRegisters;
   }

   // Return the contents of a memory location visible to the SID.
   static uint8_t _MEM8 ( uint32_t addr )
   {
      return *(m_SIDmemory+(addr-SID_BASE));
   }

   // Modify the contents of a memory location visible to the SID.
   static void _MEM8 ( uint32_t addr, uint8_t data )
   {
      *(m_SIDmemory+(addr-SID_BASE)) = data;
   }

   // Return the contents of a memory location visible to the SID.
   static uint16_t _MEM16 ( uint32_t addr )
   {
      return *(uint16_t*)(m_SIDmemory+(addr-SID_BASE));
   }

   // Modify the contents of a memory location visible to the SID.
   static void _MEM16 ( uint32_t addr, uint16_t data )
   {
      *(uint16_t*)(m_SIDmemory+(addr-SID_BASE)) = data;
   }

protected:
   // The database for CPU core registers.  Declaration
   // is in source file.
   static CRegisterDatabase* m_dbRegisters;

   // SID register data.
   static uint8_t m_SIDmemory[32];
};

#endif // CC64SID_H
