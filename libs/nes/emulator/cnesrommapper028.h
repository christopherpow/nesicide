#if !defined ( ROM_MAPPER028_H )
#define ROM_MAPPER028_H

#include "cnesrom.h"

class CROMMapper028 : public CROM
{
private:
   CROMMapper028();
public:
   static inline CROMMapper028* CARTFACTORY() { return new CROMMapper028(); }
   virtual ~CROMMapper028();

   void RESET ( bool soft );
   void HMAPPER ( uint32_t addr, uint8_t data );
   uint32_t LMAPPER ( uint32_t addr );
   void LMAPPER ( uint32_t addr, uint8_t data );
   void SETCPU ( void );
   void SETPPU ( void );
   uint32_t DEBUGINFO ( uint32_t addr );

   // Internal accessors for mapper information inspector...
   // Note: called directly!
   uint32_t REGSEL ( void )
   {
      return m_reg_sel;
   }
   uint32_t PRGMODE ( void )
   {
      return m_prg_mode;
   }
   uint32_t PRGSIZE ( void )
   {
      return m_prg_size;
   }
   uint32_t PRGOUTERBANK ( void )
   {
      return m_prg_outer_bank;
   }
   uint32_t PRGINNERBANK ( void )
   {
      return m_prg_inner_bank;
   }
   uint32_t CHRBANK ( void )
   {
      return m_chr_bank;
   }

protected:
   // Streemerz Collection mapper
   uint8_t m_reg_sel;	//register select S: supervisor=1/user=0 (D7),  R: register select (D0)
   //In a multicart, registers $00 and $01 change the bank within a game, and registers $80 and $81
   //remain constant throughout a given game's execution.

   //$00 CHR bank:	used for CNROM, and one screen mirroring
   uint8_t m_chr_bank;	//D1-0 set CHR RAM A14-13

   //$01 Inner bank: prg bank for A/B/UxROM etc.
   uint8_t m_prg_inner_bank;	//D4-0 set PRG ROM A14/15 and up for single game in game use

   //$80 Mode: used by multicart supervisor software
   uint8_t m_prg_size;	//D5-4	0: 32KB, 1: 64KB, 2:128KB, 3:256KB
   uint8_t m_prg_mode;	//D3-2	0,1: current 32KB @ $8000, 2: UNROM standard, 3: UNROM #180
   uint8_t m_mirror;	//D1-0	0: 1scn lower, 1: 1scn upper, 2: vert, 3: horiz

   //$81 Outer bank: sets the upper PRG ROM bits to select current game
   uint8_t m_prg_outer_bank;	//sets PRG ROM A15-20

   uint8_t m_bank_size_mask[4];
};

#endif
