#ifndef CPLUGINMANAGER_H
#define CPLUGINMANAGER_H

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include <QString>

#include "cbuildertextlogger.h"

class CPluginManager
{
public:
    CPluginManager();
    ~CPluginManager();
    QString getVersionInfo();
    void doInitScript();
    void defineInterfaces(lua_State *lua);

    // Functions called by lua
    void lua_compiler_logger_print(QString text);

protected:
    lua_State *globalLuaInstance;
    QString report(lua_State *L, int status);
};

extern CPluginManager *pluginManager;



#endif // CPLUGINMANAGER_H
