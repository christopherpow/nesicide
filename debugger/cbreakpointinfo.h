#ifndef CBREAKPOINTINFO_H
#define CBREAKPOINTINFO_H

#define NUM_BREAKPOINTS 4

typedef enum
{
   eBreakOnCPUExecution = 0,
   eBreakOnCPUMemoryAccess,
   eBreakOnCPUMemoryRead,
   eBreakOnCPUMemoryWrite,
   eBreakOnCPUState,
   eBreakOnCPUEvent,
   eBreakOnPPUFetch,
   eBreakOnPPUState,
   eBreakOnPPUEvent,
   eBreakOnAPUState,
   eBreakOnAPUEvent,
   eBreakOnMapperState,
   eBreakOnMapperEvent
} eBreakpointType;

typedef enum
{
   eBreakInCPU = 0,
   eBreakInPPU,
   eBreakInAPU,
   eBreakInMapper
} eBreakpointTarget;

typedef enum
{
   eBreakIfAnything = 0,
   eBreakIfEqual,
   eBreakIfNotEqual,
   eBreakIfGreaterThan,
   eBreakIfLessThan
} eBreakpointCondition;

typedef enum
{
   eBreakpointItemAddress = 0,
   eBreakpointItemRegister,
   eBreakpointItemEvent,
   eBreakpointItemNone
} eBreakpointItemType;

typedef enum
{
   eBreakpointConditionNone = 0,
   eBreakpointConditionTest
} eBreakpointConditionType;

typedef enum
{
   eBreakpointDataNone = 0,
   eBreakpointDataPure,
   eBreakpointDataPick
} eBreakpointDataType;

class CBreakpointEventInfo
{
public:
   CBreakpointEventInfo(const char* name, int elements, const char* displayFormat, int elementRadix )
   {
      m_name = name;
      m_elements = elements;
      m_displayFormat = displayFormat;
      m_elementRadix = elementRadix;
   }
   const char* GetName ( void ) const { return m_name; }
   int         GetNumElements ( void ) const { return m_elements; }
   const char* GetDisplayFormat ( void ) const { return m_displayFormat; }
   int         GetElementRadix ( void ) const { return m_elementRadix; }
private:
   const char* m_name;
   int         m_elements;
   const char* m_displayFormat;
   int         m_elementRadix;
};

typedef struct
{
   eBreakpointType type;
   eBreakpointTarget target;
   eBreakpointItemType itemType;
   int event;
   CBreakpointEventInfo* pEvent;
   int item1;
   int item2;
   eBreakpointConditionType conditionType;
   int condition;
   eBreakpointDataType dataType;
   int data; // depending on type this field will be real value or index of bitfield value...
   bool hit;
} BreakpointInfo;

class CBreakpointInfo
{
public:
    CBreakpointInfo();
    void AddBreakpoint ( eBreakpointType type, eBreakpointItemType itemType, int event, int item1, int item2, eBreakpointConditionType conditionType, int condition, eBreakpointDataType dataType, int data );
    void RemoveBreakpoint ( int index );
    void GetPrintable ( int idx, char* msg );
    int GetNumBreakpoints ( void ) const { return m_numBreakpoints; }
    BreakpointInfo* GetBreakpoint ( int idx ) { return &(m_breakpoint[idx]); }

private:
    BreakpointInfo m_breakpoint [ NUM_BREAKPOINTS ];
    int            m_numBreakpoints;
};

#endif // CBREAKPOINTINFO_H
