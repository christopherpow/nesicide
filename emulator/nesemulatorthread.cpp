#include "nesemulatorthread.h"
#include "cnes.h"
#include "cnesrom.h"
#include "cnesppu.h"
#include "cnesapu.h"
#include "cnes6502.h"
#include "cnesicideproject.h"

QSemaphore emulatorSemaphore;
extern QSemaphore breakpointSemaphore;

NESEmulatorThread::NESEmulatorThread(QObject *)
{
   m_joy [ JOY1 ] = 0x00;
   m_joy [ JOY2 ] = 0x00;
   m_isRunning = false;
   m_isPaused = false;
   m_showOnPause = false;
   m_isStarting = false;
   m_isTerminating = false;
   m_isResetting = true;
   m_pCartridge = NULL;
}

NESEmulatorThread::~NESEmulatorThread()
{

}

void NESEmulatorThread::kill()
{
   // Force hard-reset of the machine...
   CNES::BREAKPOINTS(false);

   breakpointSemaphore.release();
   m_isRunning = true;
   m_isPaused = false;
   m_showOnPause = false;
   m_isTerminating = true;
   emulatorSemaphore.release();
}

void NESEmulatorThread::setDialog(QDialog* dialog)
{
}

void NESEmulatorThread::primeEmulator()
{
   if ( (nesicideProject) &&
        (nesicideProject->get_pointerToCartridge()) )
   {
      m_pCartridge = nesicideProject->get_pointerToCartridge();

      // Force hard-reset of the machine...
      CNES::BREAKPOINTS(false);
      CNES::CLEAROPCODEMASKS();

      // If we were stopped at a breakpoint, release...
      // Breakpoints have been deleted.
      if ( !(breakpointSemaphore.available()) )
      {
         breakpointSemaphore.release();
      }
   }
}

void NESEmulatorThread::loadCartridge()
{
   int b;

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

   // Initialize NES...
   CNES::RESET ( m_pCartridge->getMapperNumber() );

   emit cartridgeLoaded();
}

void NESEmulatorThread::resetEmulator()
{
   // Force hard-reset of the machine...
   CNES::BREAKPOINTS(false);

   // If during the last run we were stopped at a breakpoint, clear it...
   if ( !(breakpointSemaphore.available()) )
   {
      breakpointSemaphore.release();
   }

   m_isResetting = true;
}

void NESEmulatorThread::startEmulation ()
{
   // If during the last run we were stopped at a breakpoint, clear it...
   if ( !(breakpointSemaphore.available()) )
   {
      breakpointSemaphore.release();
   }

   m_isStarting = true;
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

   m_isStarting = true;
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

   m_isStarting = true;
}

void NESEmulatorThread::pauseEmulation (bool show)
{
   m_isStarting = false;
   m_isRunning = false;
   m_isPaused = true;
   m_showOnPause = show;
}

void NESEmulatorThread::run ()
{
   for ( ; ; )
   {
      // Allow thread exit...
      if ( m_isTerminating )
      {
         break;
      }

      // Allow thread to keep going...
      if ( m_isStarting )
      {
         m_isStarting = false;
         m_isRunning = true;
         m_isPaused = false;

         // Trigger Breakpoint dialog redraw...
         emit breakpointClear();

         // Trigger emulator UI button update...
         emit emulatorStarted();
      }

      // Properly coordinate NES reset with emulator...
      if ( m_isResetting )
      {
         // Reset emulated I/O devices...
         m_joy [ JOY1 ] = 0x00;
         m_joy [ JOY2 ] = 0x00;

         // Re-enable breakpoints that were previously enabled...
         CNES::BREAKPOINTS(true);

         // Reset NES...
         CNES::RESET ( CROM::MAPPER() );

         // Allow cartridge to be loaded...
         if ( m_pCartridge )
         {
            // Reload the cartridge image...
            // This internally causes a NES reset.
            loadCartridge();
            m_pCartridge = NULL;
         }

         emit emulatorReset();

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

      // Pause?
      if ( m_isPaused )
      {
         // Trigger inspectors to update on a pause also...
         emit emulatorPaused(m_showOnPause);
         m_isPaused = false;
      }

      // Acquire the semaphore...
      emulatorSemaphore.acquire();
   }

   return;
}
