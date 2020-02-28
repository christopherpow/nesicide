#if !defined ( ROM_MAPPER005_H )
#define ROM_MAPPER005_H

#include "cnesrom.h"
#include "cnesapu.h"

class CNAMETABLEFILLER: public CMEMORY
{
public:
   CNAMETABLEFILLER() : CMEMORY(0x5106,1) {}
   ~CNAMETABLEFILLER() {};

   uint32_t TOTALSIZE() const { return 0; }

   inline uint8_t MEM (uint32_t addr);
   inline void MEM (uint32_t addr, uint8_t data);

protected:
   uint8_t m_tileFill;
   uint8_t m_attrFill;
};

class CROMMapper005 : public CROM
{
private:
   CROMMapper005();
public:
   static inline CROMMapper005* CARTFACTORY() { return new CROMMapper005(); }
   ~CROMMapper005();

   void RESET ( bool soft );
   uint32_t HMAPPER ( uint32_t addr );
   void HMAPPER ( uint32_t addr, uint8_t data );
   uint32_t LMAPPER ( uint32_t addr );
   void LMAPPER ( uint32_t addr, uint8_t data );
   void SYNCPPU ( uint32_t ppuCycle, uint32_t ppuAddr );
   void SYNCCPU ( bool write, uint16_t addr, uint8_t data );
   void SETCPU ( void );
   void SETPPU ( void );
   uint32_t DEBUGINFO ( uint32_t addr );
   uint16_t AMPLITUDE ( void );
   static void SOUNDENABLE ( uint32_t mask ) { m_soundEnableMask = mask; }

protected:
   // MMC5
   CNAMETABLEFILLER* m_pFILLmemory;
   uint8_t m_prgMode;
   uint8_t m_chrMode;
   uint8_t m_chrHigh;
   uint8_t m_irqScanline;
   uint8_t m_irqEnabled;
   uint8_t m_irqStatus;
   bool          m_prgRAM [ 3 ];
   bool          m_wp;
   uint32_t m_ppuCycle;
   uint16_t m_chr[12];
   uint8_t m_wp1;
   uint8_t m_wp2;
   uint8_t  m_mult1;
   uint8_t  m_mult2;
   uint16_t m_timer;
   bool     m_timerIrq;
   uint16_t m_prod;
   uint8_t m_8x16e;
   uint8_t m_8x16z;
   uint8_t  m_reg[46];

   CAPUSquare m_square[2];
   CAPUDMC    m_dmc;

   static uint32_t  m_soundEnableMask;
};

#endif
