#include "cconfigurator.h"

#include <QCoreApplication>

CConfigurator* appConfig;

CConfigurator::CConfigurator()
{
    QCoreApplication::setOrganizationName("CSPSoftware");
    QCoreApplication::setOrganizationDomain("nesicide.com");
    QCoreApplication::setApplicationName("NESICIDE");
    m_config = new QSettings();

    // Initialize the default settings
    loadDefaultSettings();

    // Load any existing configuration settings
    m_config->sync();



}


CConfigurator::~CConfigurator()
{
    if (m_config)
        delete m_config;
}

void CConfigurator::loadDefaultSettings()
{

}

