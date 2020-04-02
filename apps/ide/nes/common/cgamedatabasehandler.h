#ifndef CGAMEDATABASEHANDLER_H
#define CGAMEDATABASEHANDLER_H

#include <QDomDocument>
#include <QDomElement>
#include <QString>

#include "ccartridge.h"

#define GAME_DATA(attr)  QDomElement gameElem = m_game.toElement(); return gameElem.attribute(attr)
#define CART_DATA(attr)  QDomElement cartridgeElem = m_cartridge.toElement(); return cartridgeElem.attribute(attr)

class CGameDatabaseHandler
{
public:
   static CGameDatabaseHandler *instance()
   {
      if ( !_instance )
      {
         _instance = new CGameDatabaseHandler();
      }
      return _instance;
   }
   bool initialize(QString fileName);

   // Database information.
   QString getGameDBTimestamp();
   QString getGameDBAuthor();

   // Database searching.
   bool find(CCartridge* pCartridge);

   // Game values.
   QString getName()
   {
      GAME_DATA("name");
   }
   QString getPublisher()
   {
      GAME_DATA("publisher");
   }
   QString getDate()
   {
      GAME_DATA("date");
   }
   int getRegion();

   // Cartridge values.
   QString getSystem()
   {
      CART_DATA("system");
   }
   QString getSHA1()
   {
      CART_DATA("sha1").toUpper();
   }
private:
   static CGameDatabaseHandler *_instance;
   CGameDatabaseHandler();

protected:
   QDomDocument m_db;
   QDomNode     m_game;
   QDomNode     m_cartridge;
};

#endif // CGAMEDATABASEHANDLER_H
