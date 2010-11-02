#include "cpluginmanager.h"

#ifdef Q_WS_MAC
#include <sys/stat.h>
#endif

#include <QDir>

static int luabind_compiler_logger_print(lua_State *lua);

CPluginManager *pluginManager = (CPluginManager *)NULL;

QHash<QString,QDomDocument*> CPluginManager::plugins;

CPluginManager::CPluginManager()
{
    globalLuaInstance = lua_open();
    defineInterfaces(globalLuaInstance);
}

void CPluginManager::doInitScript()
{
#ifdef Q_WS_WIN
   const char * initScriptPath   = "../nesicide2-master/plugins/init.lua";
#else
   const char * initScriptPath   = "plugins/init.lua";
#endif
    int status;
    QString result;
#ifdef Q_WS_MAC
    struct stat stFileInfo;
    QString filePath = initScriptPath;
    if(!stat((filePath = QString(initScriptPath)).toAscii().constData(), &stFileInfo)) {
        // the script is in a plugin folder in the same folder as a distributed application
    } else if(!stat((filePath = QString("nesicide2.app/Contents/Resources/")+initScriptPath).toAscii().constData(), &stFileInfo)) {
        // normal path to scripts when dubbel clicking on an application
    } else if(!stat((filePath = QString("../../../")+initScriptPath).toAscii().constData(), &stFileInfo)) {
        // the script is in a plugin folder in the same folder as an application that's debugged with Qt
    } else if(!stat((filePath = QString("../Resources/")+initScriptPath).toAscii().constData(), &stFileInfo)) {
        // path to find scripts when using the Qt debuger
    } else {
        // no more place to look for the script, bailing out... lua will do the error handling though
    }
    status = luaL_dofile(globalLuaInstance, filePath.toAscii().constData());
#else
    status = luaL_dofile(globalLuaInstance, initScriptPath);
#endif
    result = report(globalLuaInstance, status);
    if (!result.isEmpty())
    {
        generalTextLogger.write("<font color='red'><strong>Script Error:</strong> " + result + "</font>");
    }
}

void CPluginManager::loadPlugins()
{
#ifdef Q_WS_WIN
   const char * pluginPath   = "../nesicide2-master/plugins/";
#else
   const char * pluginPath   = "plugins/";
#endif
   QDir pluginDir(pluginPath);
   QStringList pluginFiles;
   QDomElement pluginDocElement;
   QDomNode    pluginNode;
   QDomElement pluginElement;
   int i;

   generalTextLogger.write ( "<strong>Loading plugins...</strong>" );
   
   pluginFiles = pluginDir.entryList(QStringList("*.xml"),QDir::Files | QDir::NoSymLinks);

   for ( i = 0; i < pluginFiles.size(); i++ )
   {
      generalTextLogger.write ( pluginFiles[i] + ": " );
      
      QFile pluginFile ( pluginDir.absoluteFilePath(pluginFiles[i]) );
      pluginFile.open(QIODevice::ReadOnly);
      if ( pluginFile.isOpen() )
      {         
         QDomDocument* plugin = new QDomDocument();
         
         plugin->setContent(&pluginFile);
         
         plugins.insert(pluginFiles[i],plugin);
         
         pluginDocElement = plugin->documentElement();
         generalTextLogger.write ( "&nbsp;&nbsp;&nbsp;caption: " + pluginDocElement.attribute("caption") );
         generalTextLogger.write ( "&nbsp;&nbsp;&nbsp;author: " + pluginDocElement.attribute("author") );
         generalTextLogger.write ( "&nbsp;&nbsp;&nbsp;version: " + pluginDocElement.attribute("version") );
         
         pluginFile.close();
      }
      generalTextLogger.write ( "done." );
   }
   
   generalTextLogger.write ( "<strong>Done loading plugins.</strong>" );
}

CPluginManager::~CPluginManager()
{
    lua_close(globalLuaInstance);
}

void CPluginManager::defineInterfaces(lua_State *lua)
{
    lua_pushcclosure (lua, luabind_compiler_logger_print, 0);
    lua_setglobal (lua, "compiler_logger_print");
}

void CPluginManager::lua_compiler_logger_print(QString text)
{
    generalTextLogger.write(text);
}

QString CPluginManager::getVersionInfo()
{
    return QString(LUA_VERSION" "LUA_COPYRIGHT);
}

// ========================================================================
// Only place functions bound directly to lua below!
// ========================================================================

QString CPluginManager::report(lua_State *L, int status)
{
    if (status && !lua_isnil(L, -1)) {
        const char *msg = lua_tostring(L, -1);
        if (msg == NULL) msg = "(error object is not a string)";
        lua_pop(L, 1);
        return QString(msg);
    }

    return QString();
}

static int luabind_compiler_logger_print(lua_State *lua)
{
    if (!lua_isstring(lua, 1))
        return 0;
    pluginManager->lua_compiler_logger_print(QString(lua_tostring (lua, 1)));
    return 0;
}
