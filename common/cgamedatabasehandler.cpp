#include "cgamedatabasehandler.h"

#include <QResource>
#include <QFile>
#include <QCryptographicHash>

#include <cnes.h>

CGameDatabaseHandler::CGameDatabaseHandler()
{
}

void CGameDatabaseHandler::initialize(QString fileName)
{
   QFile file(fileName);
   QFile res(":/resources/NesCarts (2010-02-08).xml");

   // First attempt to open the user-specified game database...
   file.open(QIODevice::ReadOnly);
   if ( file.isOpen() )
   {
      m_db.clear();
      m_db.setContent(&file);
      file.close();
   }
   else
   {
      // Couldn't open the user-specified game database, resort
      // to using the internal resource...
      res.open(QIODevice::ReadOnly);

      m_db.clear();
      m_db.setContent(&res);
      res.close();
   }
}

QString CGameDatabaseHandler::getGameDBTimestamp()
{
   QDomElement        docElem = m_db.documentElement();
   return docElem.attribute("timestamp");
}

QString CGameDatabaseHandler::getGameDBAuthor()
{
   QDomElement        docElem = m_db.documentElement();
   return docElem.attribute("author");
}

bool CGameDatabaseHandler::find(CCartridge* pCartridge)
{
   QDomElement        docElem = m_db.documentElement();
   QCryptographicHash sha1alg(QCryptographicHash::Sha1);
   QByteArray         sha1key;
   int                i;

   // Reset the crypto...
   sha1alg.reset();

   // Clear the found elements...
   m_game.clear();
   m_cartridge.clear();

   // Pump ROM data into crypto to get SHA1...
   for ( i = 0; i < pCartridge->getPointerToPrgRomBanks()->get_pointerToArrayOfBanks()->count(); i++ )
   {
      sha1alg.addData((char*)pCartridge->getPointerToPrgRomBanks()->get_pointerToArrayOfBanks()->at(i)->get_pointerToBankData(),0x4000);
   }
   for ( i = 0; i < pCartridge->getPointerToChrRomBanks()->banks.count(); i++ )
   {
      sha1alg.addData((char*)pCartridge->getPointerToChrRomBanks()->banks.at(i)->data,0x2000);
   }

   // Get the resulting hash value from the crypto...
   sha1key = sha1alg.result();

   // Search the loaded game database for the corresponding hash value...
   QDomNode gameNode = docElem.firstChild();
   while(!gameNode.isNull())
   {
      QDomElement gameElem = gameNode.toElement(); // try to convert the node to an element.
      if(!gameElem.isNull())
      {
         QDomNode cartridgeNode = gameElem.firstChild();
         while(!cartridgeNode.isNull())
         {
            QDomElement cartridgeElem = cartridgeNode.toElement(); // try to convert the node to an element.

            if ( sha1key.toHex().toUpper() == cartridgeElem.attribute("sha1").toUpper() )
            {
               // Save found game for later reference...
               m_game = gameNode.cloneNode();
               m_cartridge = cartridgeNode.cloneNode();
               return true;
            }
            cartridgeNode = cartridgeNode.nextSibling();
         }
      }
      gameNode = gameNode.nextSibling();
   }
   return false;
}

int CGameDatabaseHandler::getRegion()
{
   QDomElement cartridgeElem = m_cartridge.toElement();
   QString str = cartridgeElem.attribute("system");
   if ( str.contains("USA") )
   {
      return MODE_NTSC;
   }
   else
   {
      return MODE_PAL;
   }
}
