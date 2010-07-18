#ifndef CCONFIGURATOR_H
#define CCONFIGURATOR_H

#include <QSettings>

class CConfigurator
{
public:
   CConfigurator();
protected:
   QSettings* m_config;
};

#endif // CCONFIGURATOR_H

#if 0
// CPTODO: pull old config crap from old tool into new...
   BOOL Initialize ( void );

   inline UINT* GetControllerConfig ( UINT controller ) { return m_controllerConfig[controller]; }
   void SetControllerConfig ( UINT controller, UINT* config );

   inline CString GetProjectPath ( void ) { return m_szProjectPath; }
   void SetProjectPath ( CString path );

   inline BOOL IsTracerEnabled ( void ) { return m_bTracerEnabled; }
   void SetTracerEnabled ( BOOL tracerEnabled );

   inline BOOL IsLoggerEnabled ( void ) { return m_bLoggerEnabled; }
   void SetLoggerEnabled ( BOOL loggerEnabled );

protected:
   UINT    m_controllerConfig [ NUM_JOY ] [ 9 ];
   CString m_szProjectPath;
   BOOL    m_bTracerEnabled;
   BOOL    m_bLoggerEnabled;
#endif
