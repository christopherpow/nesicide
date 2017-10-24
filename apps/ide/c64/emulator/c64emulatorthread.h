#ifndef C64EMULATORTHREAD_H
#define C64EMULATORTHREAD_H

#include <QThread>
#include <QTcpSocket>
#include <QProcess>
#include <QSemaphore>
#include <QMutex>

#include "ixmlserializable.h"

#include "c64_emulator_core.h"

class TcpClient : public QObject
{
   Q_OBJECT
public:
   explicit TcpClient(QString monitorIPAddress,int monitorPort,QObject *parent = 0);
   ~TcpClient();

private:
   QTcpSocket* pSocket;
   QString responseMessage;
   QStringList m_requests;
   QStringList m_responses;
   QList<int> m_expectDataInResponse;
   QList<bool> m_requestSent;
   int m_request;
   QString m_ipAddress;
   int m_port;
   QMutex*  m_clientMutex;

signals:
   void responses(QStringList requests,QStringList responses);
   void traps(QString traps);
   void clientConnected();
   void clientDisconnected();

private slots:
   void sendRequests(QStringList requests,QList<int> expectings);
   void error(QAbstractSocket::SocketError error);
   void connected();
   void disconnected();
   void readyRead();
   void bytesWritten(qint64);
};

class C64EmulatorThread : public QThread, public IXMLSerializable
{
   Q_OBJECT
public:
   C64EmulatorThread ( QObject* parent = 0 );
   virtual ~C64EmulatorThread ();
   void kill();

   // IXMLSerializable Interface Implementation
   virtual bool serialize(QDomDocument& doc, QDomNode& node);
   virtual bool deserialize(QDomDocument& doc, QDomNode& node, QString& errors);

   virtual bool serializeContent(QFile& fileOut);
   virtual bool deserializeContent(QFile& fileIn);

   void _breakpointHook();

protected:
   void timerEvent(QTimerEvent *event);

public slots:
   void breakpointsChanged();
   void viceStarted();
   void viceError(QProcess::ProcessError error);
   void viceFinished(int exitCode,QProcess::ExitStatus exitStatus);
   void primeEmulator ();
   void resetEmulator ();
   void startEmulation ();
   void pauseEmulation (bool show);
   void pauseEmulationAfter (int32_t /*frames*/) {}
   void stepCPUEmulation ();
   void stepOverCPUEmulation ();
   void stepOutCPUEmulation ();
   void processResponses(QStringList requests,QStringList responses);
   void processTraps(QString traps);

signals:
   void breakpoint();
   void emulatorConnected();
   void emulatorDisconnected();
   void emulatedFrame();
   void updateDebuggers();
   void emulatorPaused(bool show);
   void emulatorPausedAfter();
   void emulatorReset();
   void emulatorStarted();
   void debugMessage(char* message);
   void machineReady();
   void sendRequests(QStringList requests,QList<int> expectings);
   void emulatorWantsExit();

protected:
   void lockRequestQueue();
   void clearRequestQueue();
   void addToRequestQueue(QString command,int expecting);
   void runRequestQueue();
   void unlockRequestQueue();

   QProcess*   m_pViceApp;
   TcpClient*  m_pClient;
   QMutex*  m_requestMutex;

   QString     m_pFile;
   bool        m_showOnPause;

   QStringList m_requests;
   QStringList m_responses;
   QList<int>  m_responseExpected;
   QString     m_traps;

   bool m_isRunning;
};

#endif // C64EMULATORTHREAD_H
