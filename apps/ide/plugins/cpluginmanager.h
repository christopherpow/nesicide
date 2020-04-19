#ifndef CPLUGINMANAGER_H
#define CPLUGINMANAGER_H

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <QString>
#include <QDomDocument>
#include <QHash>

#include "cbuildertextlogger.h"

class CPluginManager : public QObject
{
   Q_OBJECT
public:
   static CPluginManager *instance()
   {
      if ( !_instance )
      {
         _instance = new CPluginManager();
      }
      return _instance;
   }
   virtual ~CPluginManager();
   QString getVersionInfo();
   void doInitScript();
   void loadPlugins();
   void defineInterfaces(lua_State* lua);

   // Functions called by lua
   void lua_compiler_logger_print(QString text);

private:
   static CPluginManager *_instance;
   CPluginManager();

protected:
   lua_State* globalLuaInstance;
   int report(lua_State* L, int status);

   // Database of plugins
   static QHash<QString,QDomDocument*> plugins;
};

#endif // CPLUGINMANAGER_H
