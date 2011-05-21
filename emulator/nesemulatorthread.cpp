//    NESICIDE - an IDE for the 8-bit NES.
//    Copyright (C) 2009  Christopher S. Pow

//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <QSettings>

#include "nesemulatorthread.h"

#include "dbg_cnes.h"
#include "dbg_cnesrom.h"

#include "ccc65interface.h"

#include "main.h"

#undef main
#include <SDL.h>

#include "cnesicideproject.h"

extern QSemaphore breakpointSemaphore;
extern QSemaphore breakpointWatcherSemaphore;

#include <QMutex>

SDL_AudioSpec sdlAudioSpec;
QMutex doFrameMutex;

// Hook function endpoints.
void coreMutexLock ( void )
{
   doFrameMutex.lock();
}

void coreMutexUnlock ( void )
{
   doFrameMutex.unlock();
}

void breakpointHook ( void )
{
   breakpointWatcherSemaphore.release();
   breakpointSemaphore.acquire();
}

extern "C" void SDL_GetMoreData(void* userdata, uint8_t* stream, int32_t len)
{
   int32_t samplesAvailable;

   coreMutexLock();
   samplesAvailable = nesGetAudioSamplesAvailable();
   coreMutexUnlock();
   if (samplesAvailable < 0)
   {
      return;
   }

   memcpy(stream,nesGetAudioSamples(),len);

   if ( emulator->isFinished() && (samplesAvailable < APU_BUFFER_PRERENDER_THRESHOLD) )
   {
      emulator->start();
   }
}

NESEmulatorThread::NESEmulatorThread(QObject*)
{
   SDL_AudioSpec obtained;

   m_joy [ CONTROLLER1 ] = 0x00;
   m_joy [ CONTROLLER2 ] = 0x00;
   m_isRunning = false;
   m_isPaused = false;
   m_showOnPause = false;
   m_pauseAfterFrames = -1;
   m_isStarting = false;
   m_isTerminating = false;
   m_isResetting = false;
   m_debugFrame = 0;
   m_pCartridge = NULL;

   nesSetCoreMutexLockHook(coreMutexLock);
   nesSetCoreMutexUnlockHook(coreMutexUnlock);

   // Enable debugging in the external emulator library.
   nesEnableDebug ();

   // Enable breakpoint callbacks from the external emulator library.
   nesSetBreakpointHook(breakpointHook);

   SDL_Init ( SDL_INIT_AUDIO );

   sdlAudioSpec.callback = SDL_GetMoreData;
   sdlAudioSpec.userdata = NULL;
   sdlAudioSpec.channels = 1;
   sdlAudioSpec.format = AUDIO_S16SYS;
   sdlAudioSpec.freq = SDL_SAMPLE_RATE;

   // Set up audio sample rate for video mode...
   sdlAudioSpec.samples = APU_SAMPLES;

   SDL_OpenAudio ( &sdlAudioSpec, &obtained );

   SDL_PauseAudio ( 0 );

   coreMutexLock();
   nesClearAudioSamplesAvailable();
   coreMutexUnlock();
}

NESEmulatorThread::~NESEmulatorThread()
{
   SDL_PauseAudio ( 1 );

   SDL_CloseAudio ();

   SDL_Quit();
}

void NESEmulatorThread::kill()
{
   // Force hard-reset of the machine...
   nesEnableBreakpoints(false);

   m_isRunning = true;
   m_isPaused = false;
   m_showOnPause = false;
   m_isTerminating = true;

   breakpointSemaphore.release();
}

void NESEmulatorThread::adjustAudio(int32_t bufferDepth)
{
   SDL_AudioSpec obtained;

   SDL_PauseAudio ( 1 );

   SDL_CloseAudio ();

   sdlAudioSpec.callback = SDL_GetMoreData;
   sdlAudioSpec.userdata = NULL;
   sdlAudioSpec.channels = 1;
   sdlAudioSpec.format = AUDIO_S16SYS;
   sdlAudioSpec.freq = SDL_SAMPLE_RATE;

   // Set up audio sample rate for video mode...
   sdlAudioSpec.samples = bufferDepth;

   SDL_OpenAudio ( &sdlAudioSpec, &obtained );

   SDL_PauseAudio ( 0 );

   coreMutexLock();
   nesClearAudioSamplesAvailable();
   coreMutexUnlock();
}

void NESEmulatorThread::loadCartridge()
{
   int32_t b;
   int32_t a;

   if ( m_pCartridge->getPrgRomBanks()->getPrgRomBanks().count() )
   {
      // Clear emulator's cartridge ROMs...
      nesUnloadROM();

      // Load cartridge PRG-ROM banks into emulator...
      for ( b = 0; b < m_pCartridge->getPrgRomBanks()->getPrgRomBanks().count(); b++ )
      {
         nesLoadPRGROMBank ( b, (uint8_t*)m_pCartridge->getPrgRomBanks()->getPrgRomBanks().at(b)->getBankData() );

         // Update opcode masks to show proper disassembly...
         for ( a = 0; a < MEM_8KB; a++ )
         {
            if ( CCC65Interface::isAbsoluteAddressAnOpcode((b*MEM_8KB)+a) )
            {
               nesSetOpcodeMask((b*MEM_8KB)+a,1);
            }
            else
            {
               nesSetOpcodeMask((b*MEM_8KB)+a,0);
            }
         }
      }

      // Load cartridge CHR-ROM banks into emulator...
      for ( b = 0; b < m_pCartridge->getChrRomBanks()->getChrRomBanks().count(); b++ )
      {
         nesLoadCHRROMBank ( b, (uint8_t*)m_pCartridge->getChrRomBanks()->getChrRomBanks().at(b)->getBankData() );
      }

      // Perform any necessary fixup from the ROM loading...
      nesLoadROM();

      // Set up PPU with iNES header information...
      if ( m_pCartridge->getMirrorMode() == HorizontalMirroring )
      {
         nesSetHorizontalMirroring();
      }
      else if ( m_pCartridge->getMirrorMode() == VerticalMirroring )
      {
         nesSetVerticalMirroring();
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
      nesResetInitial(m_pCartridge->getMapperNumber());

      emit cartridgeLoaded();
   }
}

void NESEmulatorThread::primeEmulator()
{
   if ( (nesicideProject) &&
        (nesicideProject->getCartridge()) )
   {
      m_pCartridge = nesicideProject->getCartridge();

      // Force hard-reset of the machine...
      nesEnableBreakpoints(false);
      nesClearOpcodeMasks();

      // If we were stopped at a breakpoint, release...
      // Breakpoints have been deleted.
      if ( !(breakpointSemaphore.available()) )
      {
         breakpointSemaphore.release();
      }
   }
}

void NESEmulatorThread::resetEmulator()
{
   // Force hard-reset of the machine...
   nesEnableBreakpoints(false);

   // If during the last run we were stopped at a breakpoint, clear it...
   if ( !(breakpointSemaphore.available()) )
   {
      breakpointSemaphore.release();
   }

   m_isResetting = true;
   m_isStarting = false;
   m_isRunning = false;
   m_isPaused = true;
   m_showOnPause = false;
   emulator->start();
}

void NESEmulatorThread::startEmulation ()
{
   // If during the last run we were stopped at a breakpoint, clear it...
   if ( !(breakpointSemaphore.available()) )
   {
      breakpointSemaphore.release();
   }

   m_isStarting = true;
   emulator->start();
}

void NESEmulatorThread::stepCPUEmulation ()
{
   // If during the last run we were stopped at a breakpoint, clear it...
   // But ensure we come right back...
   nesStepCpu();

   if ( !(breakpointSemaphore.available()) )
   {
      breakpointSemaphore.release();
   }

   m_isStarting = true;
   m_isPaused = false;
   emulator->start();
}

void NESEmulatorThread::stepPPUEmulation ()
{
   // If during the last run we were stopped at a breakpoint, clear it...
   // But ensure we come right back...
   nesStepPpu();

   if ( !(breakpointSemaphore.available()) )
   {
      breakpointSemaphore.release();
   }

   m_isStarting = true;
   m_isPaused = false;
   emulator->start();
}

void NESEmulatorThread::pauseEmulation (bool show)
{
   m_isStarting = false;
   m_isRunning = false;
   m_isPaused = true;
   m_showOnPause = show;
   emulator->start();
}

void NESEmulatorThread::run ()
{
   int32_t samplesAvailable;
   int32_t debuggerUpdateRate = EnvironmentSettingsDialog::debuggerUpdateRate();

   // Special case for 1Hz debugger update to match system mode.
   if ( debuggerUpdateRate == -1 )
   {
      if ( nesGetSystemMode() == MODE_NTSC )
      {
         debuggerUpdateRate = 60;
      }
      else
      {
         debuggerUpdateRate = 50;
      }
   }

   while ( m_isStarting || m_isRunning || m_isResetting || m_isPaused )
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
         m_joy [ CONTROLLER1 ] = 0x00;
         m_joy [ CONTROLLER2 ] = 0x00;

         // Reset NES...
         nesReset();

         // Re-enable breakpoints that were previously enabled...
         nesEnableBreakpoints(true);

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

      // Pause?
      if ( m_isPaused || (m_pauseAfterFrames == 0) )
      {
         // Trigger inspectors to update on a pause also...
         emit emulatorPaused(m_showOnPause);
         m_isPaused = false;
         m_isRunning = false;
         if ( m_pauseAfterFrames == 0 )
         {
            m_pauseAfterFrames = -1;
         }

         nesClearAudioSamplesAvailable();

         break;
      }

      // Run the NES...
      if ( m_isRunning )
      {
         coreMutexLock();
         samplesAvailable = nesGetAudioSamplesAvailable();
         coreMutexUnlock();

         if ( samplesAvailable >= APU_BUFFER_PRERENDER )
         {
            break;
         }

         // Make sure breakpoint semaphore is on the precipice...
         breakpointSemaphore.tryAcquire();

         // Run emulator for one frame...
         SDL_LockAudio();
         nesRun(m_joy);
         SDL_UnlockAudio();

         if ( m_pauseAfterFrames != -1 )
         {
            m_pauseAfterFrames--;
         }

         emit emulatedFrame();

         if ( m_debugFrame )
         {
            m_debugFrame--;
         }
         if ( (!m_debugFrame) && (debuggerUpdateRate) )
         {
            m_debugFrame = debuggerUpdateRate;
            emit updateDebuggers();
         }
      }
   }

   return;
}
