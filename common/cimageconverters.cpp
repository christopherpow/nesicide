#include "cimageconverters.h"

CImageConverters::CImageConverters()
{
}

QByteArray CImageConverters::fromIndexed8(QImage imgIn)
{
   QByteArray imgOut;
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
      imgOut.append(plane1,8);
      imgOut.append(plane2,8);
   }

   return imgOut;
}
