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
   virtual ~NESEmulatorThread () {}

   void setDialog(QDialog* dialog);

   void setRunning ( bool run ) { m_isRunning = run; }
   bool isRunning ( void ) { return m_isRunning; }
   void setFrequency ( float iFreq );
   float getFrequency ( void ) { return m_fFreq; }
   void setFactorIndex ( int factorIdx ) { m_factorIdx = factorIdx; setFrequency(m_fFreq); }
   int getFactorIndex ( void ) { return m_factorIdx; }
   bool isAtBreakpoint ( void ) { return m_isAtBreakpoint; }

   void setCartridge ( CCartridge* cartridge );

public slots:
   void startEmulation ();
   void pauseEmulation ();
   void stopEmulation ();
   void resetEmulator ();
   void controllerInput ( unsigned char* joy ) { m_joy[JOY1] = joy[JOY1]; m_joy[JOY2] = joy[JOY2]; }
   void primeEmulator ();
   void killEmulator ();

signals:
   void emulatedFrame ();
   void cartridgeLoaded ();

protected:
   virtual void run ();

   CCartridge*   m_pCartridge;
   bool          m_isRunning;
   qint32         m_lastVblankTime;
   qint32         m_currVblankTime;
   float         m_periodVblank;
   float         m_fFreq;
   float         m_fFreqReal;
   int m_factorIdx;
   unsigned char m_joy [ NUM_JOY ];
   bool          m_isAtBreakpoint;
};

#endif // NESEMULATORTHREAD_H
