#ifndef NESEMULATORTHREAD_H
#define NESEMULATORTHREAD_H

#include <QThread>
#include <QSemaphore>

#include "ixmlserializable.h"

#include "nes_emulator_core.h"

#include "ccartridge.h"
// EMU
class NESEmulatorThread : public QThread, public IXMLSerializable
{
   Q_OBJECT
public:
   NESEmulatorThread ( QObject* parent = 0 );
   virtual ~NESEmulatorThread ();
   void kill();

   // IXMLSerializable Interface Implementation
   virtual bool serialize(QDomDocument& doc, QDomNode& node);
   virtual bool deserialize(QDomDocument& doc, QDomNode& node, QString& errors);

   virtual bool serializeContent(QFile& fileOut);
   virtual bool deserializeContent(QFile& fileIn);

public slots:
   void resetEmulator ();
   void softResetEmulator ();
   void startEmulation ();
   void pauseEmulation (bool show);
   void controllerInput ( uint8_t* joy )
   {
      m_joy[CONTROLLER1] = joy[CONTROLLER1];
      m_joy[CONTROLLER2] = joy[CONTROLLER2];
   }
   void primeEmulator ( CCartridge* pCartridge );

signals:
   void emulatedFrame ();
   void cartridgeLoaded ();
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
   bool          m_isSoftReset;
   bool          m_isStarting;
   uint8_t m_joy [ NUM_CONTROLLERS ];
};

#endif // NESEMULATORTHREAD_H
