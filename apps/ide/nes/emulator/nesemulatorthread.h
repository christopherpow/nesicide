#ifndef NESEMULATORTHREAD_H
#define NESEMULATORTHREAD_H

#include <QThread>
#include <QSemaphore>

#include "ixmlserializable.h"

#include "nes_emulator_core.h"

#include "ccartridge.h"

class NESEmulatorWorker : public QObject, public IXMLSerializable
{
   Q_OBJECT
public:
   NESEmulatorWorker ( QObject* parent = 0 );
   virtual ~NESEmulatorWorker ();

   // IXMLSerializable Interface Implementation
   virtual bool serialize(QDomDocument& doc, QDomNode& node);
   virtual bool deserialize(QDomDocument& doc, QDomNode& node, QString& errors);

   virtual bool serializeContent(QFile& fileOut);
   virtual bool deserializeContent(QFile& fileIn);

   void _breakpointHook();

   QSemaphore* nesBreakpointSemaphore;
   QSemaphore* nesAudioSemaphore;

   void breakpointsChanged ();
   void primeEmulator ();
   void resetEmulator ();
   void softResetEmulator ();
   void startEmulation ();
   void pauseEmulation (bool show);
   void pauseEmulationAfter (int32_t frames) { m_pauseAfterFrames = frames; }
   void stepCPUEmulation ();
   void stepOverCPUEmulation ();
   void stepOutCPUEmulation ();
   void stepPPUEmulation ();
   void advanceFrame ();
   void adjustAudio ( int32_t bufferDepth );
   void controllerInput ( uint32_t* joy )
   {
      m_joy[CONTROLLER1] = joy[CONTROLLER1];
      m_joy[CONTROLLER2] = joy[CONTROLLER2];
   }
   void loadCartridge ();

signals:
   void breakpoint ();
   void emulatedFrame ();
   void updateDebuggers ();
   void machineReady ();
   void emulatorPaused(bool show);
   void emulatorPausedAfter();
   void emulatorReset();
   void emulatorStarted();
   void debugMessage(char* message);

public slots:
   void process();

protected:
   QTimer* pTimer;

   CCartridge*   m_pCartridge;

   bool          m_isRunning;
   bool          m_isPaused;
   bool          m_showOnPause;
   int           m_pauseAfterFrames;
   bool          m_isTerminating;
   bool          m_isResetting;
   bool          m_isSoftReset;
   bool          m_isStarting;
   int           m_debugFrame;
   uint32_t      m_joy [ NUM_CONTROLLERS ];
};

class NESEmulatorThread : public QObject, public IXMLSerializable
{
   Q_OBJECT
public:
   NESEmulatorThread ( QObject* parent = 0 );
   virtual ~NESEmulatorThread ();

   // IXMLSerializable Interface Implementation
   virtual bool serialize(QDomDocument& doc, QDomNode& node);
   virtual bool deserialize(QDomDocument& doc, QDomNode& node, QString& errors);

   virtual bool serializeContent(QFile& fileOut);
   virtual bool deserializeContent(QFile& fileIn);

   NESEmulatorWorker* worker() { return pWorker; }
   
public slots:
   void breakpointsChanged ();
   void primeEmulator ();
   void resetEmulator ();
   void softResetEmulator ();
   void startEmulation ();
   void pauseEmulation (bool show);
   void pauseEmulationAfter (int32_t frames) { pWorker->pauseEmulationAfter(frames); }
   void stepCPUEmulation ();
   void stepOverCPUEmulation ();
   void stepOutCPUEmulation ();
   void stepPPUEmulation ();
   void advanceFrame ();
   void adjustAudio ( int32_t bufferDepth );
   void controllerInput ( uint32_t* joy ) { pWorker->controllerInput(joy); }
signals:
   void breakpoint ();
   void emulatedFrame ();
   void updateDebuggers ();
   void machineReady ();
   void emulatorPaused(bool show);
   void emulatorPausedAfter();
   void emulatorReset();
   void emulatorStarted();
   void debugMessage(char* message);

protected:
   QThread* pThread;
   NESEmulatorWorker* pWorker;
};

#endif // NESEMULATORTHREAD_H
