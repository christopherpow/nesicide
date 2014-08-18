#include "cimageconverters.h"
#include "cnessystempalette.h"
#include "cdesignercommon.h"

QByteArray CImageConverters::fromIndexed8(QImage imgIn)
{
   QByteArray chrOut;
   int x = 0;
   int y = 0;
   int width = imgIn.width();
   int height = imgIn.height();
   int tile;
   int tileWidth = width/8;
   int tileHeight = height/8;
   int tileX;
   int tileY;
   int numTiles = (tileWidth*tileHeight);
   char plane1[8];
   char plane2[8];
   const uchar* bits = imgIn.bits();

   if ( tileWidth > 16 ) tileWidth = 16;

   for ( tile = 0; tile < numTiles; tile++ )
   {
      tileX = (tile%tileWidth)*8;
      tileY = (tile/tileWidth)*8;

      if ( tile >= 256 )
      {
         tileX += 128;
         tileY -= 128;
      }

      for ( y = 0; y < 8; y++ )
      {
         for ( x = 0; x < 8; x++ )
         {
            plane1[y] <<= 1;
            plane2[y] <<= 1;

            plane1[y] |= bits[((tileY+y)*width)+tileX+x]&0x01;
            plane2[y] |= ((bits[((tileY+y)*width)+tileX+x]&0x02)>>1);
         }
      }
      chrOut.append(plane1,8);
      chrOut.append(plane2,8);
   }

   return chrOut;
}

QImage CImageConverters::toIndexed8(QByteArray chrIn, int xSize, int ySize)
{
   QImage imgOut(xSize,ySize,QImage::Format_Indexed8);

   int x;
   int y;
   int tile;
   int tileX;
   int tileY;
   int tileWidth = xSize/8;
   int tileHeight = ySize/8;
   int numTiles = chrIn.count()/0x10;
   char plane1;
   char plane2;
   char pixel;

   imgOut.setColorCount(16);
   imgOut.setColor(0,qRgb(0x00,0x00,0x00));
   imgOut.setColor(1,qRgb(0x40,0x40,0x40));
   imgOut.setColor(2,qRgb(0x80,0x80,0x80));
   imgOut.setColor(3,qRgb(0xC0,0xC0,0xC0));

   // Constrain to "left-and-right banks" for 256x128 CHR image if necessary.
   if ( (tileHeight <= 16) && (tileWidth > 16) ) tileWidth = 16;

   for ( tile = 0; tile < numTiles; tile++ )
   {
      tileX = (tile%tileWidth)*8;
      tileY = (tile/tileWidth)*8;

      // Constrain to "left-and-right banks" for 256x128 CHR image if necessary.
      if ( tileHeight <= 16 )
      {
         if ( tile >= 256 )
         {
            tileX += 128;
            tileY -= 128;
         }
      }

      for ( y = 0; y < 8; y++ )
      {
         plane1 = chrIn.at((tile<<4)+y);
         plane2 = chrIn.at((tile<<4)+y+8);

         for ( x = 0; x < 8; x++ )
         {
            pixel = (!!(plane1&0x80))
                  | ((!!(plane2&0x80))<<1);
            imgOut.setPixel(tileX+x,tileY+y,pixel);
            plane1 <<= 1;
            plane2 <<= 1;
         }
      }
   }

   return imgOut;
}

QImage CImageConverters::toIndexed8(QByteArray chrIn, QList<int> colorTable, int xSize, int ySize)
{
   QImage imgOut(xSize,ySize,QImage::Format_Indexed8);

   int x;
   int y;
   int tile;
   int tileX;
   int tileY;
   int tileWidth = xSize/8;
   int tileHeight = ySize/8;
   int numTiles = chrIn.count()/0x10;
   char plane1;
   char plane2;
   char pixel;
   int idx;

   // Constrain to "left-and-right banks" for 256x128 CHR image if necessary.
   if ( (tileHeight <= 16) && (tileWidth > 16) ) tileWidth = 16;

   imgOut.setColorCount(16);
   for ( idx = 0; idx < colorTable.count(); idx++ )
   {
      imgOut.setColor(idx,qRgb(CBasePalette::GetPaletteR(colorTable.at(idx)),CBasePalette::GetPaletteG(colorTable.at(idx)),CBasePalette::GetPaletteB(colorTable.at(idx))));
   }

   for ( tile = 0; tile < numTiles; tile++ )
   {
      tileX = (tile%tileWidth)*8;
      tileY = (tile/tileWidth)*8;

      // Constrain to "left-and-right banks" for 256x128 CHR image if necessary.
      if ( tileHeight <= 16 )
      {
         if ( tile >= 256 )
         {
            tileX += 128;
            tileY -= 128;
         }
      }

      for ( y = 0; y < 8; y++ )
      {
         plane1 = chrIn.at((tile<<4)+y);
         plane2 = chrIn.at((tile<<4)+y+8);

         for ( x = 0; x < 8; x++ )
         {
            pixel = (!!(plane1&0x80))
                  | ((!!(plane2&0x80))<<1);
            imgOut.setPixel(tileX+x,tileY+y,pixel);
            plane1 <<= 1;
            plane2 <<= 1;
         }
      }
   }

   return imgOut;
}

QImage CImageConverters::toIndexed8(QByteArray chrIn, QByteArray attrIn, QList<uint8_t> colorTable, int xSize, int ySize)
{
   QImage imgOut(xSize,ySize,QImage::Format_Indexed8);

   int x;
   int y;
   int tile;
   int tileX;
   int tileY;
   int tileWidth = xSize/8;
   int tileHeight = ySize/8;
   int numTiles = chrIn.count()/0x10;
   char plane1;
   char plane2;
   char plane34;
   char pixel;
   int attrQuadsX;
   int attrQuadsY;
   int attrQuadX;
   int attrQuadY;
   int attrQuad;
   int idx;

   // Constrain to "left-and-right banks" for 256x128 CHR image if necessary.
   if ( (tileHeight <= 16) && (tileWidth > 16) ) tileWidth = 16;

   attrQuadsX = (tileWidth>>2);
   attrQuadsY = (tileHeight>>2);

   if ( attrQuadsX == 0 ) attrQuadsX = 1;
   if ( attrQuadsY == 0 ) attrQuadsY = 1;

   imgOut.setColorCount(16);
   for ( idx = 0; idx < colorTable.count(); idx++ )
   {
      imgOut.setColor(idx,qRgb(CBasePalette::GetPaletteR(colorTable.at(idx)),CBasePalette::GetPaletteG(colorTable.at(idx)),CBasePalette::GetPaletteB(colorTable.at(idx))));
   }

   for ( tile = 0; tile < numTiles; tile++ )
   {
      tileX = (tile%tileWidth)*8;
      tileY = (tile/tileWidth)*8;

      // Constrain to "left-and-right banks" for 256x128 CHR image if necessary.
      if ( tileHeight <= 16 )
      {
         if ( tile >= 256 )
         {
            tileX += 128;
            tileY -= 128;
         }
      }

      for ( y = 0; y < 8; y++ )
      {
         // Get bitplanes from image data.
         plane1 = chrIn.at((tile<<4)+y);
         plane2 = chrIn.at((tile<<4)+y+8);

         // Get bitplanes from attribute data.
         attrQuadX = PIXEL_TO_ATTRQUAD(tileX);
         attrQuadY = PIXEL_TO_ATTRQUAD(tileY);
         attrQuad = (attrQuadY*attrQuadsX)+attrQuadX;
         if ( attrQuad < attrIn.count() )
         {
            plane34 = attrIn.at(attrQuad);
         }
         else
         {
            plane34 = 0x00;
         }
         plane34 >>= (PIXEL_TO_ATTRSECTION(tileX,tileY)<<1);
         plane34 &= 0x03;
         plane34 <<= 2;

         for ( x = 0; x < 8; x++ )
         {
            pixel = (!!(plane1&0x80))
                  | ((!!(plane2&0x80))<<1)
                  | plane34;
            imgOut.setPixel(tileX+x,tileY+y,pixel);
            plane1 <<= 1;
            plane2 <<= 1;
         }
      }
   }

   return imgOut;
}
