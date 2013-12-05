#include "appsettings.h"

void AppSettings::setAppMode(AppMode mode)
{
   m_appMode = mode;
   
   emit appSettingsChanged();
}
