#include "cpluginmanager.h"

CPluginManager *pluginManager = (CPluginManager *)NULL;

CPluginManager::CPluginManager()
{
}

CPluginManager::~CPluginManager()
{

}

QString CPluginManager::getVersionInfo()
{
    return QString(LUA_VERSION) + " " + QString(LUA_COPYRIGHT);
}
