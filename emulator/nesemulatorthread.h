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

   void primeEmulator ();
   void resetEmulator ();
   void startEmulation ();
   void pauseEmulation (bool show);
   void stepCPUEmulation ();
   void stepPPUEmulation ();
   void controllerInput ( unsigned char* joy ) { m_joy[JOY1] = joy[JOY1]; m_joy[JOY2] = joy[JOY2]; }

signals:
   void emulatedFrame ();
   void cartridgeLoaded ();
   void breakpointClear ();
   void emulatorPaused (bool show);
   void emulatorReset();
   void emulatorStarted();

protected:
   virtual void run ();
   void setCartridge ( CCartridge* cartridge );

   CCartridge*   m_pCartridge;
   bool          m_isRunning;
   bool          m_isPaused;
   bool          m_showOnPause;
   bool          m_isTerminating;
   bool          m_isResetting;
   bool          m_isStarting;
   unsigned char m_joy [ NUM_JOY ];
};

#endif // NESEMULATORTHREAD_H
