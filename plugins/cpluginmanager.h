#ifndef CPLUGINMANAGER_H
#define CPLUGINMANAGER_H

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include <QString>

class CPluginManager
{
public:
    CPluginManager();
    ~CPluginManager();
    QString getVersionInfo();
};

extern CPluginManager *pluginManager;

#endif // CPLUGINMANAGER_H
