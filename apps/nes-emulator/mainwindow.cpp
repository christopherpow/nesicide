#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QUrl>

#include "main.h"

#include "nes_emulator_core.h"

#include "cthreadregistry.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

QWidget* MainWindow::_me = NULL;

MainWindow::MainWindow(QWidget* parent) :
   QMainWindow(parent),
   ui(new Ui::MainWindow)
{
   QSettings settings;

   // Set up singleton pointer for main window referencing.
   _me = this;

   ui->setupUi(this);

   m_pNESEmulatorThread = new NESEmulatorThread();
   CThreadRegistry::addThread("Emulator",m_pNESEmulatorThread);

   m_pEmulator = new NESEmulatorDockWidget(this);
   ui->frame->layout()->addWidget(m_pEmulator);
   m_pEmulator->setVisible(true);

   m_pEmulatorControl = new EmulatorControl();

   // Connect emulator signals.
   QObject::connect(this,SIGNAL(startEmulation()),m_pNESEmulatorThread,SLOT(startEmulation()));
   QObject::connect(this,SIGNAL(pauseEmulation(bool)),m_pNESEmulatorThread,SLOT(pauseEmulation(bool)));
   QObject::connect(this,SIGNAL(resetEmulator()),m_pNESEmulatorThread,SLOT(resetEmulator()));
   QObject::connect(this,SIGNAL(primeEmulator(CCartridge*)),m_pNESEmulatorThread,SLOT(primeEmulator(CCartridge*)));

   // Add menu for emulator control.  The emulator control provides menu for itself!  =]
   QAction* firstEmuMenuAction = ui->menuEmulator->actions().at(0);
   ui->menuEmulator->insertActions(firstEmuMenuAction,m_pEmulatorControl->menu());
   ui->menuEmulator->insertSeparator(firstEmuMenuAction);

   EmulatorPrefsDialog::readSettings();

   // Set TV standard to use.
   int systemMode = EmulatorPrefsDialog::getTVStandard();
   ui->actionNTSC->setChecked(systemMode==MODE_NTSC);
   ui->actionPAL->setChecked(systemMode==MODE_PAL);
   nesSetSystemMode(systemMode);

   // Set up controllers.
   nesSetControllerType(0,EmulatorPrefsDialog::getControllerType(0));
   nesSetControllerSpecial(0,EmulatorPrefsDialog::getControllerSpecial(0));
   nesSetControllerType(1,EmulatorPrefsDialog::getControllerType(1));
   nesSetControllerSpecial(1,EmulatorPrefsDialog::getControllerSpecial(1));

   // Set sound channel enables.
   bool square1 = EmulatorPrefsDialog::getSquare1Enabled();
   bool square2 = EmulatorPrefsDialog::getSquare2Enabled();
   bool triangle = EmulatorPrefsDialog::getTriangleEnabled();
   bool noise = EmulatorPrefsDialog::getNoiseEnabled();
   bool dmc = EmulatorPrefsDialog::getDMCEnabled();
   int mask = ((square1<<0)|(square2<<1)|(triangle<<2)|(noise<<3)|(dmc<<4));

   ui->actionSquare_1->setChecked(square1);
   ui->actionSquare_2->setChecked(square2);
   ui->actionTriangle->setChecked(triangle);
   ui->actionNoise->setChecked(noise);
   ui->actionDelta_Modulation->setChecked(dmc);
   ui->actionMute_All->setChecked(!mask);

   QStringList sl_raw = QApplication::arguments();
   QStringList sl_nes = sl_raw.filter ( ".nes", Qt::CaseInsensitive );

   if ( sl_nes.count() >= 1 )
   {
      emit pauseEmulation(false);

      loadCartridge(sl_nes.at(0));

      // set up emulator if it needs to be...
      emit primeEmulator ( cartridge );

      emit resetEmulator();
      emit startEmulation();

      if ( sl_nes.count() > 1 )
      {
         QMessageBox::information ( 0, "Command Line Error", "Too many NES ROM files were specified on the command\n"
                                    "line.  Only the first NES ROM was opened, all others\n"
                                    "were ignored." );
      }
   }

   emit resetEmulator();

   if ( settings.value("Environment/rememberWindowSettings").toBool() )
   {
      restoreGeometry(settings.value("EmulatorGeometry").toByteArray());
      restoreState(settings.value("EmulatorState").toByteArray());
   }
}

MainWindow::~MainWindow()
{
   delete ui;
}

void MainWindow::applicationActivationChanged(bool activated)
{
   if ( activated )
   {
      // Nothing to do...
   }
   else
   {
      if ( EmulatorPrefsDialog::getPauseOnTaskSwitch() )
      {
         emit pauseEmulation(false);
      }
   }
}

void MainWindow::on_actionExit_triggered()
{
   close();
}

void MainWindow::closeEvent ( QCloseEvent* event )
{
   QSettings settings;

   settings.setValue("EmulatorGeometry",saveGeometry());
   settings.setValue("EmulatorState",saveState());

   emit pauseEmulation(false);

   // Now save the emulator state if a save state file is specified.
   if ( (cartridge) && (!cartridge->getSaveStateFile().isEmpty()) )
   {
      saveEmulatorState(cartridge->getSaveStateFile());
   }

   QMainWindow::closeEvent(event);
}

void MainWindow::saveEmulatorState(QString fileName)
{
   // Only save the emulator state if the SRAM is dirty.
   if ( nesIsSRAMDirty() )
   {
      QFile file(fileName);

      if ( !file.open( QFile::WriteOnly) )
      {
         QMessageBox::critical(this, "Error", "An error occured while trying to open the save state file for writing.");
         return;
      }

#if defined(XML_SAVE_STATE)
      QDomDocument doc;
      QDomProcessingInstruction instr = doc.createProcessingInstruction("xml", "version='1.0' encoding='UTF-8'");
      doc.appendChild(instr);

      if (!emulator->serialize(doc, doc))
      {
         QMessageBox::critical(this, "Error", "An error occured while trying to serialize the save state data.");
         file.close();
      }

      // Create a text stream so we can stream the XML data to the file easily.
      QTextStream ts( &file );

      // Use the standard C++ stream function for streaming the string representation of our XML to
      // our file stream.
      ts << doc.toString();
#else
      if (!m_pNESEmulatorThread->serializeContent(file))
      {
         QMessageBox::critical(this, "Error", "An error occured while trying to serialize the save state data.");
         file.close();
      }
#endif

      // And finally close the file.
      file.close();
   }
}

void MainWindow::loadCartridge ( QString fileName )
{
   QString str;

   // Make sure our pointers are in order..
   if (!cartridge)
   {
      cartridge = new CCartridge();
   }

   QFile fileIn(fileName);
   QFileInfo fileInfo(fileName);

   if (fileIn.exists() && fileIn.open(QIODevice::ReadOnly))
   {
      QDataStream fs(&fileIn);

      // Check the NES header
      char nesHeader[4] = {'N', 'E', 'S', 0x1A};
      char nesTest[4] = {0, 0, 0, 0};
      fs.readRawData(nesTest,4);

      if (memcmp(nesHeader, nesTest,4))
      {
         // Header check failed, quit
         fileIn.close();
         QMessageBox::information(0, "Error", "Invalid ROM format.\nCannot create project.");
         return;
      }

      // Number of 16 KB PRG-ROM banks
      qint8 numPrgRomBanks;
      fs >> numPrgRomBanks;

      // Convert to 8 KB banks
      numPrgRomBanks <<= 1;

      // Get the number of 8 KB CHR-ROM / VROM banks
      qint8 numChrRomBanks;
      fs >> numChrRomBanks;

      // ROM Control Byte 1:
      // - Bit 0 - Indicates the type of mirroring used by the game
      //   where 0 indicates horizontal mirroring, 1 indicates
      //   vertical mirroring.
      //
      // - Bit 1 - Indicates the presence of battery-backed RAM at
      //   memory locations $6000-$7FFF.
      //
      // - Bit 2 - Indicates the presence of a 512-byte trainer at
      //   memory locations $7000-$71FF.
      //
      // - Bit 3 - If this bit is set it overrides bit 0 to indicate fourscreen
      //   mirroring should be used.
      //
      // - Bits 4-7 - Four lower bits of the mapper number.
      qint8 romCB1;
      fs >> romCB1;

      // First extract the mirror mode
      if ((romCB1&FLAG_MIRROR) == FLAG_MIRROR_VERT)
      {
         cartridge->setMirrorMode(VerticalMirroring);
      }
      else
      {
         cartridge->setMirrorMode(HorizontalMirroring);
      }

      // Now extract the two flags (battery backed ram and trainer)
      cartridge->setBatteryBackedRam(romCB1 & 0x02);
      bool hasTrainer = (romCB1 & 0x04);

      // ROM Control Byte 2:
      //  Bits 0-3 - Reserved for future usage and should all be 0.
      //  Bits 4-7 - Four upper bits of the mapper number.
      qint8 romCB2;
      fs >> romCB2;

      if ( romCB2&0x0F )
      {
         romCB2 = 0x00;
         QMessageBox::information(0, "Warning", "Invalid iNES header format.\nSave the project to fix.");
      }

      // Extract the upper four bits of the mapper number
      cartridge->setMapperNumber(((romCB1>>4)&0x0F)|(romCB2&0xF0));

      // Number of 8 KB RAM banks. For compatibility with previous
      // versions of the iNES format, assume 1 page of RAM when
      // this is 0.
      qint8 numRamBanks;
      fs >> numRamBanks;

      if (numRamBanks == 0)
      {
         numRamBanks = 1;
      }

      // Skip the 7 reserved bytes
      qint8 skip;

      for (int i=0; i<7; i++)
      {
         fs >> skip;
      }

      // Extract the trainer (if it exists)
      if (hasTrainer)
      {
         // TODO: Handle trainer. Skipping for now.
         for (int i=0; i<512; i++)
         {
            fs >> skip;
         }
      }

      // Load the PRG-ROM banks (16KB each)
      for (int bank=0; bank<numPrgRomBanks; bank++)
      {
         // Load in the binary data
         fs.readRawData(cartridge->getPointerToPrgRomBank(bank),MEM_8KB);
      }

      cartridge->setNumPrgRomBanks(numPrgRomBanks);

      // Load the CHR-ROM banks (8KB each)
      for (int bank=0; bank<numChrRomBanks; bank++)
      {
         // Load in the binary data
         fs.readRawData(cartridge->getPointerToChrRomBank(bank),MEM_8KB);
      }

      cartridge->setNumChrRomBanks(numChrRomBanks);

      cartridge->setSaveStateFile(fileInfo.completeBaseName()+".sav");

      fileIn.close();
   }
}

void MainWindow::on_actionOpen_triggered()
{
   QSettings settings;

   QString romPath = settings.value("Environment/romPath","").toString();
   QString fileName = QFileDialog::getOpenFileName(this, "Open ROM", romPath, "iNES ROM (*.nes)");

   if (fileName.isEmpty())
   {
      return;
   }

   QFileInfo fileInfo(fileName);
   QDir::setCurrent(fileInfo.path());

   emit pauseEmulation(false);

   loadCartridge(fileName);

   // set up emulator if it needs to be...
   emit primeEmulator ( cartridge );

   emit resetEmulator();
   emit startEmulation();
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
    QList<QUrl> fileUrls;
    QString     fileName;

    if ( event->mimeData()->hasUrls() )
    {
       fileUrls = event->mimeData()->urls();

       fileName = fileUrls.at(0).toLocalFile();

       if ( fileName.contains(".nes",Qt::CaseInsensitive) )
       {
          event->acceptProposedAction();
       }
    }
}

void MainWindow::dragMoveEvent(QDragMoveEvent* event)
{
    QList<QUrl> fileUrls;
    QString     fileName;

    if ( event->mimeData()->hasUrls() )
    {
       fileUrls = event->mimeData()->urls();

       fileName = fileUrls.at(0).toLocalFile();

       if ( fileName.contains(".nes",Qt::CaseInsensitive) )
       {
          event->acceptProposedAction();
       }
    }
}

void MainWindow::dropEvent(QDropEvent* event)
{
   QList<QUrl> fileUrls;
   QString     fileName;

   if ( event->mimeData()->hasUrls() )
   {
      fileUrls = event->mimeData()->urls();

      fileName = fileUrls.at(0).toLocalFile();

      if ( fileName.contains(".nes",Qt::CaseInsensitive) )
      {
         QFileInfo fileInfo(fileName);
         QDir::setCurrent(fileInfo.path());

         emit pauseEmulation(false);

         loadCartridge(fileName);

         // set up emulator if it needs to be...
         emit primeEmulator ( cartridge );

         emit resetEmulator();
         emit startEmulation();

         event->acceptProposedAction();
      }
   }
}

void MainWindow::on_actionNTSC_triggered()
{
   EmulatorPrefsDialog::setTVStandard(MODE_NTSC);
   ui->actionNTSC->setChecked(true);
   ui->actionPAL->setChecked(false);
   nesSetSystemMode(MODE_NTSC);

   emit resetEmulator();
   emit startEmulation();
}

void MainWindow::on_actionPAL_triggered()
{
   EmulatorPrefsDialog::setTVStandard(MODE_PAL);
   ui->actionNTSC->setChecked(false);
   ui->actionPAL->setChecked(true);
   nesSetSystemMode(MODE_PAL);

   emit resetEmulator();
   emit startEmulation();
}

void MainWindow::on_actionDelta_Modulation_toggled(bool value)
{
   EmulatorPrefsDialog::setDMCEnabled(value);
   if ( value )
   {
      ui->actionMute_All->setChecked(false);
      nesSetAudioChannelMask(nesGetAudioChannelMask()|0x10);
   }
   else
   {
      nesSetAudioChannelMask(nesGetAudioChannelMask()&(~0x10));
   }
}

void MainWindow::on_actionNoise_toggled(bool value)
{
   EmulatorPrefsDialog::setNoiseEnabled(value);
   if ( value )
   {
      ui->actionMute_All->setChecked(false);
      nesSetAudioChannelMask(nesGetAudioChannelMask()|0x08);
   }
   else
   {
      nesSetAudioChannelMask(nesGetAudioChannelMask()&(~0x08));
   }
}

void MainWindow::on_actionTriangle_toggled(bool value)
{
   EmulatorPrefsDialog::setTriangleEnabled(value);
   if ( value )
   {
      ui->actionMute_All->setChecked(false);
      nesSetAudioChannelMask(nesGetAudioChannelMask()|0x04);
   }
   else
   {
      nesSetAudioChannelMask(nesGetAudioChannelMask()&(~0x04));
   }
}

void MainWindow::on_actionSquare_2_toggled(bool value)
{
   EmulatorPrefsDialog::setSquare2Enabled(value);
   if ( value )
   {
      ui->actionMute_All->setChecked(false);
      nesSetAudioChannelMask(nesGetAudioChannelMask()|0x02);
   }
   else
   {
      nesSetAudioChannelMask(nesGetAudioChannelMask()&(~0x02));
   }
}

void MainWindow::on_actionSquare_1_toggled(bool value)
{
   EmulatorPrefsDialog::setSquare1Enabled(value);
   if ( value )
   {
      ui->actionMute_All->setChecked(false);
      nesSetAudioChannelMask(nesGetAudioChannelMask()|0x01);
   }
   else
   {
      nesSetAudioChannelMask(nesGetAudioChannelMask()&(~0x01));
   }
}

void MainWindow::on_actionMute_All_toggled(bool value)
{
   EmulatorPrefsDialog::setSquare1Enabled(!value);
   EmulatorPrefsDialog::setSquare2Enabled(!value);
   EmulatorPrefsDialog::setTriangleEnabled(!value);
   EmulatorPrefsDialog::setNoiseEnabled(!value);
   EmulatorPrefsDialog::setDMCEnabled(!value);
   ui->actionSquare_1->setChecked(!value);
   ui->actionSquare_2->setChecked(!value);
   ui->actionTriangle->setChecked(!value);
   ui->actionNoise->setChecked(!value);
   ui->actionDelta_Modulation->setChecked(!value);

   if ( value )
   {
      nesSetAudioChannelMask(nesGetAudioChannelMask()&(~0x1F));
   }
   else
   {
      nesSetAudioChannelMask(nesGetAudioChannelMask()|0x1F);
   }
}

void MainWindow::on_actionAbout_triggered()
{
   AboutDialog* dlg = new AboutDialog(this);
   dlg->exec();
   dlg->deleteLater();
}

void MainWindow::on_actionPreferences_triggered()
{
   EmulatorPrefsDialog dlg;

   dlg.exec();

   // Synchronize UI elements with changes.
   // Set TV standard to use.
   int systemMode = EmulatorPrefsDialog::getTVStandard();
   ui->actionNTSC->setChecked(systemMode==MODE_NTSC);
   ui->actionPAL->setChecked(systemMode==MODE_PAL);
   nesSetSystemMode(systemMode);

   bool square1 = EmulatorPrefsDialog::getSquare1Enabled();
   bool square2 = EmulatorPrefsDialog::getSquare2Enabled();
   bool triangle = EmulatorPrefsDialog::getTriangleEnabled();
   bool noise = EmulatorPrefsDialog::getNoiseEnabled();
   bool dmc = EmulatorPrefsDialog::getDMCEnabled();
   int mask = ((square1<<0)|(square2<<1)|(triangle<<2)|(noise<<3)|(dmc<<4));

   if ( !(square1|square2|triangle|noise|dmc) )
   {
      ui->actionMute_All->setChecked(true);
      nesSetAudioChannelMask(nesGetAudioChannelMask()&(~0x1F));
   }
   else
   {
      ui->actionMute_All->setChecked(false);
      nesSetAudioChannelMask(nesGetAudioChannelMask()|mask);
   }
   ui->actionSquare_1->setChecked(square1);
   ui->actionSquare_2->setChecked(square2);
   ui->actionTriangle->setChecked(triangle);
   ui->actionNoise->setChecked(noise);
   ui->actionDelta_Modulation->setChecked(dmc);

   if ( EmulatorPrefsDialog::videoSettingsChanged() )
   {
      resize((EmulatorPrefsDialog::getScalingFactor()*256),(EmulatorPrefsDialog::getScalingFactor()*240)+41);
   }

   if ( EmulatorPrefsDialog::controllerSettingsChanged() )
   {
      // Set up controllers.
      nesSetControllerType(0,EmulatorPrefsDialog::getControllerType(0));
      nesSetControllerSpecial(0,EmulatorPrefsDialog::getControllerSpecial(0));
      nesSetControllerType(1,EmulatorPrefsDialog::getControllerType(1));
      nesSetControllerSpecial(1,EmulatorPrefsDialog::getControllerSpecial(1));
   }
}

void MainWindow::on_actionFullscren_toggled(bool value)
{
   if ( value )
   {
      m_bEmulatorFloating = m_pEmulator->isFloating();
      m_pEmulator->setFloating(true);
      m_pEmulator->showFullScreen();
   }
   else
   {
      m_pEmulator->showNormal();
      m_pEmulator->setFloating(m_bEmulatorFloating);
   }
}

void MainWindow::on_actionAbout_Qt_triggered()
{
   QMessageBox::aboutQt(this);
}
