#ifndef CCONFIGURATOR_H
#define CCONFIGURATOR_H

#include <QSettings>


class CConfigurator
{
public:
   CConfigurator();
   ~CConfigurator();



protected:
   QSettings* m_config;
   void loadDefaultSettings();
};

#endif // CCONFIGURATOR_H

