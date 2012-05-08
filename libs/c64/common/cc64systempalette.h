// BasePalette.h: interface for the CBasePalette class.
//
//////////////////////////////////////////////////////////////////////

#if !defined ( C64_BASE_PALETTE_H )
#define C64_BASE_PALETTE_H

#include <stdint.h> // for standard base types...
#include <string.h> // for memcpy...
#include <stdio.h> // for sprintf...

#define RGB_VALUE(r,g,b) ( ((r&0xFF)<<24)|((g&0xFF)<<16)|((b&0xFF)<<8) )

class CBasePalette
{
public:
   static inline uint8_t GetPaletteIndex ( int8_t red, int8_t green, int8_t blue )
   {
      int i;
      for ( i = 0; i < 16; i++ )
      {
         if ( (red == GetPaletteR(i)) &&
              (blue == GetPaletteB(i)) &&
              (green == GetPaletteG(i)) )
         {
            return i;
         }
      }
      return -1;
   }
   static inline uint32_t GetPalette ( int idx )
   {
      return *(m_paletteBase+idx);
   }
   static inline int8_t GetPaletteR ( int idx )
   {
      return *(*(m_paletteRGBs+idx)+0);
   }
   static inline int8_t GetPaletteG ( int idx )
   {
      return *(*(m_paletteRGBs+idx)+1);
   }
   static inline int8_t GetPaletteB ( int idx )
   {
      return *(*(m_paletteRGBs+idx)+2);
   }
   static void CalculateVariants ( void );
   CBasePalette()
   {
      CalculateVariants ();
   };

protected:
   static uint32_t m_paletteBase[16];
   static int8_t   m_paletteRGBs[16][3];
};

#endif
