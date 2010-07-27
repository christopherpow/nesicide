#include "cconfigurator.h"
#include "cnesicidecommon.h"

#include <QCoreApplication>
/*
static unsigned int m_DEFAULTcontrollerConfig [ NUM_JOY ][ 9 ] =
{
    { 1, JOY1_LEFT, JOY1_RIGHT, JOY1_UP, JOY1_DOWN, JOY1_SELECT, JOY1_START, JOY1_B, JOY1_A },
    { 1, JOY2_LEFT, JOY2_RIGHT, JOY2_UP, JOY2_DOWN, JOY2_SELECT, JOY2_START, JOY2_B, JOY2_A }
};
*/

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

