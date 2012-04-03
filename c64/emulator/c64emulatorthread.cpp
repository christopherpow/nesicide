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
#include <QRegExp>

#include <cdockwidgetregistry.h>

#include "c64emulatorthread.h"

#include "ccc65interface.h"

#include "cthreadregistry.h"
#include "main.h"

QSemaphore c64BreakpointSemaphore(0);

QSemaphore*  m_requestSemaphore;
QSemaphore*  m_clientSemaphore;

static void breakpointHook ( void )
{
   BreakpointWatcherThread* breakpointWatcher = dynamic_cast<BreakpointWatcherThread*>(CThreadRegistry::getThread("Breakpoint Watcher"));
   breakpointWatcher->breakpointWatcherSemaphore()->release();
   c64BreakpointSemaphore.acquire();
}

C64EmulatorThread::C64EmulatorThread(QObject*)
{
   m_pViceApp = new QProcess(this);
   m_pViceApp->start("C:\\WinVICE\\x64sc.exe -remotemonitor");
   m_pViceApp->waitForStarted();

   m_pClient = new TcpClient();
   m_pClient->moveToThread(this);

   QObject::connect(m_pClient,SIGNAL(responses(QStringList,QStringList)),this,SLOT(processResponses(QStringList,QStringList)));
   QObject::connect(this,SIGNAL(sendRequests(QStringList,QList<int>)),m_pClient,SLOT(sendRequests(QStringList,QList<int>)));
   QObject::connect(m_pClient,SIGNAL(traps(QString)),this,SLOT(processTraps(QString)));

   // Enable breakpoint callbacks from the external emulator library.
   c64SetBreakpointHook(breakpointHook);

   m_requestSemaphore = new QSemaphore(1);
   m_clientSemaphore = new QSemaphore(1);

   // Start the thread.
   start();
}

C64EmulatorThread::~C64EmulatorThread()
{
   m_pViceApp->kill();
   m_pViceApp->deleteLater();
}

void C64EmulatorThread::kill()
{
   lockRequestQueue();
   clearRequestQueue();
   addToRequestQueue("until $a474",false); // using "exit" doesn't seem to work.
   runRequestQueue();

   // Get outta here!
   exit();

   // Force hard-reset of the machine...
   c64EnableBreakpoints(false);

   c64BreakpointSemaphore.release();
}

void C64EmulatorThread::primeEmulator()
{
}

void C64EmulatorThread::resetEmulator()
{
   // Force hard-reset of the machine...
   c64EnableBreakpoints(false);

   // If during the last run we were stopped at a breakpoint, clear it...
   if ( !(c64BreakpointSemaphore.available()) )
   {
      c64BreakpointSemaphore.release();
   }

   if ( nesicideProject->isInitialized() )
   {
      QDir dirProject(nesicideProject->getProjectLinkerOutputBasePath());
      QDir dirVICE("C:\\WinVICE");
      QString fileName = QDir::toNativeSeparators(dirProject.absoluteFilePath(nesicideProject->getProjectLinkerOutputName()));
      QString request;
      int addr;

      addr = CCC65Interface::getSegmentBase("STARTUP");

      lockRequestQueue();
      clearRequestQueue();
      addToRequestQueue("until $a474",false);
      addToRequestQueue("reset",true);
      if ( addr > 0 )
      {
         request = "load \"" + fileName + "\" 0";
         addToRequestQueue(request.toAscii(),true);
         request = "r pc = $" + QString::number(addr,16);
         addToRequestQueue(request,false);
//         addToRequestQueue("r",true);
//         addToRequestQueue("io",true);
//         addToRequestQueue("m $0 $cfff",true);
      }
      runRequestQueue();
   }
}

void C64EmulatorThread::startEmulation ()
{
   // If during the last run we were stopped at a breakpoint, clear it...
   if ( !(c64BreakpointSemaphore.available()) )
   {
      c64BreakpointSemaphore.release();
   }

   lockRequestQueue();
   clearRequestQueue();
   addToRequestQueue("until $a474",false); // using "exit" doesn't seem to work.
   runRequestQueue();
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
   absAddr = c64GetAbsoluteAddressFromAddress(addr);
   endAddr = CCC65Interface::getEndAddressFromAbsoluteAddress(addr,absAddr);

   QString dbg;
   dbg.append("STEP:");
   dbg.append(QString::number(addr,16));
   dbg.append(","+QString::number(absAddr,16));
   dbg.append(","+QString::number(endAddr,16));
   qDebug(dbg.toAscii().constData());

#if 0
// VICE doesn't give us what we need to step a full C language line yet.
   if ( endAddr != 0xFFFFFFFF )
   {
      c64SetGotoAddress(endAddr);

      // If during the last run we were stopped at a breakpoint, clear it...
      if ( !(c64BreakpointSemaphore.available()) )
      {
         c64BreakpointSemaphore.release();
      }

      lockRequestQueue();
      clearRequestQueue();
      request = "break load $" + QString::number(endAddr,16);
      addToRequestQueue(request,true);
      addToRequestQueue("r",true);
//      addToRequestQueue("io",true);
      addToRequestQueue("m $0 $cfff",true);
      runRequestQueue();
   }
   else
#endif
   {
      // If during the last run we were stopped at a breakpoint, clear it...
      // But ensure we come right back...
      if ( !(c64BreakpointSemaphore.available()) )
      {
         c64BreakpointSemaphore.release();
      }

      lockRequestQueue();
      clearRequestQueue();
      addToRequestQueue("step",true);
      addToRequestQueue("r",true);
//      addToRequestQueue("io",true);
      addToRequestQueue("m $0 $cfff",true);
      runRequestQueue();
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
   absAddr = c64GetAbsoluteAddressFromAddress(addr);
   endAddr = CCC65Interface::getEndAddressFromAbsoluteAddress(addr,absAddr);

   if ( endAddr != 0xFFFFFFFF )
   {
      // If the line has enough of an assembly-stream associated with it...
      if ( endAddr-addr >= 2 )
      {
         // Check if last instruction on line is JSR...
         // This is fairly typical of if conditions with function calls on the same line.
         instr = c64GetMemory(endAddr-2);
         instAbsAddr = c64GetAbsoluteAddressFromAddress(endAddr-2);
         isInstr = CCC65Interface::isAbsoluteAddressAnOpcode(instAbsAddr);
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

      // If during the last run we were stopped at a breakpoint, clear it...
      if ( !(c64BreakpointSemaphore.available()) )
      {
         c64BreakpointSemaphore.release();
      }

      lockRequestQueue();
      clearRequestQueue();
      request = "until $" + QString::number(endAddr+1,16);
      addToRequestQueue(request,false);
      addToRequestQueue(".",true);
      addToRequestQueue("r",true);
//      addToRequestQueue("io",true);
      addToRequestQueue("m $0 $cfff",true);
      runRequestQueue();
   }
   else
   {
      stepCPUEmulation();
   }
}

void C64EmulatorThread::stepOutCPUEmulation ()
{
   // If during the last run we were stopped at a breakpoint, clear it...
   if ( !(c64BreakpointSemaphore.available()) )
   {
      c64BreakpointSemaphore.release();
   }

   lockRequestQueue();
   clearRequestQueue();
   addToRequestQueue("next",true);
   addToRequestQueue("r",true);
//   addToRequestQueue("io",true);
   addToRequestQueue("m $0 $cfff",true);
   runRequestQueue();
}

void C64EmulatorThread::pauseEmulation (bool show)
{
   m_showOnPause = show;

   lockRequestQueue();
   clearRequestQueue();
   addToRequestQueue("r",true);
//   addToRequestQueue("io",true);
   addToRequestQueue("m $0 $cfff",true);
   runRequestQueue();
}

void C64EmulatorThread::loadFile()
{
   // Trigger inspector updates...
   c64Disassemble();
   emit updateDebuggers();

   // Trigger UI updates...
   emit machineReady();
}

void C64EmulatorThread::clearRequestQueue()
{
   m_requests.clear();
   m_responses.clear();
   m_responseExpected.clear();
}

void C64EmulatorThread::addToRequestQueue(QString request,bool expecting)
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

      emit sendRequests(m_requests,m_responseExpected);
   }
   else
   {
      unlockRequestQueue();
   }
}

void C64EmulatorThread::processTraps(QString traps)
{
   qDebug("TRAP:");
   qDebug(traps.toAscii().constData());
   pauseEmulation(true);
}

void C64EmulatorThread::processResponses(QStringList requests,QStringList responses)
{
   qDebug("processResponses");
   qDebug(requests.at(0).toAscii().constData());
   qDebug(QString::number(requests.count()).toAscii().constData());
   qDebug(QString::number(responses.count()).toAscii().constData());
   int resp;
   int32_t a;

#if 0
   for ( int i = 0; i < requests.count(); i++ )
   {
      QString str;
      if ( requests.at(i) != "END" )
      {
         str = QString::number(i);
         str += " Request: \n";
         str += requests.at(i);
         str += "\nResponse: \n";
         str += QString::number(responses.at(i).length());
         qDebug(str.toAscii().constData());
      }
   }
#endif
   for ( resp = 0; resp < responses.count(); resp++ )
   {
      // Check if we need to update debuggers.
      if ( requests.at(resp).startsWith("reset") )
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
            if ( CCC65Interface::isAbsoluteAddressAnOpcode(a) )
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

//         qDebug(responseHeader.toAscii().constData());
         responseHeader.remove(QRegExp("^([ \r\n\t]*[(]C:[$][0-9a-f]+[)])*"));
//         qDebug(responseData.toAscii().constData());
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

         // emit done in io dump below.
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

         emit emulatorPaused(m_showOnPause);
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
   m_requestSemaphore->acquire();
}

void C64EmulatorThread::unlockRequestQueue()
{
   m_requestSemaphore->release();
}

TcpClient::TcpClient(QObject *parent)
{
   pSocket = new QTcpSocket(this);
   QObject::connect(pSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(error(QAbstractSocket::SocketError)));
   QObject::connect(pSocket,SIGNAL(connected()),this,SLOT(connected()));
   QObject::connect(pSocket,SIGNAL(readyRead()),this,SLOT(readyRead()));
   QObject::connect(pSocket,SIGNAL(bytesWritten(qint64)),this,SLOT(bytesWritten(qint64)));
   pSocket->connectToHost("127.0.0.1",6510);

   m_request = -1;
}

TcpClient::~TcpClient()
{
   pSocket->close();
   delete pSocket;
}

void TcpClient::error(QAbstractSocket::SocketError)
{
   qDebug("SOCKET ERROR!");
}

void TcpClient::connected()
{
   qDebug("SOCKET CONNECTED!");
}

void TcpClient::sendRequests(QStringList requests,QList<int> expectings)
{
   m_clientSemaphore->acquire();
   responseMessage.clear();
   m_request = 0;
   m_requests.clear();
   m_requests.append(requests);
//   qDebug(m_requests.at(m_request).toAscii().constData());
   m_expectDataInResponse = expectings;
   m_responses.clear();
   pSocket->readAll();
   pSocket->write(m_requests.at(m_request).toAscii());
}

void TcpClient::readyRead()
{
   QRegExp regex("([(]C:[$][0-9a-f]+[)]) ");

   responseMessage.append(pSocket->readAll());

   if ( m_request >= (m_requests.count()-1) )
   {
      emit traps(responseMessage);
      return;
   }

   if ( (!m_expectDataInResponse.at(m_request)) || (responseMessage.lastIndexOf(regex) > 0) )
   {
      m_responses.append(responseMessage);
      m_request++;
      if ( m_requests.at(m_request) != "END" )
      {
         responseMessage.clear();
//         qDebug(m_requests.at(m_request).toAscii().constData());
         if ( m_requests.at(m_request) != ".\n" )
         {
            pSocket->write(m_requests.at(m_request).toAscii());
         }
      }
      else
      {
//         qDebug("FINISHED");
         emit responses(m_requests,m_responses);
         m_clientSemaphore->release();
         m_requestSemaphore->release();
      }
   }
}

void TcpClient::bytesWritten(qint64 bytes)
{
//   qDebug("bytesWritten");
//   qDebug(QString::number(bytes).toAscii().constData());
}

bool C64EmulatorThread::serialize(QDomDocument& doc, QDomNode& node)
{
   return true;
}

bool C64EmulatorThread::serializeContent(QFile& fileOut)
{
}

bool C64EmulatorThread::deserialize(QDomDocument& doc, QDomNode& node, QString& errors)
{
   return true;
}

bool C64EmulatorThread::deserializeContent(QFile& fileIn)
{
}
