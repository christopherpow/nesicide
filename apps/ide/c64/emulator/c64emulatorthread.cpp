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

#include <QRegExp>
#include <QMessageBox>
#include <QVector>

#include <cdockwidgetregistry.h>

#include "c64emulatorthread.h"

#include "c64_emulator_core.h"

#include "ccc65interface.h"

#include "emulatorprefsdialog.h"
#include "cobjectregistry.h"
#include "breakpointwatcherthread.h"
#include "main.h"

static void breakpointHook ( void )
{
   // Tell the world.
   C64EmulatorThread* emulator = dynamic_cast<C64EmulatorThread*>(CObjectRegistry::getObject("Emulator"));
   emulator->_breakpointHook();
}

void C64EmulatorThread::_breakpointHook()
{
   emit breakpoint();
}

C64EmulatorThread::C64EmulatorThread(QObject*)
{
   QDir dir(EmulatorPrefsDialog::getVICEExecutable());
   QString viceStartup;

   viceStartup = EmulatorPrefsDialog::getViceInvocation();
   
   m_pViceApp = new QProcess(this);
   QObject::connect(m_pViceApp,SIGNAL(started()),this,SLOT(viceStarted()));
   QObject::connect(m_pViceApp,SIGNAL(error(QProcess::ProcessError)),this,SLOT(viceError(QProcess::ProcessError)));
   QObject::connect(m_pViceApp,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(viceFinished(int,QProcess::ExitStatus)));
   m_pViceApp->start(viceStartup);
   
   // Enable breakpoint callbacks from the external emulator library.
   c64SetBreakpointHook(breakpointHook);
   
   BreakpointWatcherThread* breakpointWatcher = dynamic_cast<BreakpointWatcherThread*>(CObjectRegistry::getObject("Breakpoint Watcher"));
   QObject::connect(this,SIGNAL(breakpoint()),breakpointWatcher,SLOT(breakpoint()));

   m_requestMutex = new QMutex();

   m_isRunning = false;
}

C64EmulatorThread::~C64EmulatorThread()
{
   delete m_requestMutex;

   QObject::disconnect(m_pViceApp,SIGNAL(error(QProcess::ProcessError)),this,SLOT(viceError(QProcess::ProcessError)));
   QObject::disconnect(m_pViceApp,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(viceFinished(int,QProcess::ExitStatus)));

   m_pViceApp->kill();
   delete m_pViceApp;
}

void C64EmulatorThread::timerEvent(QTimerEvent */*event*/)
{
}

void C64EmulatorThread::viceStarted()
{
   // Close the pipes.
   m_pViceApp->closeReadChannel(QProcess::StandardError);
   m_pViceApp->closeReadChannel(QProcess::StandardOutput);
   m_pViceApp->closeWriteChannel();

   m_pClient = new TcpClient(EmulatorPrefsDialog::getVICEIPAddress(),EmulatorPrefsDialog::getVICEMonitorPort());
   m_pClient->moveToThread(this);

   qRegisterMetaType<QList<int>>("QList<int>");

   QObject::connect(m_pClient,SIGNAL(responses(QStringList,QStringList)),this,SLOT(processResponses(QStringList,QStringList)));
   QObject::connect(this,SIGNAL(sendRequests(QStringList,QList<int>)),m_pClient,SLOT(sendRequests(QStringList,QList<int>)));
   QObject::connect(m_pClient,SIGNAL(traps(QString)),this,SLOT(processTraps(QString)));
   QObject::connect(m_pClient,SIGNAL(clientConnected()),this,SIGNAL(emulatorConnected()));
   QObject::connect(m_pClient,SIGNAL(clientDisconnected()),this,SIGNAL(emulatorDisconnected()));

   qDebug("VICE started, starting thread in 2sec.");
   runRequestQueue(); // clear any pending requests

   sleep(2);
   
   // Start the thread.
   start();
}

void C64EmulatorThread::viceError(QProcess::ProcessError error)
{
   EmulatorPrefsDialog dlg("c64");
   QString viceStartup;
   int result;

#if 0
   QProcess::FailedToStart	0	The process failed to start. Either the invoked program is missing, or you may have insufficient permissions to invoke the program.
   QProcess::Crashed	1	The process crashed some time after starting successfully.
   QProcess::Timedout	2	The last waitFor...() function timed out. The state of QProcess is unchanged, and you can try calling waitFor...() again.
   QProcess::WriteError	4	An error occurred when attempting to write to the process. For example, the process may not be running, or it may have closed its input channel.
   QProcess::ReadError	3	An error occurred when attempting to read from the process. For example, the process may not be running.
   QProcess::UnknownError
#endif

   switch ( error )
   {
   case QProcess::FailedToStart:
      result = QMessageBox::warning(0,"VICE not found!","The VICE Commodore 64 emulator, x64sc, failed to start.\n"
                           "Please set the path to it in NESICIDE's Emulator Preferences dialog.","Exit","Fix","",1,-1);

      if ( result == 1 )
      {
         dlg.exec();
      }
      else
      {
         emit emulatorWantsExit();
         exit();
      }

      viceStartup = EmulatorPrefsDialog::getViceInvocation();
      m_pViceApp->start(viceStartup);
      break;
   }
}

void C64EmulatorThread::viceFinished(int /*exitCode*/,QProcess::ExitStatus /*exitStatus*/)
{
   EmulatorPrefsDialog dlg("c64");
   QString viceStartup;
   QString str = "The VICE Commodore 64 emulator, x64sc, has exited unexpectedly.\n"
                 "Debugging this project cannot continue.\n\n"
                 "If you know this is a problem with NESICIDE's configuration, click\n"
                 "'Fix' to fix it."
                 "\n\nOutput from VICE:\n";

   str += m_pViceApp->readAll();

   int result = QMessageBox::warning(0,"VICE exited!",str,"Exit","Fix","",1,-1);

   if ( result == 1 )
   {
      dlg.exec();

      viceStartup = EmulatorPrefsDialog::getViceInvocation();
      m_pViceApp->start(viceStartup);
   }
   else
   {
      emit emulatorWantsExit();
      exit();
   }
}

void C64EmulatorThread::kill()
{
   lockRequestQueue();
   clearRequestQueue();
   addToRequestQueue("until $ffff",0); // using "exit" doesn't seem to work.
   runRequestQueue();
   unlockRequestQueue();

   // Force hard-reset of the machine...
   c64EnableBreakpoints(false);

   // Get outta here!
   exit();
}

void C64EmulatorThread::breakpointsChanged()
{
   lockRequestQueue();
   clearRequestQueue();
   addToRequestQueue("break",1); // Get/update breakpoints on emulated machine.

   // If the emulator is running, restart it after this interruption.
   if ( m_isRunning )
   {
      addToRequestQueue("until $ffff",0);
   }
   runRequestQueue();
   unlockRequestQueue();
}

void C64EmulatorThread::primeEmulator()
{
   // Nothing to do here, it's all done in resetEmulator.
}

void C64EmulatorThread::resetEmulator()
{
   // Force hard-reset of the machine...
   c64EnableBreakpoints(false);

   if ( nesicideProject->isInitialized() )
   {
      QDir dirProject(nesicideProject->getProjectOutputBasePath());
      QString fileName = dirProject.toNativeSeparators(dirProject.absoluteFilePath(nesicideProject->getProjectLinkerOutputName()));
      QString request;
      int addr;
      qDebug("C64EmulatorThread::resetEmulator ...%s...\n",fileName.toLatin1().data());

      if ( fileName.endsWith(".c64",Qt::CaseInsensitive) ||
           fileName.endsWith(".prg",Qt::CaseInsensitive) )
      {
         addr = CCC65Interface::getSegmentBase("STARTUP");

         lockRequestQueue();
         clearRequestQueue();
         addToRequestQueue("until $a474",0);
         addToRequestQueue("reset",1);
         if ( addr > 0 )
         {
            request = "load \"" + fileName + "\" 0";
            addToRequestQueue(request.toLatin1(),1);
            request = "r pc = $" + QString::number(addr,16);
            addToRequestQueue(request,0);
            addToRequestQueue("r",1);
            addToRequestQueue("io",1);
            addToRequestQueue("m $0 $ffff",1);
            addToRequestQueue("break",1);
         }
         runRequestQueue();
         unlockRequestQueue();
      }
      else if ( fileName.endsWith(".d64",Qt::CaseInsensitive) )
      {
         lockRequestQueue();
         clearRequestQueue();
         addToRequestQueue("until $a474",0);
         addToRequestQueue("reset",1);
         request = "autostart \"" + fileName + "\"";
         addToRequestQueue(request.toLatin1(),2);
         addToRequestQueue("r",1);
         addToRequestQueue("io",1);
//         addToRequestQueue("m $0 $ffff",1);
         addToRequestQueue("break",1);
         addToRequestQueue("until $ffff",0);
         runRequestQueue();
         unlockRequestQueue();
      }
   }
}

void C64EmulatorThread::startEmulation ()
{
   m_isRunning = true;

   lockRequestQueue();
   clearRequestQueue();
   addToRequestQueue("until $ffff",0); // using "exit" doesn't seem to work.
   runRequestQueue();
   unlockRequestQueue();
}

void C64EmulatorThread::stepCPUEmulation ()
{
   QString request;
   uint32_t endAddr;
   uint32_t addr;
   uint32_t absAddr;

   // Check if we have an end address to stop at from a debug information file.
   // If we do, it'll be the valid end of a C statement or an assembly instruction.
   addr = c64GetCPURegister(CPU_PC);
   absAddr = c64GetPhysicalAddressFromAddress(addr);
   endAddr = CCC65Interface::getEndAddressFromPhysicalAddress(addr,absAddr);

   if ( endAddr != 0xFFFFFFFF )
   {
      // Find the last opcode in the C-statement.
      for ( ; endAddr > absAddr; endAddr-- )
      {
         if ( CCC65Interface::isPhysicalAddressAnOpcode(endAddr) )
         {
            break;
         }
      }
      
      c64SetGotoAddress(endAddr);

      lockRequestQueue();
      clearRequestQueue();
      if ( endAddr == absAddr )
      {
         addToRequestQueue("step",1);
      }
      else
      {
         request = "until $" + QString::number(endAddr,16);
         addToRequestQueue(request,1);
         addToRequestQueue("until $ffff",0); // using "exit" doesn't seem to work.
         addToRequestQueue("step",1);
      }
      addToRequestQueue("r",1);
      addToRequestQueue("io",1);
      addToRequestQueue("m $0 $ffff",1);
      runRequestQueue();
      unlockRequestQueue();
   }
   else
   {
      lockRequestQueue();
      clearRequestQueue();
      addToRequestQueue("step",1);
      addToRequestQueue("r",1);
      addToRequestQueue("io",1);
      addToRequestQueue("m $0 $ffff",1);
      runRequestQueue();
      unlockRequestQueue();
   }
}

void C64EmulatorThread::stepOverCPUEmulation ()
{
   QString request;
   uint32_t endAddr;
   uint32_t addr;
   uint32_t absAddr;
   uint32_t instAbsAddr;
   bool    isInstr;
   uint8_t instr;

   // Check if we have an end address to stop at from a debug information file.
   // If we do, it'll be the valid end of a C statement or an assembly instruction.
   addr = c64GetCPURegister(CPU_PC);
   absAddr = c64GetPhysicalAddressFromAddress(addr);
   endAddr = CCC65Interface::getEndAddressFromPhysicalAddress(addr,absAddr);

   if ( endAddr != 0xFFFFFFFF )
   {
      // If the line has enough of an assembly-stream associated with it...
      if ( endAddr-addr >= 2 )
      {
         // Check if last instruction on line is JSR...
         // This is fairly typical of if conditions with function calls on the same line.
         instr = c64GetMemory(endAddr-2);
         instAbsAddr = c64GetPhysicalAddressFromAddress(endAddr-2);
         isInstr = CCC65Interface::isPhysicalAddressAnOpcode(instAbsAddr);
         if ( !isInstr )
         {
            instr = c64GetMemory(addr);
         }
      }
      else
      {
         instr = c64GetMemory(addr);
      }
   }
   else
   {
      // Check if the current instruction is a JSR...
      instr = c64GetMemory(addr);

      // Assume the instruction is JSR for loop below.
      endAddr = addr+2;
   }

   // If the current instruction is a JSR we need to tell the emulator to
   // go to the PC one past the JSR to 'step over' the JSR.
   if ( instr == JSR_ABSOLUTE )
   {
      // Go to next opcode point in ROM.
      // This *should* be where the JSR will vector back to on RTS.
      c64SetGotoAddress(endAddr+1);

      lockRequestQueue();
      clearRequestQueue();
      request = "until $" + QString::number(endAddr+1,16);
      addToRequestQueue(request,1);
      addToRequestQueue("until $ffff",0); // using "exit" doesn't seem to work.
      addToRequestQueue("r",1);
      addToRequestQueue("io",1);
      addToRequestQueue("m $0 $ffff",1);
      runRequestQueue();
      unlockRequestQueue();
   }
   else
   {
      stepCPUEmulation();
   }
}

void C64EmulatorThread::stepOutCPUEmulation ()
{
   lockRequestQueue();
   clearRequestQueue();
   addToRequestQueue("next",1);
   addToRequestQueue("r",1);
   addToRequestQueue("io",1);
   addToRequestQueue("m $0 $ffff",1);
   runRequestQueue();
   unlockRequestQueue();
}

void C64EmulatorThread::pauseEmulation (bool show)
{
   m_isRunning = false;
   m_showOnPause = show;

   lockRequestQueue();
   clearRequestQueue();
   addToRequestQueue("r",1);
   addToRequestQueue("io",1);
   addToRequestQueue("m $0 $ffff",1);
   runRequestQueue();
   unlockRequestQueue();
}

void C64EmulatorThread::clearRequestQueue()
{
   m_requests.clear();
   m_responses.clear();
   m_responseExpected.clear();
}

void C64EmulatorThread::addToRequestQueue(QString request,int expecting)
{
   request.append('\n');
   m_requests.append(request);
   m_responseExpected.append(expecting);
}

void C64EmulatorThread::runRequestQueue()
{
   if ( m_requests.count() )
   {
      m_requests.append("END");
      m_responseExpected.append(0);

      emit sendRequests(m_requests,m_responseExpected);
   }
}

void C64EmulatorThread::processTraps(QString traps)
{
   CBreakpointInfo* pBreakpoints = c64GetBreakpointDatabase();
   QStringList trapLines;
   QRegExp bpRegex("[#]([0-9]+)[ \t]+[(]Stop[ \t]+on[ \t]+exec[ \t]+([0-9a-f]+)[)] ");
   QString trap;
   int bp;
   int trapLine;
   bool ok;
   bool hook = false;

   // First append this message chunk to the message string we're holding on to.
   m_traps += traps;

   // Then split the message string into lines, throwing away empties.
   trapLines = m_traps.split(QRegExp("[\n]"),QString::SkipEmptyParts);

   // For each line, check if it is a breakpoint hit indication.
   // Don't check the last line as it may not yet be completely received.
   for ( trapLine = 0; trapLine < trapLines.count()-1; trapLine++ )
   {
      trap = trapLines.at(trapLine);
      if ( trap.contains(bpRegex) && (bpRegex.captureCount() == 2) )
      {
         qDebug("TRAP:");
         qDebug(trap.toLatin1().constData());

         // Figure out which breakpoint hit this is.
         for ( bp = 0; bp < pBreakpoints->GetNumBreakpoints(); bp++ )
         {
            BreakpointInfo* pBreakpoint = pBreakpoints->GetBreakpoint(bp);
            uint32_t addr = bpRegex.cap(2).toInt(&ok,16);

            // And prepare to tell the UI.
            pBreakpoint->hit = false;
            if ( (pBreakpoint->item1 <= addr) && (pBreakpoint->item2 >= addr) )
            {
               qDebug("HIT BREAKPOINT %d",bp);
               pBreakpoint->hit = true;
               hook = true;
            }
         }
      }

      // Since we've done something with this part of the
      // trap message string, null it out so we don't do it again.
      trapLines.replace(trapLine,"");
   }

   // Finally, put the message string back together as one long bunch.
   m_traps = trapLines.join("\n");

   // If we found a breakpoint hit, tell the UI.
   if ( hook )
   {
      lockRequestQueue();
      clearRequestQueue();
      addToRequestQueue("r",1);
      addToRequestQueue("io",1);
      addToRequestQueue("m $0 $ffff",1);
      runRequestQueue();
      unlockRequestQueue();

      breakpointHook();
   }
}

void C64EmulatorThread::processResponses(QStringList requests,QStringList responses)
{
   CBreakpointInfo* pBreakpoints = c64GetBreakpointDatabase();
   int resp;
   int32_t a;
   int bp;

#if 0
   for ( int resp = 0; resp < requests.count(); resp++ )
   {
      QString str;
      if ( requests.at(resp) != "END" )
      {
         str = QString::number(resp);
         str += " Request: \n";
         str += requests.at(resp);
         str += "\nResponse: \n";
         if ( requests.at(resp).startsWith("m ") )
         {
            str += QString::number(responses.at(resp).length());
         }
         else
         {
            str += responses.at(resp);
         }
         qDebug(str.toLatin1().constData());
      }
   }
#endif

   for ( resp = 0; resp < responses.count(); resp++ )
   {
      // Check if we need to update debuggers.
      if ( requests.at(resp).contains("break\n") )
      {
         // Create a mapping of break address to breakpoint-number
         QStringList targetBreakpoints = responses.at(resp).split(QRegExp("[\n]"),QString::SkipEmptyParts);
         QVector<QString> bpVector;
         QVector<QString>::const_iterator bpIterator;
         QRegExp bpRegex("(BREAK|WATCH|UNTIL):[ \t]+([0-9]+)");
         QString bpText;

         // Find all breakpoints reported by the target.  Map them so we can use them.
         foreach ( QString bpText, targetBreakpoints )
         {
            if ( bpText.contains(bpRegex) && (bpRegex.captureCount() == 2) )
            {
               bpVector.append(bpRegex.cap(2));
            }
         }

         // Delete all breakpoints...
         lockRequestQueue();
         clearRequestQueue();
         for ( bpIterator = bpVector.constBegin();
               bpIterator != bpVector.constEnd();
               ++bpIterator )
         {
            bpText = "delete ";
            bpText += bpIterator;
            addToRequestQueue(bpText,0);
         }

         // Add all enabled breakpoints...
         for ( bp = 0; bp < pBreakpoints->GetNumBreakpoints(); bp++ )
         {
            BreakpointInfo* pBreakpoint = pBreakpoints->GetBreakpoint(bp);

            if ( pBreakpoint->enabled )
            {
               if ( pBreakpoint->type == eBreakOnCPUExecution )
               {
                  bpText = "break exec $";
               }
               else if ( pBreakpoint->type == eBreakOnCPUMemoryAccess )
               {
                  bpText = "break load store $";
               }
               else if ( pBreakpoint->type == eBreakOnCPUMemoryRead )
               {
                  bpText = "break load $";
               }
               else if ( pBreakpoint->type == eBreakOnCPUMemoryWrite )
               {
                  bpText = "break store $";
               }
               bpText += QString::number(pBreakpoint->item1,16);
               if ( pBreakpoint->item2 != pBreakpoint->item1 )
               {
                  bpText += " $";
                  bpText += QString::number(pBreakpoint->item2,16);
               }
               addToRequestQueue(bpText,0);
            }
         }
         runRequestQueue();
         unlockRequestQueue();
      }
      else if ( requests.at(resp).startsWith("reset") )
      {
         emit emulatorReset();
      }
      else if ( requests.at(resp).startsWith("until") )
      {
         emit emulatorStarted();
      }
      else if ( requests.at(resp).startsWith("r pc") )
      {
         c64ClearOpcodeMasks();

         // Update opcode masks to show proper disassembly...
         for ( a = 0; a < MEM_64KB; a++ )
         {
            if ( CCC65Interface::isPhysicalAddressAnOpcode(a) )
            {
               c64SetOpcodeMask(a,1);
            }
            else
            {
               c64SetOpcodeMask(a,0);
            }
         }

         emit machineReady();
      }
      else if ( requests.at(resp) == "r\n" )
      {
         QStringList responseLines = responses.at(resp).split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
         QString responseHeader = responseLines.at(0);
         QString responseData = responseLines.at(1);

//         qDebug(responseHeader.toLatin1().constData());
         responseHeader.remove(QRegExp("^([ \r\n\t]*[(]C:[$][0-9a-f]+[)])*"));
//         qDebug(responseData.toLatin1().constData());
         responseData.remove(QRegExp("^([ \r\n\t]*[(]C:[$][0-9a-f]+[)])*"));

         QStringList responseHeaderParts = responseHeader.split(QRegExp("[ \t\n]"),QString::SkipEmptyParts);
         QStringList responseDataParts = responseData.split(QRegExp("[ \t\n]"),QString::SkipEmptyParts);
         QString str;
         int part;
         bool ok;

         for ( part = 0; part < responseHeaderParts.count(); part++ )
         {
            if ( responseHeaderParts.at(part) == "ADDR" )
            {
               str = responseDataParts.at(part);
               str.remove(QRegExp("^([.;])*"));
               c64SetCPURegister(CPU_PC,str.toInt(&ok,16));
            }
            if ( responseHeaderParts.at(part) == "AC" )
            {
               str = responseDataParts.at(part);
               str.remove(QRegExp("^([.;])*"));
               c64SetCPURegister(CPU_A,str.toInt(&ok,16));
            }
            if ( responseHeaderParts.at(part) == "XR" )
            {
               str = responseDataParts.at(part);
               str.remove(QRegExp("^([.;])*"));
               c64SetCPURegister(CPU_X,str.toInt(&ok,16));
            }
            if ( responseHeaderParts.at(part) == "YR" )
            {
               str = responseDataParts.at(part);
               str.remove(QRegExp("^([.;])*"));
               c64SetCPURegister(CPU_Y,str.toInt(&ok,16));
            }
            if ( responseHeaderParts.at(part) == "SP" )
            {
               str = responseDataParts.at(part);
               str.remove(QRegExp("^([.;])*"));
               c64SetCPURegister(CPU_SP,str.toInt(&ok,16));
            }
            if ( responseHeaderParts.at(part) == "NV-BDIZC" )
            {
               str = responseDataParts.at(part);
               str.remove(QRegExp("^([.;])*"));
               c64SetCPURegister(CPU_F,str.toInt(&ok,2));
            }
         }

         emit emulatorPaused(true);
         
         m_isRunning = false;
      }
      else if ( requests.at(resp).startsWith("m "))
      {
         QStringList responseLines = responses.at(resp).split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
         foreach ( QString responseData, responseLines )
         {
            responseData.remove(QRegExp("^([ \r\n\t]*[(]C:[$][0-9a-f]+[)])*"));

            QStringList responseParts = responseData.split(QRegExp("[ \t]"),QString::SkipEmptyParts);
            QString str;
            uint32_t addr;
            int part;
            bool ok;

            // Get address.
            if ( responseParts.count() )
            {
               str = responseParts.at(0);
               str.remove(QRegExp(">C:"));
               addr = str.toInt(&ok,16);

               for ( part = 0; part < responseParts.count()-1; part++ )
               {
                  str = responseParts.at(1+part);
                  c64SetMemory(addr+part,str.toInt(&ok,16));
               }
            }
         }
      }
      else if ( requests.at(resp).startsWith("io\n"))
      {
         QStringList responseLines = responses.at(resp).split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
         foreach ( QString responseData, responseLines )
         {
            responseData.remove(QRegExp("^([ \r\n\t]*[(]C:[$][0-9a-f]+[)])*"));

            QStringList responseParts = responseData.split(QRegExp("[ \t]"),QString::SkipEmptyParts);
            QString str;
            uint32_t addr;
            int part;
            bool ok;

            // Get address.
            if ( responseParts.count() )
            {
               str = responseParts.at(0);
               str.remove(QRegExp(">C:"));
               addr = str.toInt(&ok,16);

               for ( part = 0; part < responseParts.count()-1; part++ )
               {
                  str = responseParts.at(1+part);
                  c64SetMemory(addr+part,str.toInt(&ok,16));
               }
            }
         }
#if 0
         // Set up SID.
         for ( addr = SID_BASE; addr < SID_END; addr++ )
         {
            if ( (addr == 0xd400) ||
                 (addr == 0xd402) ||
                 (addr == 0xd407) ||
                 (addr == 0xd409) ||
                 (addr == 0xd40e) ||
                 (addr == 0xd410) )
            {
               c64SetSID16Register(addr,(c64GetMemory(addr)<<8)|c64GetMemory(addr+1));
            }
            else
            {
               c64SetSID8Register(addr,c64GetMemory(addr));
            }
         }
#endif
      }
   }
}

void C64EmulatorThread::lockRequestQueue()
{
   m_requestMutex->lock();
}

void C64EmulatorThread::unlockRequestQueue()
{
   m_requestMutex->unlock();
}

TcpClient::TcpClient(QString monitorIPAddress,int monitorPort,QObject */*parent*/)
   : m_ipAddress(monitorIPAddress),
     m_port(monitorPort)
{
   m_clientMutex = new QMutex();

   pSocket = new QTcpSocket(this);
   QObject::connect(pSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(error(QAbstractSocket::SocketError)));
   QObject::connect(pSocket,SIGNAL(connected()),this,SLOT(connected()));
   QObject::connect(pSocket,SIGNAL(disconnected()),this,SLOT(disconnected()));
   QObject::connect(pSocket,SIGNAL(readyRead()),this,SLOT(readyRead()));
   QObject::connect(pSocket,SIGNAL(bytesWritten(qint64)),this,SLOT(bytesWritten(qint64)));
   pSocket->connectToHost(m_ipAddress,m_port);

   m_request = 0;
}

TcpClient::~TcpClient()
{
   pSocket->close();
   delete pSocket;
}

void TcpClient::error(QAbstractSocket::SocketError error)
{
   qDebug("SOCKET ERROR");
   qDebug(QString::number((int)error).toLatin1().constData());
   switch ( error )
   {
   case QAbstractSocket::ConnectionRefusedError:
      pSocket->connectToHost(m_ipAddress,m_port);
      break;
   }

#if 0
   QAbstractSocket::ConnectionRefusedError	0	The connection was refused by the peer (or timed out).
   QAbstractSocket::RemoteHostClosedError	1	The remote host closed the connection. Note that the client socket (i.e., this socket) will be closed after the remote close notification has been sent.
   QAbstractSocket::HostNotFoundError	2	The host address was not found.
   QAbstractSocket::SocketAccessError	3	The socket operation failed because the application lacked the required privileges.
   QAbstractSocket::SocketResourceError	4	The local system ran out of resources (e.g., too many sockets).
   QAbstractSocket::SocketTimeoutError	5	The socket operation timed out.
   QAbstractSocket::DatagramTooLargeError	6	The datagram was larger than the operating system's limit (which can be as low as 8192 bytes).
   QAbstractSocket::NetworkError	7	An error occurred with the network (e.g., the network cable was accidentally plugged out).
   QAbstractSocket::AddressInUseError	8	The address specified to QUdpSocket::bind() is already in use and was set to be exclusive.
   QAbstractSocket::SocketAddressNotAvailableError	9	The address specified to QUdpSocket::bind() does not belong to the host.
   QAbstractSocket::UnsupportedSocketOperationError	10	The requested socket operation is not supported by the local operating system (e.g., lack of IPv6 support).
   QAbstractSocket::ProxyAuthenticationRequiredError	12	The socket is using a proxy, and the proxy requires authentication.
   QAbstractSocket::SslHandshakeFailedError	13	The SSL/TLS handshake failed, so the connection was closed (only used in QSslSocket)
   QAbstractSocket::UnfinishedSocketOperationError	11	Used by QAbstractSocketEngine only, The last operation attempted has not finished yet (still in progress in the background).
   QAbstractSocket::ProxyConnectionRefusedError	14	Could not contact the proxy server because the connection to that server was denied
   QAbstractSocket::ProxyConnectionClosedError	15	The connection to the proxy server was closed unexpectedly (before the connection to the final peer was established)
   QAbstractSocket::ProxyConnectionTimeoutError	16	The connection to the proxy server timed out or the proxy server stopped responding in the authentication phase.
   QAbstractSocket::ProxyNotFoundError	17	The proxy address set with setProxy() (or the application proxy) was not found.
   QAbstractSocket::ProxyProtocolError	18	The connection negotiation with the proxy server because the response from the proxy server could not be understood.
   QAbstractSocket::UnknownSocketError
#endif
}

void TcpClient::connected()
{
   emit clientConnected();

   m_clientMutex->lock();
   if ( m_requests.count() )
   {
      // Kick off writing anything that's been queued.
      pSocket->write(m_requests.at(0).toLatin1());
      debugTextLogger->write("<pre>\nrequest: "+m_requests.at(0)+"</pre>");
   }
   m_clientMutex->unlock();
   qDebug("SOCKET CONNECTED!");
}

void TcpClient::disconnected()
{
   emit clientDisconnected();
   qDebug("SOCKET DISCONNECTED!");
}

void TcpClient::sendRequests(QStringList requests,QList<int> expectings)
{
   int currentRequest;
   int req;

   m_clientMutex->lock();
   currentRequest = m_request;
   m_requests.append(requests);
   for ( req = 0; req < m_requests.count(); req++ )
   {
      m_requestSent.append(false);
   }
   m_expectDataInResponse.append(expectings);

   if ( (!currentRequest) &&
        (currentRequest < m_requests.count()) &&
        (!m_requestSent.at(currentRequest)) &&
        (pSocket->state() == QAbstractSocket::ConnectedState) )
   {
      // Kick off if nothing going on.
      pSocket->write(m_requests.at(0).toLatin1());
      debugTextLogger->write("<pre>\nrequest: "+m_requests.at(0)+"</pre>");
      m_requestSent.replace(0,true);
   }
   m_clientMutex->unlock();
}

void TcpClient::readyRead()
{
   QStringList returnRequests;
   QStringList returnResponses;
   QString     receivedData;
   QRegExp     regex("([\n][(]C:[$][0-9a-f]+[)]) ");
   int count;
   int currentRequest;

   receivedData = pSocket->readAll();
   debugTextLogger->write("<pre>\nresponse: "+receivedData+"</pre>");
   responseMessage.append(receivedData);

   emit traps(receivedData);

   m_clientMutex->lock();
   count = m_requests.count();
   currentRequest = m_request;

   m_clientMutex->unlock();

   if ( currentRequest >= (count-1) )
   {
      responseMessage.clear();
      return;
   }

   if ( (!m_expectDataInResponse.at(currentRequest)) || (responseMessage.lastIndexOf(regex) > 0) )
   {
      m_clientMutex->lock();
      m_responses.append(responseMessage);

      if ( m_expectDataInResponse.at(currentRequest) )
      {
         m_expectDataInResponse.replace(currentRequest,m_expectDataInResponse.takeAt(currentRequest)-1);
         if ( m_expectDataInResponse.at(currentRequest) != 0 )
         {
            return;
         }
      }

      m_request++;

      if ( m_requests.at(m_request) != "END" )
      {
//         qDebug(m_requests.at(m_request).toLatin1().constData());
         pSocket->write(m_requests.at(m_request).toLatin1());
         debugTextLogger->write("<pre>\nrequest: "+m_requests.at(0)+"</pre>");
         m_requestSent.replace(m_request,true);
      }
      else
      {
//         qDebug("FINISHED");
         // Remove the "END" marker.
         m_requests.removeAt(m_request);
         m_responses.removeAt(m_request);
         m_expectDataInResponse.removeAt(m_request);
         m_requestSent.removeAt(m_request);

         // Copy the requests/responses to return in this bundle.
         do
         {
            m_request--;
            returnRequests.prepend(m_requests.at(m_request));
            returnResponses.prepend(m_responses.at(m_request));
            m_requests.removeAt(m_request);
            m_responses.removeAt(m_request);
            m_expectDataInResponse.removeAt(m_request);
            m_requestSent.removeAt(m_request);
         } while ( m_request > 0 );

         // Give the responses back to the requestor.
         emit responses(returnRequests,returnResponses);

         // If new requests have come in, process those.  Otherwise
         // they'll be processed by the next bundle's arrival.
         if ( m_requests.count() )
         {
//            qDebug(m_requests.at(0).toLatin1().constData());
            pSocket->write("> "+m_requests.at(0).toLatin1());
         }
      }
      m_clientMutex->unlock();
      responseMessage.clear();
   }
}

void TcpClient::bytesWritten(qint64 bytes)
{
//   qDebug("bytesWritten");
//   qDebug(QString::number(bytes).toLatin1().constData());
}

bool C64EmulatorThread::serialize(QDomDocument& /*doc*/, QDomNode& /*node*/)
{
   return true;
}

bool C64EmulatorThread::serializeContent(QFile& /*fileOut*/)
{
   return true;
}

bool C64EmulatorThread::deserialize(QDomDocument& /*doc*/, QDomNode& /*node*/, QString& /*errors*/)
{
   return true;
}


bool C64EmulatorThread::deserializeContent(QFile& /*fileIn*/)
{
}
