// BasePalette.h: interface for the CBasePalette class.
//
//////////////////////////////////////////////////////////////////////

#if !defined ( BASE_PALETTE_H )
#define BASE_PALETTE_H

#include "cnesicidecommon.h"
#include <QColor>

class CBasePalette  
{
public:
   static inline QColor GetPalette ( int idx, int bMonochrome = 0, int bEmphasizeRed = 0, int bEmphasizeGreen = 0, int bEmphasizeBlue = 0 )
   {
      if ( bMonochrome )
      {
         idx&= 0xF0;
      }
      return *(*(m_paletteVariants+((bEmphasizeRed)|((bEmphasizeGreen)<<1)|((bEmphasizeBlue)<<2)))+idx);
   }
   static inline QColor GetDisplayPalette ( int idx, int bMonochrome = 0, int bEmphasizeRed = 0, int bEmphasizeGreen = 0, int bEmphasizeBlue = 0 )
   { 
      if ( bMonochrome )
      {
         idx&= 0xF0;
      }
      return *(*(m_paletteDisplayVariants+((bEmphasizeRed)|((bEmphasizeGreen)<<1)|((bEmphasizeBlue)<<2)))+idx);
   }
   static inline void SetPalette ( int idx, QColor color )
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
   static QColor m_paletteBase [ 64 ];
   static QColor m_paletteVariants [ 8 ] [ 64 ];
   static QColor m_paletteDisplayVariants [ 8 ] [ 64 ];
};

#endif // !defined(AFX_BASEPALETTE_H__1F178478_2CEF_4CDC_B006_95C428BF9926__INCLUDED_)
