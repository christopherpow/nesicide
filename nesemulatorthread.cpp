#include "nesemulatorthread.h"
#include "NES.h"
#include "ROM.h"
#include "PPU.h"
#include "6502.h"

static float m_factor [ 6 ] = { 0.25, 0.5, 1.0, 2.0, 4.0, 100.0 };

NESEmulatorThread::NESEmulatorThread (QObject *parent)
{
    m_factorIdx = 2;

    m_joy [ JOY1 ] = 0x00;
    m_joy [ JOY2 ] = 0x00;

    setFrequency ( FREQ_60HZ );

    m_lastVblankTime = 0;
    m_currVblankTime = 0;
    m_joy [ JOY1 ] = 0x00;
    m_joy [ JOY2 ] = 0x00;
    m_bBreakpoint = false;
    m_bRunning = false;
}

void NESEmulatorThread::setCartridge(CCartridge *cartridge)
{
    int b;

    // Clear emulator's cartridge ROMs...
    CROM::Clear16KBanks ();
    CROM::Clear8KBanks ();

    // Load cartridge PRG-ROM banks into emulator...
    for ( b = 0; b < cartridge->prgromBanks->banks.count(); b++ )
    {
        CROM::Set16KBank ( b, (unsigned char*)cartridge->prgromBanks->banks.at(b)->data, NULL );
    }

    // Load cartridge CHR-ROM banks into emulator...
    for ( b = 0; b < cartridge->chrromBanks->banks.count(); b++ )
    {
        CROM::Set8KBank ( b, (unsigned char*)cartridge->chrromBanks->banks.at(b)->data );
    }

    // Perform any necessary fixup on from the ROM loader...
    CROM::DoneLoadingBanks ();

    // Set up PPU with iNES header information...
    if ( (cartridge->mirrorMode == CCartridge::NoMirroring) ||
         (cartridge->mirrorMode == CCartridge::HorizontalMirroring) )
    {
        CPPU::MIRRORHORIZ ();
    }
    else if ( cartridge->mirrorMode == CCartridge::VerticalMirroring )
    {
        CPPU::MIRRORVERT ();
    }

    // Force mapper to intialize...
    mapperfunc [ cartridge->mapperNumber ].reset ();

// CPTODO: implement mapper reloading...project reload should load ROM in saved state.
#if 0
    MapperState* pMapperState = m_pRIID->GetMapperState ();

    if ( pMapperState->valid )
    {
       mapperfunc [ m_pRIID->GetMapperID() ].load ( pMapperState );
    }
#endif

    reset ();
}

void NESEmulatorThread::reset()
{
    // Reset emulated PPU...
    CPPU::RESET ();

// CPTODO: don't think i need this...
//    PullROMInfo ();

// CPTODO: removed logger stuff for now...
#if 0
    for ( idx = 0; idx < m_pRIID->GetNumBanksInRom(); idx++ )
    {
       CItemInfo* pIIRomBank = m_pRIID->GetRomBankPtr ( idx );
       if ( pIIRomBank != NULL )
       {
          CROMBankItemData* pRBID = (CROMBankItemData*)pIIRomBank->GetItemData();
          pRBID->GetLogger()->ClearData();
       }
    }
    pDoc->GetCodeDataLoggerDlg()->Reset();
    pDoc->GetTracerDlg()->Reset();
#endif

    // Reset emulated 6502 and APU [APU reset internal to 6502]...
    C6502::RESET ();

    // Clear emulated machine memory and registers...
    C6502::MEMCLR ();
    CPPU::MEMCLR ();
    CPPU::OAMCLR ();
    CROM::CHRRAMCLR ();

    // Clear the rest...
    CNES::RESET ();

// CPTODO: removed joypad logger for now...
#if 0
    pDoc->GetJoypadLoggerDlg()->Reset ();
#endif

    // Reset mapper...
    mapperfunc [ CROM::MAPPER() ].reset ();

    // Reset emulated I/O devices...
    m_joy [ JOY1 ] = 0x00;
    m_joy [ JOY2 ] = 0x00;
}

void NESEmulatorThread::setFrequency ( float fFreq )
{
   m_fFreq = fFreq;
   m_fFreqReal = fFreq*m_factor[m_factorIdx];
   m_periodVblank = (1000.0/m_fFreqReal);
}

void NESEmulatorThread::run ()
{
    static float adjust = 0.0;
    static float elapsed = 0.0;

// CPTODO: mmsys
//    timeBeginPeriod ( 1 );

    while ( 1 )
    {
       while ( m_bRunning )
       {
// CPTODO: mmsys -- figure out how to do timing in Qt
//          m_currVblankTime = timeGetTime ();
#if 0
          elapsed += (float)(m_currVblankTime-m_lastVblankTime);
          if ( elapsed < ((float)m_periodVblank-1-(float)adjust) )
          {
             msleep ( 1000 );
          }
          else
          {
             adjust = (elapsed-(float)m_periodVblank);
             elapsed = 0.0;
#endif
             // Run emulator for one frame...
             // CPTODO: this needs to be re-factored into a RUN-by-PPU-clock-tick method.
             //         internally it does everything by PPU ticks...but in order to support
             //         breakpoints effectively it needs to be wound up to this level.
             m_bBreakpoint = CNES::RUN ( m_joy );
             if ( m_bBreakpoint )
             {
                m_bRunning = false;
             }
#if 0
          }
          m_lastVblankTime = m_currVblankTime;
#endif
       }
    }

// CPTODO: mmsys
//    timeEndPeriod ();

    return;
}
