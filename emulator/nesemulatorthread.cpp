#include "nesemulatorthread.h"
#include "cnes.h"
#include "cnesrom.h"
#include "cnesppu.h"
#include "cnesapu.h"
#include "cnes6502.h"
#include "cnesicideproject.h"

QSemaphore emulatorSemaphore;
extern QSemaphore breakpointSemaphore;

NESEmulatorThread::NESEmulatorThread(QObject *parent)
{
   m_joy [ JOY1 ] = 0x00;
   m_joy [ JOY2 ] = 0x00;
   m_isRunning = false;
   m_isPaused = false;
   m_isTerminating = false;
   m_isResetting = false;
   m_pCartridge = NULL;
}

NESEmulatorThread::~NESEmulatorThread()
{

}

void NESEmulatorThread::kill()
{
   m_isRunning = false;
   m_isPaused = false;
   m_isTerminating = true;
   if (!breakpointSemaphore.available())
      breakpointSemaphore.release();
   if (!emulatorSemaphore.available())
      emulatorSemaphore.release();
}

void NESEmulatorThread::setDialog(QDialog* dialog)
{
   QObject::connect(dialog, SIGNAL(controllerInput(unsigned char*)), this, SLOT(controllerInput(unsigned char*)));
   QObject::connect(dialog, SIGNAL(primeEmulator()), this, SLOT(primeEmulator()));
}

void NESEmulatorThread::primeEmulator()
{
   if ( (nesicideProject) &&
        (nesicideProject->get_pointerToCartridge()) )
   {
      setCartridge ( nesicideProject->get_pointerToCartridge() );
   }
}

void NESEmulatorThread::setCartridge(CCartridge *cartridge)
{
   int b;

   // Store pointer to cartridge for later use [resets]...
   m_pCartridge = cartridge;

   // Clear emulator's cartridge ROMs...
   CROM::Clear16KBanks ();
   CROM::Clear8KBanks ();

   // Load cartridge PRG-ROM banks into emulator...
   for ( b = 0; b < m_pCartridge->getPointerToPrgRomBanks()->get_pointerToArrayOfBanks()->count(); b++ )
   {
      CROM::Set16KBank ( b, (unsigned char*)m_pCartridge->getPointerToPrgRomBanks()->get_pointerToArrayOfBanks()->at(b)->
                         get_pointerToBankData() );
   }

   // Load cartridge CHR-ROM banks into emulator...
   for ( b = 0; b < m_pCartridge->getPointerToChrRomBanks()->banks.count(); b++ )
   {
      CROM::Set8KBank ( b, (unsigned char*)m_pCartridge->getPointerToChrRomBanks()->banks.at(b)->data );
   }

   // Perform any necessary fixup on from the ROM loading...
   CROM::DoneLoadingBanks ();

   // Set up PPU with iNES header information...
   if ( (m_pCartridge->getMirrorMode() == GameMirrorMode::NoMirroring) ||
        (m_pCartridge->getMirrorMode() == GameMirrorMode::HorizontalMirroring) )
   {
     CPPU::MIRRORHORIZ ();
   }
   else if ( m_pCartridge->getMirrorMode() == GameMirrorMode::VerticalMirroring )
   {
     CPPU::MIRRORVERT ();
   }

   // CPTODO: implement mapper reloading...project reload should load ROM in saved state.
#if 0
   // Force mapper to intialize...
   mapperfunc [ m_pCartridge->getMapperNumber() ].reset ();

   MapperState* pMapperState = m_pRIID->GetMapperState ();

   if ( pMapperState->valid )
   {
      mapperfunc [ m_pRIID->GetMapperID() ].load ( pMapperState );
   }
#endif

   // Reset NES...
   CNES::RESET ( m_pCartridge->getMapperNumber() );

   emit cartridgeLoaded();
}

void NESEmulatorThread::resetEmulator()
{
   m_isResetting = true;
}

void NESEmulatorThread::startEmulation ()
{
   // If during the last run we were stopped at a breakpoint, clear it...
   if ( !(breakpointSemaphore.available()) )
   {
      breakpointSemaphore.release();
   }

   // Trigger Breakpoint dialog redraw...
   emit breakpointClear();

   m_isRunning = true;
   m_isPaused = false;
}

void NESEmulatorThread::stepCPUEmulation ()
{
   // If during the last run we were stopped at a breakpoint, clear it...
   // But ensure we come right back...
   CNES::STEPCPUBREAKPOINT();
   if ( !(breakpointSemaphore.available()) )
   {
      breakpointSemaphore.release();
   }

   // Trigger Breakpoint dialog redraw...
   emit breakpointClear();

   m_isRunning = true;
   m_isPaused = false;
}

void NESEmulatorThread::stepPPUEmulation ()
{
   // If during the last run we were stopped at a breakpoint, clear it...
   // But ensure we come right back...
   CNES::STEPPPUBREAKPOINT();
   if ( !(breakpointSemaphore.available()) )
   {
      breakpointSemaphore.release();
   }

   // Trigger Breakpoint dialog redraw...
   emit breakpointClear();

   m_isRunning = true;
   m_isPaused = false;
}

void NESEmulatorThread::pauseEmulation ()
{
   m_isRunning = false;
   m_isPaused = true;
}

void NESEmulatorThread::run ()
{
   // Seed mechanism for breaking...
   breakpointSemaphore.acquire ();

   for ( ; ; )
   {
      // Allow thread exit...
      if ( m_isTerminating )
      {
         break;
      }

      // Properly coordinate NES reset with emulator...
      if ( m_isResetting )
      {
         // Reset emulated I/O devices...
         m_joy [ JOY1 ] = 0x00;
         m_joy [ JOY2 ] = 0x00;

         // Reset NES...
         CNES::RESET ( CROM::MAPPER() );

         // Don't *keep* resetting...
         m_isResetting = false;
      }

      // Run the NES...
      if ( m_isRunning )
      {
         // Make sure breakpoint semaphore is on the precipice...
         breakpointSemaphore.tryAcquire();

         // Run emulator for one frame...
         CNES::RUN ( m_joy );

         emit emulatedFrame();
      }
      else
      {
         // Trigger inspectors to update on a pause also...
         if ( m_isPaused )
         {
            emit emulatorPaused();
            m_isPaused = false;
         }
      }

      // Acquire the semaphore...
      emulatorSemaphore.acquire();
   }

   return;
}
