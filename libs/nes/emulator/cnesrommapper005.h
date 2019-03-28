#if !defined ( ROM_MAPPER005_H )
#define ROM_MAPPER005_H

#include "cnesrom.h"
#include "cnesapu.h"

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
   void SYNCCPU ( void );
   void SETCPU ( void );
   void SETPPU ( void );
   uint32_t DEBUGINFO ( uint32_t addr );
   uint16_t AMPLITUDE ( void );
   static void SOUNDENABLE ( uint32_t mask ) { m_soundEnableMask = mask; }

protected:
   // MMC5
   uint8_t m_prgMode;
   uint8_t m_chrMode;
   uint8_t m_chrHigh;
   uint8_t m_irqScanline;
   uint8_t m_irqEnabled;
   uint8_t m_irqStatus;
   bool          m_prgRAM [ 3 ];
   bool          m_wp;
   uint32_t m_ppuCycle;
   uint8_t m_chr[12];
   uint8_t m_wp1;
   uint8_t m_wp2;
   uint8_t  m_mult1;
   uint8_t  m_mult2;
   uint16_t m_prod;
   uint8_t  m_fillTile;
   uint8_t  m_fillAttr;
   uint8_t  m_reg[44];

   CAPUSquare m_square[2];
   CAPUDMC    m_dmc;

   static uint32_t  m_soundEnableMask;
};

#endif
