#if !defined ( CODE_DATA_LOGGER_H )
#define CODE_DATA_LOGGER_H

#include "nes_emulator_core.h"

#define LAST_VALUE_LIST_LEN 10

enum
{
   eLogger_InstructionFetch,
   eLogger_OperandFetch,
   eLogger_DataRead,
   eLogger_DMA,
   eLogger_DataWrite
};

enum
{
   eLoggerCol_Type = 0,
   eLoggerCol_Source,
   eLoggerCol_CPUAddr,
   eLoggerCol_Cycle,
   eLoggerCol_Count,
   eLoggerCol_LastLoadAddr
};

#pragma pack(1)
typedef struct _LoggerInfo
{
   uint32_t cycle;
   uint16_t cpuAddr;
   uint32_t count;
   int8_t type;
   int8_t source;
   struct _LoggerInfo* pLastLoad;
} LoggerInfo;
#pragma pack()

class CCodeDataLogger
{
public:
   CCodeDataLogger(uint32_t size, uint32_t mask);
   ~CCodeDataLogger();

   void ClearData ( void );
   void LogAccess ( uint32_t cycle, uint32_t addr, uint8_t data, int8_t type, int8_t source );
   uint32_t GetMask() { return m_mask; }
   uint32_t GetCount ( uint32_t addr )
   {
      return (*(m_pLogger+(addr&m_mask))).count;
   }
   uint32_t GetCycle ( uint32_t addr )
   {
      return (*(m_pLogger+(addr&m_mask))).cycle;
   }
   uint32_t GetCPUAddr ( uint32_t addr )
   {
      return (*(m_pLogger+(addr&m_mask))).cpuAddr;
   }
   uint32_t GetType ( uint32_t addr )
   {
      return (*(m_pLogger+(addr&m_mask))).type;
   }
   uint32_t GetSource ( uint32_t addr )
   {
      return (*(m_pLogger+(addr&m_mask))).source;
   }
   uint32_t GetSize ( void )
   {
      return m_size;
   }
   void GetPrintable ( uint32_t addr, int32_t subItem, char* str );
   uint32_t GetLastLoadAddr ( uint32_t addr );
   LoggerInfo* GetLogEntry ( uint32_t addr )
   {
      return (m_pLogger+(addr&m_mask));
   }

   static inline uint32_t GetCurCycle ( void )
   {
      return m_curCycle;
   }
   inline uint32_t GetMaxCount ( void )
   {
      return m_maxCount;
   }

protected:
   uint32_t        m_size;
   uint32_t        m_mask;
   static uint32_t m_curCycle;
   uint32_t m_maxCount;
   static LoggerInfo* m_pLastLoad;
   LoggerInfo* m_pLogger;
};

void nesClearCodeDataLoggerDatabases ();
CCodeDataLogger* nesGetCpuCodeDataLoggerDatabase ( void );
CCodeDataLogger* nesGetVirtualPRGROMCodeDataLoggerDatabase ( uint32_t addr );
CCodeDataLogger* nesGetPhysicalPRGROMCodeDataLoggerDatabase ( uint32_t addr );
CCodeDataLogger* nesGetEXRAMCodeDataLoggerDatabase ( void );
CCodeDataLogger* nesGetVirtualSRAMCodeDataLoggerDatabase ( uint32_t addr );
CCodeDataLogger* nesGetPhysicalSRAMCodeDataLoggerDatabase ( uint32_t addr );
CCodeDataLogger* nesGetPpuCodeDataLoggerDatabase ( void );

#endif
