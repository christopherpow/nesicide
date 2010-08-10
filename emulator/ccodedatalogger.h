#if !defined ( CODE_DATA_LOGGER_H )
#define CODE_DATA_LOGGER_H

#include "emulator_core.h"

#define LAST_VALUE_LIST_LEN 10

enum
{
   eLogger_InstructionFetch,
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
   unsigned int cycle;
   unsigned short cpuAddr;
   unsigned int count;
   char type;
   char source;
   unsigned char lastValue [ LAST_VALUE_LIST_LEN ];
   unsigned char lastValueCount;
   unsigned char lastValuePos;
   struct _LoggerInfo* pLastLoad;
} LoggerInfo;
#pragma pack()

class CCodeDataLogger
{
public:
   CCodeDataLogger(uint32_t size, uint32_t mask);
   virtual ~CCodeDataLogger();

   void ClearData ( void );
   void LogAccess ( unsigned int cycle, uint32_t addr, unsigned char data, char type, char source );
   unsigned int GetCount ( uint32_t addr ) { return (*(m_pLogger+addr)).count; }
   unsigned int GetCycle ( uint32_t addr ) { return (*(m_pLogger+addr)).cycle; }
   unsigned int GetCPUAddr ( uint32_t addr ) { return (*(m_pLogger+addr)).cpuAddr; }
   unsigned int GetType ( uint32_t addr ) { return (*(m_pLogger+addr)).type; }
   unsigned int GetSource ( uint32_t addr ) { return (*(m_pLogger+addr)).source; }
   unsigned int GetSize ( void ) { return m_size; }
   void GetPrintable ( uint32_t addr, int subItem, char* str );
   unsigned char GetLastValueCount ( uint32_t addr ) { return (*(m_pLogger+addr)).lastValueCount; }
   unsigned char GetLastValue ( uint32_t addr, unsigned char count );
   unsigned int GetLastLoadAddr ( uint32_t addr );
   LoggerInfo* GetLogEntry ( uint32_t addr ) { return (m_pLogger+addr); }

   static inline uint32_t GetCurCycle ( void ) { return m_curCycle; }
   static inline uint32_t GetMaxCount ( void ) { return m_maxCount; }

protected:
   uint32_t        m_size;
   uint32_t        m_mask;
   static uint32_t m_curCycle;
   static uint32_t m_maxCount;
   static LoggerInfo* m_pLastLoad;
   LoggerInfo* m_pLogger;
};

#endif
