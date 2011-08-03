#include "cimageconverters.h"

CImageConverters::CImageConverters()
{
}

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
   int numTiles = (width/8)*(height/8);
   char plane1[8];
   char plane2[8];
   const uchar* bits = imgIn.bits();

   for ( tile = 0; tile < numTiles; tile++ )
   {
      tileX = (tile%tileWidth)*8;
      tileY = (tile/tileWidth)*8;

      for ( y = 0; y < 8; y++ )
      {
         for ( x = 0; x < 8; x++ )
         {
            plane1[y%8] <<= 1;
            plane2[y%8] <<= 1;

            plane1[y%8] |= bits[((tileY+y)*width)+tileX+x]&0x01;
            plane2[y%8] |= ((bits[((tileY+y)*width)+tileX+x]&0x02)>>1);
         }
      }
      chrOut.append(plane1,8);
      chrOut.append(plane2,8);
   }

   return chrOut;
}

QImage CImageConverters::toIndexed8(QByteArray chrIn)
{
   QImage imgOut(256,128,QImage::Format_Indexed8);

   int x;
   int y;
   int tile;
   int tileX;
   int tileY;
   int numTiles = chrIn.count()/0x10;
   char plane1;
   char plane2;
   char pixel;
   uchar* bits = imgOut.bits();

   imgOut.setNumColors(4);
   imgOut.setColorCount(4);
   imgOut.setColor(0,qRgb(0x00,0x00,0x00));
   imgOut.setColor(1,qRgb(0x40,0x40,0x40));
   imgOut.setColor(2,qRgb(0x80,0x80,0x80));
   imgOut.setColor(3,qRgb(0xC0,0xC0,0xC0));

   for ( tile = 0; tile < numTiles; tile++ )
   {
      tileX = (tile%16)*8;
      tileY = (tile/16)*8;

      if ( tile >= 256 )
      {
         tileX += 128;
         tileY -= 128;
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
