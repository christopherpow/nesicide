#ifndef NESEMULATORTHREAD_H
#define NESEMULATORTHREAD_H

#include <QThread>
#include <QSemaphore>
#include <QDialog>

#include "emulator_core.h"

#include "ccartridge.h"

#include <QMutex>

extern QMutex doFrameMutex;

// Hook function endpoints.
void coreMutexLock ( void );
void coreMutexUnlock ( void );

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
   void controllerInput ( uint8_t* joy ) {
      coreMutexLock();
      m_joy[JOY1] = joy[JOY1];
      m_joy[JOY2] = joy[JOY2];
      coreMutexUnlock();
   }

signals:
   void emulatedFrame ();
   void cartridgeLoaded ();
   void breakpointClear ();
   void emulatorPaused (bool show);
   void emulatorReset();
   void emulatorStarted();

protected:
   virtual void run ();
   void loadCartridge ();

   CCartridge*   m_pCartridge;

   bool          m_isRunning;
   bool          m_isPaused;
   bool          m_showOnPause;
   bool          m_isTerminating;
   bool          m_isResetting;
   bool          m_isStarting;
   uint8_t m_joy [ NUM_JOY ];
};

#endif // NESEMULATORTHREAD_H
