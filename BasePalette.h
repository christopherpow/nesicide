// BasePalette.h: interface for the CBasePalette class.
//
//////////////////////////////////////////////////////////////////////

#if !defined ( BASE_PALETTE_H )
#define BASE_PALETTE_H

#include "NESICIDECommon.h"

class CBasePalette  
{
public:
   static inline COLORREF GetPalette ( int idx, int bMonochrome = 0, int bEmphasizeRed = 0, int bEmphasizeGreen = 0, int bEmphasizeBlue = 0 ) 
   { 
      if ( bMonochrome )
      {
         return *(*(m_paletteVariants+((bEmphasizeRed)|((bEmphasizeGreen)<<1)|((bEmphasizeBlue)<<2)))+(idx&0xF0));
      }
      else
      {
         return *(*(m_paletteVariants+((bEmphasizeRed)|((bEmphasizeGreen)<<1)|((bEmphasizeBlue)<<2)))+idx);
      }
   }
   static inline COLORREF GetDisplayPalette ( int idx, int bMonochrome = 0, int bEmphasizeRed = 0, int bEmphasizeGreen = 0, int bEmphasizeBlue = 0 ) 
   { 
      if ( bMonochrome )
      {
         return *(*(m_paletteDisplayVariants+((bEmphasizeRed)|((bEmphasizeGreen)<<1)|((bEmphasizeBlue)<<2)))+(idx&0xF0));
      }
      else
      {
         return *(*(m_paletteDisplayVariants+((bEmphasizeRed)|((bEmphasizeGreen)<<1)|((bEmphasizeBlue)<<2)))+idx);
      }
   }
   static inline void SetPalette ( int idx, COLORREF color ) 
   { 
      m_paletteVariants[0][idx] = color;
      CalculateVariants ();
   }
   static void CalculateVariants ( void );
   static void RestoreBase ( void )
   {
      memcpy ( m_paletteVariants[0], m_paletteBase, sizeof(m_paletteBase) );
      CalculateVariants ();
   }
// CPTODO: use Qt serialization to store project palette...
//   static void Serialize ( CArchive& ar );

   CBasePalette() 
   {
      // TODO: Create logical palette to speed things up?
      memcpy ( m_paletteVariants[0], m_paletteBase, sizeof(m_paletteBase) );
      CalculateVariants ();
   };

protected:
	static COLORREF m_paletteBase [ 64 ];
   static COLORREF m_paletteVariants [ 8 ] [ 64 ];
   static COLORREF m_paletteDisplayVariants [ 8 ] [ 64 ];
};

#endif // !defined(AFX_BASEPALETTE_H__1F178478_2CEF_4CDC_B006_95C428BF9926__INCLUDED_)
