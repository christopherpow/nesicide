#include "cconfigurator.h"
#include "cnesicidecommon.h"

#include <QCoreApplication>

static unsigned int m_DEFAULTcontrollerConfig [ NUM_JOY ][ 9 ] =
{
   { 1, JOY1_LEFT, JOY1_RIGHT, JOY1_UP, JOY1_DOWN, JOY1_SELECT, JOY1_START, JOY1_B, JOY1_A },
   { 1, JOY2_LEFT, JOY2_RIGHT, JOY2_UP, JOY2_DOWN, JOY2_SELECT, JOY2_START, JOY2_B, JOY2_A }
};

CConfigurator::CConfigurator()
{
   QByteArray ba;

   QCoreApplication::setOrganizationName("CSPSoftware");
   QCoreApplication::setOrganizationDomain("nesicide.com");
   QCoreApplication::setApplicationName("NESICIDE");
   m_config = new QSettings();

   // Get Controller configurations from registry...
#if 0
   ba = m_config->value("Controller1/Configuration");
   if ( !ba.isEmpty() )
   {
      // CPTODO: copy out controller config...
   }
   else
   {
      memcpy ( m_controllerConfig[JOY1], m_DEFAULTcontrollerConfig[JOY1], sizeof(m_controllerConfig[JOY1]) );
      // CPTODO: store controller config in persistent...
   }
   ba = m_config->value("Controller2/Configuration");
   if ( !ba.isEmpty() )
   {
      // CPTODO: copy out controller config...
   }
   else
   {
      memcpy ( m_controllerConfig[JOY2], m_DEFAULTcontrollerConfig[JOY2], sizeof(m_controllerConfig[JOY2]) );
      // CPTODO: store controller config in persistent...
   }
#endif
#if 0
   // Get default project path from registry...
   m_szProjectPath = pApp->GetProfileString ( "File New", "Project Path", "" );

   // Get Execution Tracer enabled flag...
   temp = pApp->GetProfileInt ( "Emulator", "Tracer", -1 );
   if ( temp == -1 )
   {
      m_bTracerEnabled = TRUE;
      pApp->WriteProfileInt ( "Emulator", "Tracer", (int)m_bTracerEnabled );
   }
   else
   {
      m_bTracerEnabled = (BOOL) temp;
   }

   // Get Code/Data Logger enabled flag...
   temp = pApp->GetProfileInt ( "Emulator", "Logger", -1 );
   if ( temp == -1 )
   {
      m_bLoggerEnabled = TRUE;
      pApp->WriteProfileInt ( "Emulator", "Logger", (int)m_bLoggerEnabled );
   }
   else
   {
      m_bLoggerEnabled = (BOOL) temp;
   }
#endif
}

