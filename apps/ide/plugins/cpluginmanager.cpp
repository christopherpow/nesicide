#include "cpluginmanager.h"

#if defined(Q_OS_MAC) || defined(Q_OS_MACX) || defined(Q_OS_MAC64)
#include <sys/stat.h>
#endif

#include <QCoreApplication>
#include <QDir>

static int luabind_compiler_logger_print(lua_State* lua);

CPluginManager *CPluginManager::_instance = NULL;

QHash<QString,QDomDocument*> CPluginManager::plugins;

CPluginManager::CPluginManager()
{
   globalLuaInstance = lua_open();
   defineInterfaces(globalLuaInstance);
}

void CPluginManager::doInitScript()
{
#ifdef Q_OS_WIN
   const char* initScriptPath   = "../plugins/init.lua";
#else
   const char* initScriptPath   = "plugins/init.lua";
#endif
   int status;
   QString result;
#if defined(Q_OS_MAC) || defined(Q_OS_MACX) || defined(Q_OS_MAC64)
   struct stat stFileInfo;
   QString filePath = initScriptPath;

   if (!stat((filePath = QString(initScriptPath)).toLatin1().constData(), &stFileInfo))
   {
      // the script is in a plugin folder in the same folder as a distributed application
   }
   else if (!stat((filePath = QString("nesicide.app/Contents/Resources/")+initScriptPath).toLatin1().constData(), &stFileInfo))
   {
      // normal path to scripts when dubbel clicking on an application
   }
   else if (!stat((filePath = QString("../../../")+initScriptPath).toLatin1().constData(), &stFileInfo))
   {
      // the script is in a plugin folder in the same folder as an application that's debugged with Qt
   }
   else if (!stat((filePath = QString("../Resources/")+initScriptPath).toLatin1().constData(), &stFileInfo))
   {
      // path to find scripts when using the Qt debuger
   }
   else
   {
      // no more place to look for the script, bailing out... lua will do the error handling though
   }

   report(globalLuaInstance, status = luaL_dofile(globalLuaInstance, filePath.toLatin1().constData()));
#else
   QString filePath = QCoreApplication::applicationDirPath()+"/"+QString(initScriptPath);
   report(globalLuaInstance, status = luaL_dofile(globalLuaInstance, filePath.toLatin1().constData()));
#endif
}

void CPluginManager::loadPlugins()
{
#ifdef Q_OS_WIN
   const char* pluginPath   = "../plugins/";
#else
   const char* pluginPath   = "plugins/";
#endif

#if defined(Q_OS_MAC) || defined(Q_OS_MACX) || defined(Q_OS_MAC64)
   struct stat stFileInfo;
   QString filePath = pluginPath;

   if (!stat((filePath = QString(pluginPath)).toLatin1().constData(), &stFileInfo))
   {
      // the script is in a plugin folder in the same folder as a distributed application
   }
   else if (!stat((filePath = QString("nesicide.app/Contents/Resources/")+pluginPath).toLatin1().constData(), &stFileInfo))
   {
      // normal path to scripts when dubbel clicking on an application
   }
   else if (!stat((filePath = QString("../../../")+pluginPath).toLatin1().constData(), &stFileInfo))
   {
      // the script is in a plugin folder in the same folder as an application that's debugged with Qt
   }
   else if (!stat((filePath = QString("../Resources/")+pluginPath).toLatin1().constData(), &stFileInfo))
   {
      // path to find scripts when using the Qt debuger
   }
   else
   {
      // no more place to look for the script, bailing out... lua will do the error handling though
   }

   QDir pluginDir(filePath.toLatin1().constData());
#else
   QDir pluginDir(pluginPath);
#endif
   QStringList pluginFiles;
   QDomElement pluginDocElement;
   int i;

   generalTextLogger->write ( "<strong>Loading plugins...</strong>" );

   pluginFiles = pluginDir.entryList(QStringList("*.plugin"),QDir::Files);

   for ( i = 0; i < pluginFiles.size(); i++ )
   {
      generalTextLogger->write ( pluginFiles[i] + ": " );

      QFile pluginFile ( pluginDir.absoluteFilePath(pluginFiles[i]) );
      pluginFile.open(QIODevice::ReadOnly|QIODevice::Text);

      if ( pluginFile.isOpen() )
      {
         QDomDocument* plugin = new QDomDocument();
         //QDomNode    logicNode;
         //QDomElement logicElement;

         plugin->setContent(&pluginFile);

         plugins.insert(pluginFiles[i],plugin);

         pluginDocElement = plugin->documentElement();
         generalTextLogger->write ( "&nbsp;&nbsp;&nbsp;caption: " + pluginDocElement.attribute("caption") );
         generalTextLogger->write ( "&nbsp;&nbsp;&nbsp;author: " + pluginDocElement.attribute("author") );
         generalTextLogger->write ( "&nbsp;&nbsp;&nbsp;version: " + pluginDocElement.attribute("version") );

         QDomNodeList logicNodeList = pluginDocElement.elementsByTagName("logic");

         if (!logicNodeList.isEmpty())
         {
            // only read the first logic node
            QDomNode logicNode = logicNodeList.item(0);
            QDomNode logicScript = logicNode.firstChild();

            if (logicScript.nodeType() == QDomNode::CDATASectionNode)
            {
               QString luascript = logicScript.toText().data();

               if (luascript.length() != 0)
               {
                  generalTextLogger->write ( "&nbsp;&nbsp;&nbsp;loading logic" );

                  if (report(globalLuaInstance, luaL_dostring(globalLuaInstance, luascript.toLatin1())))
                  {
                     continue; // ignore rest of the plugin and continue with next one if we couldn't load the script
                  }
               }
            }

            QString onLoad = logicNode.toElement().attribute("onLoad");

            if (!onLoad.isEmpty())
            {
               //onLoad
               generalTextLogger->write ( "&nbsp;&nbsp;&nbsp;call onLoadFunc \"" + onLoad + "\"");
               lua_getglobal(globalLuaInstance, onLoad.toLatin1());

               if (report(globalLuaInstance, lua_pcall(globalLuaInstance, 0, LUA_MULTRET, 0)))
               {
                  continue;
               }
            }
         }

         pluginFile.close();
      }

      generalTextLogger->write ( "done." );
   }

   generalTextLogger->write ( "<strong>Done loading plugins.</strong>" );
}

CPluginManager::~CPluginManager()
{
   lua_close(globalLuaInstance);
}

void CPluginManager::defineInterfaces(lua_State* lua)
{
   lua_pushcclosure (lua, luabind_compiler_logger_print, 0);
   lua_setglobal (lua, "compiler_logger_print");
}

void CPluginManager::lua_compiler_logger_print(QString text)
{
   generalTextLogger->write(text);
}

QString CPluginManager::getVersionInfo()
{
   return QString(LUA_VERSION " " LUA_COPYRIGHT);
}

// ========================================================================
// Only place functions bound directly to lua below!
// ========================================================================

int CPluginManager::report(lua_State* L, int status)
{
   if (status)
   {
      QString err = QString("");

      switch (status)
      {
         case LUA_YIELD:
            err += "Yield";
            break;
         case LUA_ERRRUN:
            err += "Runtime error";
            break;
         case LUA_ERRSYNTAX:
            err += "Syntax error";
            break;
         case LUA_ERRMEM:
            err += "Memory allocation error";
            break;
         case LUA_ERRERR:
            err += "Error when running the error handler";
            break;
         default:
            err += "Unknown error";
            break;
      }

      err += ", ";

      if (!lua_isnil(L, -1))
      {
         const char* msg = lua_tostring(L, -1);

         if (msg == NULL)
         {
            msg = "(error object is not a string)";
         }

         lua_pop(L, 1);
         err += msg;
         generalTextLogger->write("<font color='red'><strong> Lua Script Error:</strong> " + err + "</font>");
      }
   }

   return status;
}

static int luabind_compiler_logger_print(lua_State* lua)
{
   if (!lua_isstring(lua, 1))
   {
      return 0;
   }

   CPluginManager::instance()->lua_compiler_logger_print(QString(lua_tostring (lua, 1)));
   return 0;
}
