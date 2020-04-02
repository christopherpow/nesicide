#include "cgamedatabasehandler.h"

#include <QResource>
#include <QFile>
#include <QCryptographicHash>

#include "nes_emulator_core.h"

CGameDatabaseHandler *CGameDatabaseHandler::_instance = NULL;

CGameDatabaseHandler::CGameDatabaseHandler()
{
}

bool CGameDatabaseHandler::initialize(QString fileName)
{
   QFile file(fileName);
   QFile res(":/GameDatabase");
   bool openedFile = false;

   // First attempt to open the user-specified game database...
   file.open(QIODevice::ReadOnly);

   if ( file.isOpen() )
   {
      m_db.clear();
      m_db.setContent(&file);
      file.close();
      openedFile = true;
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
   return openedFile;
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
   for ( i = 0; i < pCartridge->getPrgRomBanks()->getPrgRomBanks().count(); i++ )
   {
      sha1alg.addData((char*)pCartridge->getPrgRomBanks()->getPrgRomBanks().at(i)->getBankData(),MEM_8KB);
   }

   for ( i = 0; i < pCartridge->getChrRomBanks()->getChrRomBanks().count(); i++ )
   {
      sha1alg.addData((char*)pCartridge->getChrRomBanks()->getChrRomBanks().at(i)->getBankData(),MEM_8KB);
   }

   // Get the resulting hash value from the crypto...
   sha1key = sha1alg.result();

   // Search the loaded game database for the corresponding hash value...
   QDomNode gameNode = docElem.firstChild();

   while (!gameNode.isNull())
   {
      QDomElement gameElem = gameNode.toElement(); // try to convert the node to an element.

      if (!gameElem.isNull())
      {
         QDomNode cartridgeNode = gameElem.firstChild();

         while (!cartridgeNode.isNull())
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
