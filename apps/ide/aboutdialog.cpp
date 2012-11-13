#include "aboutdialog.h"
#include "ui_aboutdialog.h"

#include "nes_emulator_core.h"
#include "c64_emulator_core.h"

#include <QProcess>
#include <QDir>

extern char* nesicideGetVersion();

AboutDialog::AboutDialog(QWidget* parent) :
   QDialog(parent),
   ui(new Ui::AboutDialog)
{
   ui->setupUi(this);

   QProcess            versioner;
   QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
   QString             stdioStr;
   QStringList         stdioList;
   int                 exitCode;

   // Copy the system environment to the child process.
   versioner.setProcessEnvironment(env);
   versioner.setWorkingDirectory(QCoreApplication::applicationDirPath());

   ui->versionsText->append("<a href=\"#top\"/>");
   ui->versionsText->append("<b>NESICIDE Application version:</b>");
   ui->versionsText->append(nesicideGetVersion());
   ui->versionsText->append("");

   ui->versionsText->append("<b>NES Emulator Library version:</b>");
   ui->versionsText->append(nesGetVersion());
   ui->versionsText->append("");

   ui->versionsText->append("<b>Commodore 64 (VICE Interface) Library version:</b>");
   ui->versionsText->append(c64GetVersion());
   ui->versionsText->append("");

   ui->versionsText->append("<b>Versions of external dependencies:</b>");
   ui->versionsText->append("<b>cc65:</b>");
   versioner.start("cc65 -V");
   versioner.waitForFinished();
   versioner.waitForReadyRead();
   exitCode = versioner.exitCode();
   stdioStr = QString(versioner.readAllStandardOutput());
   stdioList = stdioStr.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
   foreach ( const QString& str, stdioList )
   {
      ui->versionsText->append(str);
   }
   stdioStr = QString(versioner.readAllStandardError());
   stdioList = stdioStr.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
   foreach ( const QString& str, stdioList )
   {
      ui->versionsText->append(str);
   }
   ui->versionsText->append("");

   ui->versionsText->append("<b>ca65</b>");
   versioner.start("ca65 -V");
   versioner.waitForFinished();
   versioner.waitForReadyRead();
   exitCode = versioner.exitCode();
   stdioStr = QString(versioner.readAllStandardOutput());
   stdioList = stdioStr.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
   foreach ( const QString& str, stdioList )
   {
      ui->versionsText->append(str);
   }
   stdioStr = QString(versioner.readAllStandardError());
   stdioList = stdioStr.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
   foreach ( const QString& str, stdioList )
   {
      ui->versionsText->append(str);
   }
   ui->versionsText->append("");

   ui->versionsText->append("<b>ld65</b>");
   versioner.start("ld65 -V");
   versioner.waitForFinished();
   versioner.waitForReadyRead();
   exitCode = versioner.exitCode();
   stdioStr = QString(versioner.readAllStandardOutput());
   stdioList = stdioStr.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
   foreach ( const QString& str, stdioList )
   {
      ui->versionsText->append(str);
   }
   stdioStr = QString(versioner.readAllStandardError());
   stdioList = stdioStr.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
   foreach ( const QString& str, stdioList )
   {
      ui->versionsText->append(str);
   }
   ui->versionsText->append("");

   ui->versionsText->append("<b>make</b>");
   versioner.start("make -v");
   versioner.waitForFinished();
   versioner.waitForReadyRead();
   exitCode = versioner.exitCode();
   stdioStr = QString(versioner.readAllStandardOutput());
   stdioList = stdioStr.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
   foreach ( const QString& str, stdioList )
   {
      ui->versionsText->append(str);
   }
   stdioStr = QString(versioner.readAllStandardError());
   stdioList = stdioStr.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
   foreach ( const QString& str, stdioList )
   {
      ui->versionsText->append(str);
   }
   ui->versionsText->append("");
   ui->versionsText->scrollToAnchor("top");
}

AboutDialog::~AboutDialog()
{
   delete ui;
}
