#ifndef NESEMULATORTHREAD_H
#define NESEMULATORTHREAD_H

#include <QThread>

#include "cnesicidecommon.h"
#include "ccartridge.h"

extern QSemaphore        emulatorSemaphore;

class NESEmulatorThread : public QThread
{
   Q_OBJECT
public:
   NESEmulatorThread ( QObject* parent = 0 );
   virtual ~NESEmulatorThread ();
   void kill();

   void setDialog(QDialog* dialog);

   void setRunning ( bool run ) { m_isRunning = run; }
   bool isRunning ( void ) { return m_isRunning; }
   void setCartridge ( CCartridge* cartridge );

public slots:
   void startEmulation ();
   void pauseEmulation ();
   void stepCPUEmulation ();
   void stepPPUEmulation ();
   void resetEmulator ();
   void controllerInput ( unsigned char* joy ) { m_joy[JOY1] = joy[JOY1]; m_joy[JOY2] = joy[JOY2]; }
   void primeEmulator ();

signals:
   void emulatedFrame ();
   void cartridgeLoaded ();
   void breakpointClear ();
   void emulatorPaused ();

protected:
   virtual void run ();

   CCartridge*   m_pCartridge;
   bool          m_isRunning;
   bool          m_isPaused;
   bool          m_isTerminating;
   bool          m_isResetting;
   unsigned char m_joy [ NUM_JOY ];
};

#endif // NESEMULATORTHREAD_H
