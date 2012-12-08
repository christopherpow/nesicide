#if !defined ( TRACER_H )
#define TRACER_H

#include "nes_emulator_core.h"

#define TRACER_DEFAULT_DEPTH 262144

enum
{
   eTracer_Unknown = 0,
   eTracer_InstructionFetch,
   eTracer_OperandFetch,
   eTracer_ExtraInstructionFetch,
   eTracer_StolenCycle,
   eTracer_DataRead,
   eTracer_DataWrite,
   eTracer_DMA,
   eTracer_RESET,
   eTracer_NMI,
   eTracer_IRQ,
   eTracer_IRQRelease,
   eTracer_GarbageRead,
   eTracer_RenderBkgnd,
   eTracer_RenderSprite,
   eTracer_StartPPUFrame,
   eTracer_Sprite0Hit,
   eTracer_VBLANKStart,
   eTracer_VBLANKEnd,
   eTracer_PreRenderStart,
   eTracer_PreRenderEnd,
   eTracer_QuietStart,
   eTracer_QuietEnd,
   eTracer_EndPPUFrame,
   eTracer_StartAPUFrame,
   eTracer_SequencerStep,
   eTracer_EndAPUFrame
};

enum
{
   eTarget_Unknown = 0,
   eTarget_RAM,
   eTarget_PPURegister,
   eTarget_APURegister,
   eTarget_IORegister,
   eTarget_SRAM,
   eTarget_EXRAM,
   eTarget_Mapper,
   eTarget_PatternMemory,
   eTarget_NameTable,
   eTarget_AttributeTable,
   eTarget_Palette,
   eTarget_ExtraCycle
};

enum
{
   eTracerCol_Frame = 0,
   eTracerCol_Cycle,
   eTracerCol_Source,
   eTracerCol_Type,
   eTracerCol_Target,
   eTracerCol_Addr,
   eTracerCol_Data,
   eTracerCol_Info,
   eTracerCol_CPU_A,
   eTracerCol_CPU_X,
   eTracerCol_CPU_Y,
   eTracerCol_CPU_SP,
   eTracerCol_CPU_F,
   eTracerCol_CPU_EA,
   eTracerCol_MAX
};

#pragma pack(1)
typedef struct _TracerInfo
{
   uint32_t frame;
   uint32_t cycle;
   uint16_t addr;
   uint8_t  data;
   uint8_t  a;
   uint8_t  x;
   uint8_t  y;
   uint8_t  sp;
   uint8_t  f;
   uint32_t ea;
   uint8_t  disassemble [ 4 ];
   int8_t   type;
   int8_t   source;
   int8_t   target;
   int8_t   regsset;
   struct _TracerInfo* pCPUSample;
   struct _TracerInfo* pPPUSample;
} TracerInfo;
#pragma pack()

class CTracer
{
public:
   void ClearSampleBuffer ( void );
   inline TracerInfo* AddRESET ( void )
   {
      return AddSample ( 0, eTracer_RESET, eNESSource_CPU, 0, 0, 0 );
   }
   inline TracerInfo* AddNMI ( uint32_t cycle, int8_t source )
   {
      return AddSample ( cycle, eTracer_NMI, source, 0, 0, 0 );
   }
   inline TracerInfo* AddIRQ ( uint32_t cycle, int8_t source )
   {
      return AddSample ( cycle, eTracer_IRQ, source, 0, 0, 0 );
   }
   inline TracerInfo* AddIRQRelease ( uint32_t cycle, int8_t source )
   {
      return AddSample ( cycle, eTracer_IRQRelease, source, 0, 0, 0 );
   }
   inline TracerInfo* AddStolenCycle ( uint32_t cycle, int8_t source )
   {
      return AddSample ( cycle, eTracer_StolenCycle, source, 0, 0, 0 );
   }
   inline TracerInfo* AddGarbageFetch( uint32_t cycle, int8_t target, uint16_t addr )
   {
      return AddSample ( cycle, eTracer_GarbageRead, eNESSource_PPU, target, addr, 0 );
   }
   TracerInfo* AddSample ( uint32_t cycle, int8_t type, int8_t source, int8_t target, uint16_t addr, uint8_t data );
   bool ReallocateTracerMemory ( int32_t newDepth );
   unsigned int GetNumSamples ( void ) const
   {
      return m_samples;
   }
   TracerInfo* GetSample ( uint32_t sample );
   TracerInfo* GetCPUSample ( uint32_t sample );
   TracerInfo* GetPPUSample ( uint32_t sample );
   TracerInfo* GetLastSample ( void );
   TracerInfo* GetLastCPUSample ( void );
   void SetDisassembly ( TracerInfo* pS, uint8_t* szD );
   void SetRegisters ( TracerInfo* pS, uint8_t a, uint8_t x, uint8_t y, uint8_t sp, uint8_t f );
   void SetEffectiveAddress ( TracerInfo* pS, uint32_t ea )
   {
      if ( pS )
      {
         pS->ea = ea;
      }
   }

   CTracer();
   ~CTracer();

   unsigned int GetNumCPUSamples() const
   {
      return m_cpuSamples;
   }
   unsigned int GetNumPPUSamples() const
   {
      return m_ppuSamples;
   }

   void SetFrame(uint32_t frame)
   {
      m_frame = frame;
   }

protected:
   // Frame # is set by emulator so it doesn't have to be passed in all the time...
   uint32_t    m_frame;

   uint32_t    m_cursor;
   uint32_t    m_samples;
   uint32_t    m_sampleBufferDepth;

   uint32_t    m_cpuCursor;
   uint32_t    m_cpuSamples;
   uint32_t    m_ppuCursor;
   uint32_t    m_ppuSamples;

   TracerInfo* m_pSamples;
};

CTracer* nesGetExecutionTracerDatabase ( void );

#endif
