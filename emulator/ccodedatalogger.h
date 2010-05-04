// ccodedatalogger.h: interface for the CCodeDataLogger class.
//
//////////////////////////////////////////////////////////////////////

#if !defined ( CODE_DATA_LOGGER_H )
#define CODE_DATA_LOGGER_H

#include "cnesicidecommon.h"

#define LOGGER_DEPTH MEM_8KB

#define LAST_VALUE_LIST_LEN 10

enum
{
   eLogger_Unknown = 0,
   eLogger_InstructionFetch,
   eLogger_DataRead,
   eLogger_DMA,
   eLogger_DataWrite
};

enum
{
   eLoggerSource_Unknown = 0,
   eLoggerSource_CPU,
   eLoggerSource_PPU,
   eLoggerSource_APU
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
   CCodeDataLogger(UINT size = MEM_16KB, UINT mask = MASK_16KB);
	virtual ~CCodeDataLogger();

   void ClearData ( void );
   void LogAccess ( unsigned int cycle, UINT addr, unsigned char data, char type, char source );
   unsigned int GetCount ( UINT addr ) { return (*(m_pLogger+addr)).count; }
   unsigned int GetCycle ( UINT addr ) { return (*(m_pLogger+addr)).cycle; }
   unsigned int GetCPUAddr ( UINT addr ) { return (*(m_pLogger+addr)).cpuAddr; }
   unsigned int GetType ( UINT addr ) { return (*(m_pLogger+addr)).type; }
   unsigned int GetSource ( UINT addr ) { return (*(m_pLogger+addr)).source; }
   unsigned int GetSize ( void ) { return m_size; }
   void GetPrintable ( UINT addr, int subItem, char* str );
   unsigned char GetLastValueCount ( UINT addr ) { return (*(m_pLogger+addr)).lastValueCount; }
   unsigned char GetLastValue ( UINT addr, unsigned char count );
   unsigned int GetLastLoadAddr ( UINT addr );

   static inline UINT GetCurCycle ( void ) { return m_curCycle; }
   static inline UINT GetMaxCount ( void ) { return m_maxCount; }

protected:
   UINT        m_size;
   UINT        m_mask;
   static UINT m_curCycle;
   static UINT m_maxCount;
   static LoggerInfo* m_pLastLoad;
   LoggerInfo* m_pLogger;
};

#endif
