#if !defined ( ROM_MAPPER028_H )
#define ROM_MAPPER028_H

#include "cnesrom.h"

class CROMMapper028 : public CROM
{
public:
   CROMMapper028();
   virtual ~CROMMapper028();

   static void RESET ();
   static uint32_t MAPPER ( uint32_t addr );
   static void MAPPER ( uint32_t addr, uint8_t data );
   static uint32_t LMAPPER ( uint32_t addr );
   static void LMAPPER ( uint32_t addr, uint8_t data );
   static void SETCPU ( void );
   static void SETPPU ( void );
   static void LOAD ( MapperState* data );
   static void SAVE ( MapperState* data );

   // Internal accessors for mapper information inspector...
   // Note: called directly!
   static uint32_t PRGMODE ( void )
   {
      return m_prg_mode;
   }
   static uint32_t PRGSIZE ( void )
   {
      return m_prg_size;
   }
   static uint32_t PRGOUTERBANK ( void )
   {
      return m_prg_outer_bank;
   }
   static uint32_t PRGINNERBANK ( void )
   {
      return m_prg_inner_bank;
   }

protected:
   // Streemerz Collection mapper
   static uint8_t m_reg_sel;	//register select S: supervisor=1/user=0 (D7),  R: register select (D0)
   //In a multicart, registers $00 and $01 change the bank within a game, and registers $80 and $81
   //remain constant throughout a given game's execution.

   //$00 CHR bank:	used for CNROM, and one screen mirroring
   static uint8_t m_chr_bank;	//D1-0 set CHR RAM A14-13

   //$01 Inner bank: prg bank for A/B/UxROM etc.
   static uint8_t m_prg_inner_bank;	//D4-0 set PRG ROM A14/15 and up for single game in game use

   //$80 Mode: used by multicart supervisor software
   static uint8_t m_prg_size;	//D5-4	0: 32KB, 1: 64KB, 2:128KB, 3:256KB
   static uint8_t m_prg_mode;	//D3-2	0,1: current 32KB @ $8000, 2: UNROM standard, 3: UNROM #180
   static uint8_t m_mirror;	//D1-0	0: 1scn lower, 1: 1scn upper, 2: vert, 3: horiz

   //$81 Outer bank: sets the upper PRG ROM bits to select current game
   static uint8_t m_prg_outer_bank;	//sets PRG ROM A15-20

   static uint8_t m_bank_size_mask[4];
};

#endif
