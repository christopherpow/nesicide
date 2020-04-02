#include "appsettings.h"

AppSettings *AppSettings::_instance = NULL;

void AppSettings::setAppMode(AppMode mode)
{
   m_appMode = mode;
   
   emit appSettingsChanged();
}
