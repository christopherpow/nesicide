// BasePalette.h: interface for the CBasePalette class.
//
//////////////////////////////////////////////////////////////////////

#if !defined ( NES_BASE_PALETTE_H )
#define NES_BASE_PALETTE_H

#include <stdint.h> // for standard base types...
#include <string.h> // for memcpy...
#include <stdio.h> // for sprintf...

#define RGB_VALUE(r,g,b) ( ((r&0xFF)<<24)|((g&0xFF)<<16)|((b&0xFF)<<8) )

inline void setRed(uint32_t rgb, uint8_t r)
{
   rgb&=(~(0xFF<<24));
   rgb|=(r<<24);
}
inline void setGreen(uint32_t rgb, uint8_t g)
{
   rgb&=(~(0xFF<<16));
   rgb|=(g<<16);
}
inline void setBlue(uint32_t rgb, uint8_t b)
{
   rgb&=(~(0xFF<<8));
   rgb|=(b<<8);
}

class CBasePalette
{
public:
   static inline uint8_t GetPaletteIndex ( int8_t red, int8_t green, int8_t blue )
   {
      int i;
      for ( i = 0; i < 64; i++ )
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
   static inline uint32_t GetPalette ( int idx, int bMonochrome = 0, int bEmphasizeRed = 0, int bEmphasizeGreen = 0, int bEmphasizeBlue = 0 )
   {
      if ( bMonochrome )
      {
         idx&= 0xF0;
      }

      return *(*(m_paletteVariants+((bEmphasizeRed)|((bEmphasizeGreen)<<1)|((bEmphasizeBlue)<<2)))+idx);
   }
   static inline int8_t GetPaletteR ( int idx, int bMonochrome = 0, int bEmphasizeRed = 0, int bEmphasizeGreen = 0, int bEmphasizeBlue = 0 )
   {
      if ( bMonochrome )
      {
         idx&= 0xF0;
      }

      return *(*(*(m_paletteRGBs+((bEmphasizeRed)|((bEmphasizeGreen)<<1)|((bEmphasizeBlue)<<2)))+idx));
   }
   static inline int8_t GetPaletteG ( int idx, int bMonochrome = 0, int bEmphasizeRed = 0, int bEmphasizeGreen = 0, int bEmphasizeBlue = 0 )
   {
      if ( bMonochrome )
      {
         idx&= 0xF0;
      }

      return *(*(*(m_paletteRGBs+((bEmphasizeRed)|((bEmphasizeGreen)<<1)|((bEmphasizeBlue)<<2)))+idx)+1);
   }
   static inline int8_t GetPaletteB ( int idx, int bMonochrome = 0, int bEmphasizeRed = 0, int bEmphasizeGreen = 0, int bEmphasizeBlue = 0 )
   {
      if ( bMonochrome )
      {
         idx&= 0xF0;
      }

      return *(*(*(m_paletteRGBs+((bEmphasizeRed)|((bEmphasizeGreen)<<1)|((bEmphasizeBlue)<<2)))+idx)+2);
   }
   static inline void SetPalette ( int idx, uint32_t color )
   {
      m_paletteVariants[0][idx] = color;
      CalculateVariants ();
   }
   static inline void SetPaletteR ( int idx, uint8_t r )
   {
      setRed(m_paletteVariants[0][idx],r);
      CalculateVariants ();
   }
   static inline void SetPaletteG ( int idx, uint8_t g )
   {
      setGreen(m_paletteVariants[0][idx],g);
      CalculateVariants ();
   }
   static inline void SetPaletteB ( int idx, uint8_t b )
   {
      setBlue(m_paletteVariants[0][idx],b);
      CalculateVariants ();
   }
   static void CalculateVariants ( void );
   static void RestoreBase ( void )
   {
      memcpy ( m_paletteVariants[0], m_paletteBase, sizeof(m_paletteBase) );
      CalculateVariants ();
   }
   CBasePalette()
   {
      memcpy ( m_paletteVariants[0], m_paletteBase, sizeof(m_paletteBase) );
      CalculateVariants ();
   };

protected:
   static int32_t m_paletteBase [ 64 ];
   static int32_t m_paletteVariants [ 8 ] [ 64 ];
   static int8_t   m_paletteRGBs [ 8 ] [ 64 ] [ 3 ];
};

#endif
