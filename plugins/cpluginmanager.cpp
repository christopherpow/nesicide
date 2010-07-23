#include "cpluginmanager.h"

static int luabind_compiler_logger_print(lua_State *lua);


CPluginManager *pluginManager = (CPluginManager *)NULL;

CPluginManager::CPluginManager()
{
    globalLuaInstance = lua_open();
    defineInterfaces(globalLuaInstance);
}

void CPluginManager::doInitScript()
{
    int status = luaL_dofile(globalLuaInstance, "plugins/init.lua");
    QString result = report(globalLuaInstance, status);
    if (!result.isEmpty())
    {
        builderTextLogger.write("<font color='red'><strong>Script Error:</strong> " + result + "</font>");
    }
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
    builderTextLogger.write(text);
}

QString CPluginManager::getVersionInfo()
{
    return QString(LUA_VERSION) + " " + QString(LUA_COPYRIGHT);
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
