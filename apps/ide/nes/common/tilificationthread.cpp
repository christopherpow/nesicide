#include "tilificationthread.h"

#include "nes_emulator_core.h"

TilificationThread::TilificationThread(QObject *parent) :
   QThread(parent)
{
}

void TilificationThread::prepareToTilify()
{
   m_input.clear();
   m_output.clear();
}

void TilificationThread::addToTilificator(IChrRomBankItem* input)
{
   m_input.append(input);
}

void TilificationThread::tilify()
{
   start();
}

void TilificationThread::run()
{
   QByteArray tileDataIn;
   QByteArray tileDataLockMap;
   int idx1;
   int idx2;
   bool newTile;
   unsigned int* pSrc;
   unsigned int* pDst;
   const char* tileSrc;
   const char* tileDst;

#if 1
// disable tilification for now until I sort out how to incorporate it best.
   for ( idx1 = 0; idx1 < m_input.count(); idx1++ )
   {
      m_output.append(m_input.at(idx1)->getChrRomBankItemData());
   }
#else
   for ( idx1 = 0; idx1 < m_input.count(); idx1++ )
   {
      tileDataIn.append(m_input.at(idx1)->getChrRomBankItemData());
      if ( m_input.at(idx1)->getItemType() == "Tile" )
      {
         for ( idx2 = 0; idx2 < m_input.at(idx1)->getChrRomBankItemData().count()>>4; idx2++ )
         {
            tileDataLockMap.append('0');
         }
      }
      else
      {
         for ( idx2 = 0; idx2 < m_input.at(idx1)->getChrRomBankItemData().count()>>4; idx2++ )
         {
            tileDataLockMap.append('1');
         }
      }
   }

   for ( idx1 = 0; idx1 < tileDataIn.count(); idx1 += 16)
   {
      // Go through the tile data and 'compress' it by
      // keeping only unique tiles.
      // Pointer for tile data.
      tileSrc = tileDataIn.constData();

      // First tile always gets put in.
      if ( (tileDataLockMap.at(idx1>>4) == '1') || idx1 == 0 )
      {
         m_output.append(tileSrc,16);
      }
      else
      {
         // Go through tiles in the output list already.
         newTile = true;
         for ( idx2 = 0; idx2 < m_output.count(); idx2 += 16)
         {
            // Pointer for tile data.
            tileDst = m_output.constData();
            pSrc = (unsigned int*)(tileSrc+idx1);
            pDst = (unsigned int*)(tileDst+idx2);

            // If the tiles match, add this tile.
            if ( (*pSrc == *pDst) &&
                 (*(pSrc+2) == *(pDst+2)) &&
                 (*(pSrc+1) == *(pDst+1)) &&
                 (*(pSrc+3) == *(pDst+3)) )
            {
               // Tile already copied.
               newTile = false;
               break;
            }
         }

         // Copy tile if needed.
         if ( newTile )
         {
            m_output.append(tileSrc+idx1,16);

            // Go through original tile data and remove the tile we just found.
            for ( idx2 = idx1+16; idx2 < tileDataIn.count(); )
            {
               // Pointer for tile data.
               tileDst = tileDataIn.constData();
               pSrc = (unsigned int*)(tileSrc+idx1);
               pDst = (unsigned int*)(tileDst+idx2);

               // If the tiles match, remove this tile.
               if ( (*pSrc == *pDst) &&
                    (*(pSrc+2) == *(pDst+2)) &&
                    (*(pSrc+1) == *(pDst+1)) &&
                    (*(pSrc+3) == *(pDst+3)) )
               {
                  tileDataIn.remove(idx2,16);
               }
               else
               {
                  idx2 += 16;
               }
            }
         }
      }
   }
#endif

   emit tilificationComplete(m_output);
}
