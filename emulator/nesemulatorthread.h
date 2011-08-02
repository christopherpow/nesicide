#ifndef NESEMULATORTHREAD_H
#define NESEMULATORTHREAD_H

#include <QThread>
#include <QSemaphore>

#include "ixmlserializable.h"

#include "emulator_core.h"

#include "ccartridge.h"

#include <QMutex>

extern QMutex doFrameMutex;

// Hook function endpoints.
void coreMutexLock ( void );
void coreMutexUnlock ( void );

class NESEmulatorThread : public QThread, public IXMLSerializable
{
   Q_OBJECT
public:
   NESEmulatorThread ( QObject* parent = 0 );
   virtual ~NESEmulatorThread ();
   void kill();

   void primeEmulator ();
   void resetEmulator ();
   void adjustAudio ( int32_t bufferDepth );
   void startEmulation ();
   void pauseEmulation (bool show);
   void pauseEmulationAfter (int frames) { m_pauseAfterFrames = frames; }
   void stepCPUEmulation ();
   void stepOverCPUEmulation ();
   void stepOutCPUEmulation ();
   void stepPPUEmulation ();
   void advanceFrame ();
   void controllerInput ( uint8_t* joy )
   {
      coreMutexLock();
      m_joy[CONTROLLER1] = joy[CONTROLLER1];
      m_joy[CONTROLLER2] = joy[CONTROLLER2];
      coreMutexUnlock();
   }
   bool isActive () { return (m_isStarting||m_isRunning); }

   // IXMLSerializable Interface Implementation
   virtual bool serialize(QDomDocument& doc, QDomNode& node);
   virtual bool deserialize(QDomDocument& doc, QDomNode& node, QString& errors);

signals:
   void emulatedFrame ();
   void updateDebuggers ();
   void cartridgeLoaded ();
   void emulatorPaused(bool show);
   void emulatorReset();
   void emulatorStarted();
   void debugMessage(char* message);

protected:
   virtual void run ();
   void loadCartridge ();

   CCartridge*   m_pCartridge;

   bool          m_isRunning;
   bool          m_isPaused;
   bool          m_showOnPause;
   int           m_pauseAfterFrames;
   bool          m_isTerminating;
   bool          m_isResetting;
   bool          m_isStarting;
   int           m_debugFrame;
   uint8_t m_joy [ NUM_CONTROLLERS ];
};

#endif // NESEMULATORTHREAD_H
