// ctracer.h: interface for the CTracer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined ( TRACER_H )
#define TRACER_H

#define TRACER_DEFAULT_DEPTH 262144

enum
{
   eTracer_Unknown = 0,
   eTracer_InstructionFetch,
   eTracer_DataRead,
   eTracer_DataWrite,
   eTracer_DMA,
   eTracer_RESET,
   eTracer_NMI,
   eTracer_IRQ,
   eTracer_GarbageRead,
   eTracer_RenderBkgnd,
   eTracer_RenderSprite,
   eTracer_Sprite0Hit
};

enum
{
   eSource_Unknown = 0,
   eSource_CPU,
   eSource_PPU,
   eSource_APU,
   eSource_Mapper,
   eSource_User
};

enum
{
   eTarget_Unknown = 0,
   eTarget_RAM,
   eTarget_PPURegister,
   eTarget_APURegister,
   eTarget_IORegister,
   eTarget_SRAM,
   eTarget_ROM,
   eTarget_Mapper,
   eTarget_PatternMemory,
   eTarget_NameTable,
   eTarget_AttributeTable,
   eTarget_Palette
};

enum
{
   eTracerCol_Cycle = 0,
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
typedef struct
{
   unsigned int cycle;
   unsigned short addr;
   unsigned char  data;
   unsigned char a;
   unsigned char x;
   unsigned char y;
   unsigned char sp;
   unsigned char f;
   unsigned int  ea;
   unsigned char disassemble [ 4 ];
   char type;
   char source;
   char target;
   char regsset;
} TracerInfo;
#pragma pack()

class CTracer  
{
public:
	void ClearSampleBuffer ( void );
	TracerInfo* AddRESET ( void );
	TracerInfo* AddNMI ( char source );
	TracerInfo* AddIRQ ( char source );
   TracerInfo* AddGarbageFetch( unsigned int cycle, char target );
	TracerInfo* AddSample ( unsigned int cycle, char type, char source, char target, unsigned short addr, unsigned char data );
	bool ReallocateTracerMemory ( int newDepth );
   unsigned int GetNumSamples ( void ) const { return m_samples; }
   TracerInfo* GetSample ( unsigned int sample );
   TracerInfo* GetLastSample ( void );
   TracerInfo* SetDisassembly ( unsigned char* szD );
   TracerInfo* SetRegisters ( TracerInfo* pS, unsigned char a, unsigned char x, unsigned char y, unsigned char sp, unsigned char f );
   void SetEffectiveAddress ( TracerInfo* pS, unsigned int ea ) { if ( pS ) pS->ea = ea; }
//   int Find ( CString str, int start );

	CTracer();
   CTracer(int depth);
	virtual ~CTracer();

protected:
   unsigned int m_cursor;
   unsigned int m_samples;
   unsigned int m_sampleBufferDepth;
   TracerInfo* m_pSampleBuffer;
};

#endif // !defined(AFX_TRACER_H__19AABF5C_F4D9_4381_81E5_6BF73C9B74B1__INCLUDED_)
