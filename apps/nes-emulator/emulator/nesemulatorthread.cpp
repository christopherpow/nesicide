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

#include "nesemulatorthread.h"

#include "nes_emulator_core.h"

#undef main
#include <SDL.h>

SDL_AudioSpec sdlAudioSpec;
QSemaphore nesAudioSemaphore(0);

// Hook function endpoints.
static void audioHook ( void )
{
   nesAudioSemaphore.acquire();
}

extern "C" void SDL_GetMoreData(void* userdata, uint8_t* stream, int32_t len)
{
#if 0
   uint64_t t;
   static uint64_t to;
   uint64_t f;
   QueryPerformanceFrequency(&f);
   QueryPerformanceCounter(&t);
   QString str;
   str.sprintf("Smp: %d, Freq: %d, Ctr: %d, Diff %d, Per %lf", len>>1, f.LowPart, t.LowPart,t.LowPart-to.LowPart,(float)(t.LowPart-to.LowPart)/(float)f.LowPart);
   to = t;
   qDebug(str.toLatin1().constData());
#endif
//   QDateTime dt = QDateTime::currentDateTime();
//   static int ctr = 0;
//   qDebug("%d %d:%03d",ctr++,dt.time().second(),dt.time().msec());
   if ( nesGetAudioSamplesAvailable() >= (len>>1) )
   {
      memcpy(stream,nesGetAudioSamples(len>>1),len);
   }
#if 0
   else
   {
//      qDebug("UNDERRUN");
   }
#endif
   nesAudioSemaphore.release();
}

NESEmulatorThread::NESEmulatorThread(QObject*)
   : pWorker(new NESEmulatorWorker()),
     pThread(NULL)
{
   // Enable callbacks from the external emulator library.
   nesSetAudioHook(audioHook);

   SDL_Init ( SDL_INIT_AUDIO );

   sdlAudioSpec.callback = SDL_GetMoreData;
   sdlAudioSpec.userdata = this;
   sdlAudioSpec.channels = 1;
   sdlAudioSpec.format = AUDIO_S16SYS;
   sdlAudioSpec.freq = SDL_SAMPLE_RATE;

   // Set up audio sample rate for video mode...
   sdlAudioSpec.samples = APU_SAMPLES;

   SDL_AudioSpec sdlAudioSpecOut;
   SDL_OpenAudio ( &sdlAudioSpec, &sdlAudioSpecOut );

   SDL_PauseAudio ( 0 );

   nesClearAudioSamplesAvailable();

   pThread = new QThread();
   pWorker->moveToThread(pThread);
   pThread->start();

   QObject::connect(pWorker,SIGNAL(emulatedFrame()),this,SIGNAL(emulatedFrame()));
   QObject::connect(pWorker,SIGNAL(cartridgeLoaded()),this,SIGNAL(cartridgeLoaded()));
   QObject::connect(pWorker,SIGNAL(emulatorPaused(bool)),this,SIGNAL(emulatorPaused(bool)));
   QObject::connect(pWorker,SIGNAL(emulatorReset()),this,SIGNAL(emulatorReset()));
   QObject::connect(pWorker,SIGNAL(emulatorStarted()),this,SIGNAL(emulatorStarted()));
}

NESEmulatorThread::~NESEmulatorThread()
{
   SDL_PauseAudio ( 1 );

   SDL_CloseAudio ();

   SDL_Quit();
}

void NESEmulatorThread::primeEmulator(CCartridge* pCartridge)
{
   pWorker->primeEmulator(pCartridge);
}

void NESEmulatorThread::softResetEmulator()
{
   pWorker->softResetEmulator();
}

void NESEmulatorThread::resetEmulator()
{
   pWorker->resetEmulator();
}

void NESEmulatorThread::startEmulation ()
{
   pWorker->startEmulation();
}

void NESEmulatorThread::pauseEmulation (bool show)
{
   pWorker->pauseEmulation(show);
}

bool NESEmulatorThread::serialize(QDomDocument& doc, QDomNode& node)
{
   return pWorker->serialize(doc,node);
}

bool NESEmulatorThread::serializeContent(QFile& fileOut)
{
   return pWorker->serializeContent(fileOut);
}

bool NESEmulatorThread::deserialize(QDomDocument& doc, QDomNode& node, QString& errors)
{
   return pWorker->deserialize(doc,node,errors);
}

bool NESEmulatorThread::deserializeContent(QFile& fileIn)
{
   return pWorker->deserializeContent(fileIn);
}

NESEmulatorWorker::NESEmulatorWorker(QObject*)
{
   m_joy [ CONTROLLER1 ] = 0;
   m_joy [ CONTROLLER2 ] = 0;
   m_isRunning = false;
   m_isPaused = false;
   m_showOnPause = false;
   m_isStarting = false;
   m_isTerminating = false;
   m_isResetting = false;
   m_pCartridge = NULL;

   pTimer = new QTimer();
   QObject::connect(pTimer,SIGNAL(timeout()),this,SLOT(process()));
   pTimer->start(10);
}

NESEmulatorWorker::~NESEmulatorWorker()
{
   pTimer->stop();
}

void NESEmulatorWorker::primeEmulator(CCartridge* pCartridge)
{
   m_pCartridge = pCartridge;
}

void NESEmulatorWorker::loadCartridge()
{
   int32_t b;

   // Prepare (frontload) cartridge...
   nesFrontload(m_pCartridge->getMapperNumber());

   // Load cartridge PRG-ROM banks into emulator...
   for ( b = 0; b < m_pCartridge->getNumPrgRomBanks(); b++ )
   {
      nesLoadPRGROMBank(b,(uint8_t*)m_pCartridge->getPointerToPrgRomBank(b));
   }

   // Load cartridge CHR-ROM banks into emulator...
   for ( b = 0; b < m_pCartridge->getNumChrRomBanks(); b++ )
   {
      nesLoadCHRROMBank(b,(uint8_t*)m_pCartridge->getPointerToChrRomBank(b));
   }

   // Perform any necessary fixup from the ROM loading...
   nesFinalizeLoad();

   // Set up PPU with iNES header information...
   if ( m_pCartridge->getMirrorMode() == HorizontalMirroring )
   {
      nesSetHorizontalMirroring();
   }
   else if ( m_pCartridge->getMirrorMode() == VerticalMirroring )
   {
      nesSetVerticalMirroring();
   }
   if ( m_pCartridge->getFourScreen() )
   {
      nesSetFourScreen();
   }

   // Initialize NES...
   nesResetInitial();

   if ( !m_pCartridge->getSaveStateFile().isEmpty() )
   {
      QFile saveState( m_pCartridge->getSaveStateFile() );
#if defined(XML_SAVE_STATE)
      QDomDocument saveDoc;
      QString errors;

      if (saveState.open(QFile::ReadOnly))
      {
         saveDoc.setContent(saveState.readAll());
      }
      saveState.close();

      deserialize(saveDoc,saveDoc,errors);
#endif

      if ( saveState.open(QIODevice::ReadOnly) )
      {
         deserializeContent(saveState);
      }
   }

   // Turn off replay...
   nesSetInputPlayback(false);

   emit cartridgeLoaded();
}

void NESEmulatorWorker::softResetEmulator()
{
   m_isResetting = true;
   m_isSoftReset = true;
   m_isStarting = false;
   m_isRunning = false;
   m_isPaused = true;
   m_showOnPause = false;
}

void NESEmulatorWorker::resetEmulator()
{
   m_isResetting = true;
   m_isSoftReset = false;
   m_isStarting = false;
   m_isRunning = false;
   m_isPaused = true;
   m_showOnPause = false;
}

void NESEmulatorWorker::startEmulation ()
{
   m_isStarting = true;
}

void NESEmulatorWorker::pauseEmulation (bool show)
{
   m_isStarting = false;
   m_isRunning = false;
   m_isPaused = true;
   m_showOnPause = show;
}

void NESEmulatorWorker::process ()
{
   QWidget* emulatorWidget = MainWindow::me(); // Hacky, but works for now.
   int scaleX;
   int scaleY;
   int scale;
   int emuX;
   int emuY;

//   while ( m_isStarting || m_isRunning || m_isResetting || m_isPaused )
   {
      // Allow thread exit...
//      if ( m_isTerminating )
//      {
//         break;
//      }

      // Allow thread to keep going...
      if ( m_isStarting )
      {
         m_isStarting = false;
         m_isRunning = true;
         m_isPaused = false;

         // Trigger UI updates...
         emit emulatorStarted();
      }

      // Properly coordinate NES reset with emulator...
      if ( m_isResetting )
      {
         // Reset emulated I/O devices...
         m_joy [ CONTROLLER1 ] = 0;
         m_joy [ CONTROLLER2 ] = 0;

         // Reset NES...
         nesReset(m_isSoftReset);

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
         // Run emulator for one frame...
         if ( emulatorWidget )
         {
            // Figure out where in the window the actual emulator display is.
            scaleX = emulatorWidget->geometry().width()/256;
            scaleY = emulatorWidget->geometry().height()/240;
            scale = (scaleX<scaleY)?scaleX:scaleY;
            if ( scale == 0 ) scale = 1;
            emuX = (emulatorWidget->geometry().x()+(emulatorWidget->geometry().width()/2))-(128*scale);
            if ( emuX < 0 ) emuX = 0;
            emuY = (emulatorWidget->geometry().y()+(emulatorWidget->geometry().height()/2))-(120*scale);
            if ( emuY < 0 ) emuY = 0;

            // Note, only need to check CCW for Vaus since both CCW and CW rotation are mouse
            // controlled if one of them is.
            if ( (EmulatorPrefsDialog::getControllerType(CONTROLLER1) == IO_Zapper) ||
                 ((EmulatorPrefsDialog::getControllerType(CONTROLLER1) == IO_Vaus) &&
                 (EmulatorPrefsDialog::getControllerMouseMap(CONTROLLER1,IO_Vaus_CCW))) )
            {
               nesSetControllerScreenPosition(CONTROLLER1,
                                              QCursor::pos().x(),
                                              QCursor::pos().y(),
                                              emuX,
                                              emuY,
                                              emuX+(256*scale),
                                              emuY+(240*scale));
            }
            if ( (EmulatorPrefsDialog::getControllerType(CONTROLLER2) == IO_Zapper) ||
                 ((EmulatorPrefsDialog::getControllerType(CONTROLLER2) == IO_Vaus) &&
                 (EmulatorPrefsDialog::getControllerMouseMap(CONTROLLER2,IO_Vaus_CCW))) )
            {
               nesSetControllerScreenPosition(CONTROLLER2,
                                              QCursor::pos().x(),
                                              QCursor::pos().y(),
                                              emuX,
                                              emuY,
                                              emuX+(256*scale),
                                              emuY+(240*scale));
            }
         }
         nesRun(m_joy);

         emit emulatedFrame();
      }

      // Pause?
      if ( m_isPaused )
      {
         // Trigger inspectors to update on a pause also...
         emit emulatorPaused(m_showOnPause);
         m_isPaused = false;
         m_isRunning = false;
      }
   }

   return;
}

bool NESEmulatorWorker::serialize(QDomDocument& doc, QDomNode& node)
{
   QString cartMem;
   QString ppuMem;
   char byte[3];
   int  idx;

   // Save state.
   QDomElement saveElement = addElement ( doc, node, "save" );
   // Serialize the CPU state.
   QDomElement cpuElement = addElement( doc, saveElement, "cpu" );

   // Serialize the CPU registers.
   QDomElement cpuRegsElement = addElement(doc,cpuElement,"registers");

   cpuRegsElement.setAttribute("pc",nesGetCPURegister(CPU_PC));
   cpuRegsElement.setAttribute("sp",nesGetCPURegister(CPU_SP));
   cpuRegsElement.setAttribute("a",nesGetCPURegister(CPU_A));
   cpuRegsElement.setAttribute("x",nesGetCPURegister(CPU_X));
   cpuRegsElement.setAttribute("y",nesGetCPURegister(CPU_Y));
   cpuRegsElement.setAttribute("f",nesGetCPURegister(CPU_F));

   // Serialize the CPU memory.
   QDomElement cpuMemElement = addElement(doc,cpuElement,"memory");
   QDomCDATASection cpuMemDataSect;
   QString cpuMem;

   for ( idx = 0; idx < MEM_2KB; idx++ )
   {
      sprintf(byte,"%02X",nesGetCPUMemory(idx));
      cpuMem += byte;
   }
   cpuMemDataSect = doc.createCDATASection(cpuMem);
   cpuMemElement.appendChild(cpuMemDataSect);

   // Serialize the PPU state.
   QDomElement ppuElement = addElement( doc, saveElement, "ppu" );

   // Serialize the PPU registers.
   QDomElement ppuRegsElement = addElement(doc,ppuElement,"registers");

   QDomCDATASection ppuRegsDataSect;
   QString ppuRegs;

   for ( idx = 0; idx < MEM_8B; idx++ )
   {
      sprintf(byte,"%02X",nesGetPPURegister(0x2000+idx));
      ppuRegs += byte;
   }
   ppuRegsDataSect = doc.createCDATASection(ppuRegs);
   ppuRegsElement.appendChild(ppuRegsDataSect);

   // Serialize the PPU memory.
   QDomElement ppuMemElement = addElement(doc,ppuElement,"memory");
   QDomCDATASection ppuMemDataSect;

   ppuMem.clear();
   for ( idx = 0; idx < MEM_8KB; idx++ )
   {
      sprintf(byte,"%02X",nesGetPPUMemory(0x2000+idx));
      ppuMem += byte;
   }
   ppuMemDataSect = doc.createCDATASection(ppuMem);
   ppuMemElement.appendChild(ppuMemDataSect);

   // Serialize the PPU OAM memory.
   QDomElement ppuOamMemElement = addElement(doc,ppuElement,"oam");
   QDomCDATASection ppuOamMemDataSect;

   ppuMem.clear();
   for ( idx = 0; idx < MEM_256B; idx++ )
   {
      sprintf(byte,"%02X",nesGetPPUOAM(idx));
      ppuMem += byte;
   }
   ppuOamMemDataSect = doc.createCDATASection(ppuMem);
   ppuOamMemElement.appendChild(ppuOamMemDataSect);

   // Serialize the APU state.
   QDomElement apuElement = addElement( doc, saveElement, "apu" );

   // Serialize the APU registers.
   QDomElement apuRegsElement = addElement(doc,apuElement,"registers");

   QDomCDATASection apuRegsDataSect;
   QString apuRegs;

   for ( idx = 0; idx < MEM_32B; idx++ )
   {
      sprintf(byte,"%02X",nesGetAPURegister(0x4000+idx));
      apuRegs += byte;
   }
   apuRegsDataSect = doc.createCDATASection(apuRegs);
   apuRegsElement.appendChild(apuRegsDataSect);

   // Serialize the Cartridge state.
   QDomElement cartElement = addElement( doc, saveElement, "cartridge" );

   // Serialize the Cartridge SRAM memory.
   QDomElement cartSramMemElement = addElement( doc, cartElement, "sram" );
   QDomCDATASection cartSramMemDataSect;

   cartMem.clear();
   for ( idx = 0; idx < MEM_64KB; idx++ )
   {
      sprintf(byte,"%02X",nesGetSRAMDataPhysical(idx));
      cartMem += byte;
   }
   cartSramMemDataSect = doc.createCDATASection(cartMem);
   cartSramMemElement.appendChild(cartSramMemDataSect);

   // Serialize the Cartridge EXRAM memory.
   QDomElement cartExramMemElement = addElement( doc, cartElement, "exram" );
   QDomCDATASection cartExramMemDataSect;

   cartMem.clear();
   for ( idx = 0; idx < MEM_1KB; idx++ )
   {
      sprintf(byte,"%02X",nesGetEXRAMData(0x5C00+idx));
      cartMem += byte;
   }
   cartExramMemDataSect = doc.createCDATASection(cartMem);
   cartExramMemElement.appendChild(cartExramMemDataSect);

   return true;
}

bool NESEmulatorWorker::serializeContent(QFile& fileOut)
{
   QByteArray bytesToWrite;
   qint64     bytesWritten;
   int idx;

   for ( idx = 0; idx < MEM_64KB; idx++ )
   {
      bytesToWrite += nesGetSRAMDataPhysical(idx);
   }

   bytesWritten = fileOut.write(bytesToWrite);

   return bytesWritten > 0;
}

bool NESEmulatorWorker::deserialize(QDomDocument& doc, QDomNode& node, QString& errors)
{
   // Loop through the child elements and process the ones we find
   QDomElement saveStateElement = doc.documentElement();
   QDomNode child = saveStateElement.firstChild();
   QDomNode childsChild;
   QDomNode cdataNode;
   QDomElement childsElement;
   QDomCDATASection cdataSection;
   QString cdataString;
   int idx;
   char byte;

   do
   {
// CPTODO: For now we save lots of crap in serialize but only
//         restore the SRAM content on deserialize.  Having a
//         cycle-perfect "pick up where i left off" option just
//         isn't very feasible and not really necessary with the
//         IDE.  The standalone emulator may do it differently.
      if (child.nodeName() == "cpu")
      {
         childsChild = child.firstChild();
         do
         {
            if ( childsChild.nodeName() == "registers" )
            {
               childsElement = childsChild.toElement();
               nesSetCPURegister(CPU_PC,childsElement.attribute("pc").toInt());
               nesSetCPURegister(CPU_SP,childsElement.attribute("sp").toInt());
               nesSetCPURegister(CPU_A,childsElement.attribute("a").toInt());
               nesSetCPURegister(CPU_X,childsElement.attribute("x").toInt());
               nesSetCPURegister(CPU_Y,childsElement.attribute("y").toInt());
               nesSetCPURegister(CPU_F,childsElement.attribute("f").toInt());
            }
            else if ( childsChild.nodeName() == "memory" )
            {
               cdataNode = childsChild.firstChild();
               cdataSection = cdataNode.toCDATASection();
               cdataString = cdataSection.data();
               for ( idx = 0; idx < MEM_2KB; idx++ )
               {
                  byte = cdataString.left(2).toInt(0,16);
                  cdataString = cdataString.right(cdataString.length()-2);
                  nesSetCPUMemory(idx,byte);
               }
            }
         }
         while (!(childsChild = childsChild.nextSibling()).isNull());
      }
      else if (child.nodeName() == "ppu")
      {
         childsChild = child.firstChild();
         do
         {
            if ( childsChild.nodeName() == "registers" )
            {
               cdataNode = childsChild.firstChild();
               cdataSection = cdataNode.toCDATASection();
               cdataString = cdataSection.data();
               for ( idx = 0; idx < MEM_8B; idx++ )
               {
                  byte = cdataString.left(2).toInt(0,16);
                  cdataString = cdataString.right(cdataString.length()-2);
                  nesSetPPURegister(0x2000+idx,byte);
               }
            }
            else if ( childsChild.nodeName() == "memory" )
            {
               cdataNode = childsChild.firstChild();
               cdataSection = cdataNode.toCDATASection();
               cdataString = cdataSection.data();
               for ( idx = 0; idx < MEM_8KB; idx++ )
               {
                  byte = cdataString.left(2).toInt(0,16);
                  cdataString = cdataString.right(cdataString.length()-2);
                  nesSetPPUMemory(0x2000+idx,byte);
               }
            }
            else if ( childsChild.nodeName() == "oam" )
            {
               cdataNode = childsChild.firstChild();
               cdataSection = cdataNode.toCDATASection();
               cdataString = cdataSection.data();
               for ( idx = 0; idx < MEM_256B; idx++ )
               {
                  byte = cdataString.left(2).toInt(0,16);
                  cdataString = cdataString.right(cdataString.length()-2);
                  nesSetPPUOAM(idx,byte);
               }
            }
         }
         while (!(childsChild = childsChild.nextSibling()).isNull());
      }
      else if (child.nodeName() == "apu")
      {
         childsChild = child.firstChild();
         do
         {
            if ( childsChild.nodeName() == "registers" )
            {
               cdataNode = childsChild.firstChild();
               cdataSection = cdataNode.toCDATASection();
               cdataString = cdataSection.data();
               for ( idx = 0; idx < MEM_32B; idx++ )
               {
                  byte = cdataString.left(2).toInt(0,16);
                  cdataString = cdataString.right(cdataString.length()-2);
                  nesSetAPURegister(0x4000+idx,byte);
               }
            }
         }
         while (!(childsChild = childsChild.nextSibling()).isNull());
      }
      else if (child.nodeName() == "cartridge")
      {
         childsChild = child.firstChild();
         do
         {
            if ( childsChild.nodeName() == "sram" )
            {
               cdataNode = childsChild.firstChild();
               cdataSection = cdataNode.toCDATASection();
               cdataString = cdataSection.data();
               for ( idx = 0; idx < MEM_64KB; idx++ )
               {
                  byte = cdataString.left(2).toInt(0,16);
                  cdataString = cdataString.right(cdataString.length()-2);
                  nesLoadSRAMDataPhysical(idx,byte);
               }
            }
         }
         while (!(childsChild = childsChild.nextSibling()).isNull());
      }
   }
   while (!(child = child.nextSibling()).isNull());

   return true;
}

bool NESEmulatorWorker::deserializeContent(QFile& fileIn)
{
   QByteArray bytes;
   int idx;
   char zero = 0;

   bytes = fileIn.readAll();

   if ( bytes.count() != MEM_64KB )
   {
      if ( bytes.count() < MEM_64KB )
      {
         for ( idx = bytes.count(); idx < MEM_64KB; idx++ )
         {
            bytes.append(zero);
         }
      }

      QString str;
      str = "The save file found for this ROM:\n\n";
      str += fileIn.fileName();
      str += "\n\nis not 64KB in size and may not be\n";
      str += "valid.  Game save data may be lost.";
//      QMessageBox::warning(0,"Save file corrupted?",str);
      bytes.truncate(MEM_64KB);
   }

   for ( idx = 0; idx < bytes.count(); idx++ )
   {
      nesSetSRAMDataPhysical(idx,bytes.at(idx));
   }

   return true;
}



