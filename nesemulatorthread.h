#ifndef NESEMULATORTHREAD_H
#define NESEMULATORTHREAD_H

#include <QThread>
#include "NESICIDECommon.h"
#include "ccartridge.h"

class NESEmulatorThread : public QThread
{
public:
    NESEmulatorThread ( QObject* parent = 0);
    virtual ~NESEmulatorThread () {}

    void setRunning ( bool bRun ) { m_bRunning = bRun; }
    void setFrequency ( float iFreq );
    float getFrequency ( void ) { return m_fFreq; }
    void setFactorIndex ( int factorIdx ) { m_factorIdx = factorIdx; setFrequency(m_fFreq); }
    int getFactorIndex ( void ) { return m_factorIdx; }
    void setJoy ( unsigned char* joy ) { m_joy[JOY1] = joy[JOY1]; m_joy[JOY2] = joy[JOY2]; }
    bool isBreakpoint ( void ) { return m_bBreakpoint; }

    void setCartridge ( CCartridge* cartridge );

    // emulation events [signals?]
    void reset ( void );

protected:
    virtual void run ();

    bool m_bRunning;
    qint32         m_lastVblankTime;
    qint32         m_currVblankTime;
    float         m_periodVblank;
    float         m_fFreq;
    float         m_fFreqReal;
    int m_factorIdx;
    unsigned char m_joy [ NUM_JOY ];
    bool          m_bBreakpoint;
};

#endif // NESEMULATORTHREAD_H
