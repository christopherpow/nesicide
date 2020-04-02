#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QUrl>

#include "nes_emulator_core.h"

#include "cobjectregistry.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

QWidget* MainWindow::_me = NULL;

MainWindow::MainWindow(QWidget* parent) :
   QMainWindow(parent),
   ui(new Ui::MainWindow)
{
   int idx;

   if ( !((QCoreApplication::applicationDirPath().contains("Program Files")) ||
        (QCoreApplication::applicationDirPath().contains("apps/nes-emulator"))) ) // Developer builds
   {
      QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::applicationDirPath());
      QSettings::setPath(QSettings::IniFormat, QSettings::SystemScope, QCoreApplication::applicationDirPath());
   }
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");

   // Set up singleton pointer for main window referencing.
   _me = this;

   ui->setupUi(this);

   m_pNESEmulatorThread = new NESEmulatorThread();
   CObjectRegistry::instance()->addObject("Emulator",m_pNESEmulatorThread);

   m_pEmulator = new NESEmulatorDockWidget();
   m_pEmulator->setVisible(true);
   ui->frame->layout()->addWidget(m_pEmulator);
   ui->frame->layout()->update();
   ui->statusBar->setVisible(false);

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

   setMinimumSize(0,ui->menuBar->sizeHint().height()+1);
   setMaximumSize(0,ui->menuBar->sizeHint().height()+1);
   ncRect = rect();
   setMinimumSize(0,0);
   setMaximumSize(0,0);

   EmulatorPrefsDialog::readSettings();

   updateFromEmulatorPrefs(true);

   QStringList sl_raw = QApplication::arguments();
   QStringList sl_nes = sl_raw.filter ( ".nes", Qt::CaseInsensitive );

   emit resetEmulator();

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

   QObject::connect(ui->menuFile,SIGNAL(aboutToShow()),this,SLOT(updateRecentFiles()));
   ui->actionRecent_Files_START->setVisible(false);
   ui->actionRecent_Files_STOP->setVisible(false);
   for ( idx = 0; idx < MAX_RECENT_FILES; idx++ )
   {
      QAction* action = new QAction("Recent File "+QString::number(idx+1));
      QObject::connect(action,SIGNAL(triggered(bool)),this,SLOT(openRecentFile()));
      ui->menuFile->insertAction(ui->actionRecent_Files_START,action);
      m_recentFileActions.append(action);
   }

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

void MainWindow::openRecentFile()
{
   QAction* action = dynamic_cast<QAction*>(sender());
   QString fileName = action->text();

   QFileInfo fileInfo(fileName);
   QDir::setCurrent(fileInfo.path());

   emit pauseEmulation(false);

   loadCartridge(fileName);

   // set up emulator if it needs to be...
   emit primeEmulator ( cartridge );

   emit resetEmulator();
   emit startEmulation();

}

void MainWindow::updateRecentFiles()
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");
   int idx;

   QStringList recentFiles = settings.value("RecentFiles").toStringList();

   for ( idx = 0; idx < recentFiles.count(); idx++ )
   {
      m_recentFileActions[idx]->setText(recentFiles[idx]);
      m_recentFileActions[idx]->setVisible(true);
   }
   for ( ; idx < MAX_RECENT_FILES; idx++ )
   {
      m_recentFileActions[idx]->setVisible(false);
   }
}

void MainWindow::saveRecentFiles(QString fileName)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");

   QStringList recentFiles = settings.value("RecentFiles").toStringList();
   recentFiles.removeAll(fileName);
   recentFiles.insert(0,fileName);
   if ( recentFiles.count() > MAX_RECENT_FILES )
   {
      recentFiles.removeLast();
   }
   settings.setValue("RecentFiles",recentFiles);
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
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");

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

      if (!m_pNESEmulatorThread->serialize(doc, doc))
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
      // Keep recent files updated.
      saveRecentFiles(fileName);

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
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");

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

void MainWindow::on_actionDendy_triggered()
{
   EmulatorPrefsDialog::setTVStandard(MODE_DENDY);
   ui->actionNTSC->setChecked(false);
   ui->actionPAL->setChecked(false);
   ui->actionDendy->setChecked(true);
   nesSetSystemMode(MODE_DENDY);

   emit resetEmulator();
   emit startEmulation();
}

void MainWindow::on_actionNTSC_triggered()
{
   EmulatorPrefsDialog::setTVStandard(MODE_NTSC);
   ui->actionNTSC->setChecked(true);
   ui->actionPAL->setChecked(false);
   ui->actionDendy->setChecked(false);
   nesSetSystemMode(MODE_NTSC);

   emit resetEmulator();
   emit startEmulation();
}

void MainWindow::on_actionPAL_triggered()
{
   EmulatorPrefsDialog::setTVStandard(MODE_PAL);
   ui->actionNTSC->setChecked(false);
   ui->actionPAL->setChecked(true);
   ui->actionDendy->setChecked(false);
   nesSetSystemMode(MODE_PAL);

   emit resetEmulator();
   emit startEmulation();
}

void MainWindow::on_actionDelta_Modulation_toggled(bool value)
{
   EmulatorPrefsDialog::setDMCEnabled(value);
   if ( value )
   {
      nesSetAudioChannelMask((EmulatorPrefsDialog::getSquare1Enabled()<<0)|
                             (EmulatorPrefsDialog::getSquare2Enabled()<<1)|
                             (EmulatorPrefsDialog::getTriangleEnabled()<<2)|
                             (EmulatorPrefsDialog::getNoiseEnabled()<<3)|
                             0x10);
   }
   else
   {
      nesSetAudioChannelMask((EmulatorPrefsDialog::getSquare1Enabled()<<0)|
                             (EmulatorPrefsDialog::getSquare2Enabled()<<1)|
                             (EmulatorPrefsDialog::getTriangleEnabled()<<2)|
                             (EmulatorPrefsDialog::getNoiseEnabled()<<3));
   }
}

void MainWindow::on_actionNoise_toggled(bool value)
{
   EmulatorPrefsDialog::setNoiseEnabled(value);
   if ( value )
   {
      nesSetAudioChannelMask((EmulatorPrefsDialog::getSquare1Enabled()<<0)|
                             (EmulatorPrefsDialog::getSquare2Enabled()<<1)|
                             (EmulatorPrefsDialog::getTriangleEnabled()<<2)|
                             0x08|
                             (EmulatorPrefsDialog::getDMCEnabled()<<4));
   }
   else
   {
      nesSetAudioChannelMask((EmulatorPrefsDialog::getSquare1Enabled()<<0)|
                             (EmulatorPrefsDialog::getSquare2Enabled()<<1)|
                             (EmulatorPrefsDialog::getTriangleEnabled()<<2)|
                             (EmulatorPrefsDialog::getDMCEnabled()<<4));
   }
}

void MainWindow::on_actionTriangle_toggled(bool value)
{
   EmulatorPrefsDialog::setTriangleEnabled(value);
   if ( value )
   {
      nesSetAudioChannelMask((EmulatorPrefsDialog::getSquare1Enabled()<<0)|
                             (EmulatorPrefsDialog::getSquare2Enabled()<<1)|
                             0x04|
                             (EmulatorPrefsDialog::getNoiseEnabled()<<3)|
                             (EmulatorPrefsDialog::getDMCEnabled()<<4));
   }
   else
   {
      nesSetAudioChannelMask((EmulatorPrefsDialog::getSquare1Enabled()<<0)|
                             (EmulatorPrefsDialog::getSquare2Enabled()<<1)|
                             (EmulatorPrefsDialog::getNoiseEnabled()<<3)|
                             (EmulatorPrefsDialog::getDMCEnabled()<<4));
   }
}

void MainWindow::on_actionSquare_2_toggled(bool value)
{
   EmulatorPrefsDialog::setSquare2Enabled(value);
   if ( value )
   {
      nesSetAudioChannelMask((EmulatorPrefsDialog::getSquare1Enabled()<<0)|
                             0x02|
                             (EmulatorPrefsDialog::getTriangleEnabled()<<2)|
                             (EmulatorPrefsDialog::getNoiseEnabled()<<3)|
                             (EmulatorPrefsDialog::getDMCEnabled()<<4));
   }
   else
   {
      nesSetAudioChannelMask((EmulatorPrefsDialog::getSquare1Enabled()<<0)|
                             (EmulatorPrefsDialog::getTriangleEnabled()<<2)|
                             (EmulatorPrefsDialog::getNoiseEnabled()<<3)|
                             (EmulatorPrefsDialog::getDMCEnabled()<<4));
   }
}

void MainWindow::on_actionSquare_1_toggled(bool value)
{
   EmulatorPrefsDialog::setSquare1Enabled(value);
   if ( value )
   {
      nesSetAudioChannelMask(0x01|
                             (EmulatorPrefsDialog::getSquare2Enabled()<<1)|
                             (EmulatorPrefsDialog::getTriangleEnabled()<<2)|
                             (EmulatorPrefsDialog::getNoiseEnabled()<<3)|
                             (EmulatorPrefsDialog::getDMCEnabled()<<4));
   }
   else
   {
      nesSetAudioChannelMask((EmulatorPrefsDialog::getSquare2Enabled()<<1)|
                             (EmulatorPrefsDialog::getTriangleEnabled()<<2)|
                             (EmulatorPrefsDialog::getNoiseEnabled()<<3)|
                             (EmulatorPrefsDialog::getDMCEnabled()<<4));
   }
}

void MainWindow::on_actionAbout_triggered()
{
   AboutDialog* dlg = new AboutDialog(this);
   dlg->exec();
   delete dlg;
}

void MainWindow::updateFromEmulatorPrefs(bool initial)
{
   if ( initial || EmulatorPrefsDialog::systemSettingsChanged() )
   {
      // Synchronize UI elements with changes.
      // Set TV standard to use.
      int systemMode = EmulatorPrefsDialog::getTVStandard();
      ui->actionNTSC->setChecked(systemMode==MODE_NTSC);
      ui->actionPAL->setChecked(systemMode==MODE_PAL);
      ui->actionDendy->setChecked(systemMode==MODE_DENDY);
      nesSetSystemMode(systemMode);
   }

   if ( initial || EmulatorPrefsDialog::audioSettingsChanged() )
   {
      bool square1 = EmulatorPrefsDialog::getSquare1Enabled();
      bool square2 = EmulatorPrefsDialog::getSquare2Enabled();
      bool triangle = EmulatorPrefsDialog::getTriangleEnabled();
      bool noise = EmulatorPrefsDialog::getNoiseEnabled();
      bool dmc = EmulatorPrefsDialog::getDMCEnabled();
      uint32_t mask = ((square1<<0)|(square2<<1)|(triangle<<2)|(noise<<3)|(dmc<<4));

      ui->actionSquare_1->setChecked(square1);
      ui->actionSquare_2->setChecked(square2);
      ui->actionTriangle->setChecked(triangle);
      ui->actionNoise->setChecked(noise);
      ui->actionDelta_Modulation->setChecked(dmc);
      nesSetAudioChannelMask(mask);

      bool pulse1VRC6 = EmulatorPrefsDialog::getPulse1VRC6Enabled();
      bool pulse2VRC6 = EmulatorPrefsDialog::getPulse2VRC6Enabled();
      bool sawtoothVRC6 = EmulatorPrefsDialog::getSawtoothVRC6Enabled();
      mask = ((pulse1VRC6<<0)|(pulse2VRC6<<1)|(sawtoothVRC6<<2));

      ui->actionPulse_1VRC6->setChecked(pulse1VRC6);
      ui->actionPulse_2VRC6->setChecked(pulse2VRC6);
      ui->actionSawtoothVRC6->setChecked(sawtoothVRC6);
      nesSetVRC6AudioChannelMask(mask);

      bool wave1N106 = EmulatorPrefsDialog::getWave1N106Enabled();
      bool wave2N106 = EmulatorPrefsDialog::getWave2N106Enabled();
      bool wave3N106 = EmulatorPrefsDialog::getWave3N106Enabled();
      bool wave4N106 = EmulatorPrefsDialog::getWave4N106Enabled();
      bool wave5N106 = EmulatorPrefsDialog::getWave5N106Enabled();
      bool wave6N106 = EmulatorPrefsDialog::getWave6N106Enabled();
      bool wave7N106 = EmulatorPrefsDialog::getWave7N106Enabled();
      bool wave8N106 = EmulatorPrefsDialog::getWave8N106Enabled();
      mask = ((wave1N106<<0)|(wave2N106<<1)|(wave3N106<<2)|(wave4N106<<3)|
              (wave5N106<<4)|(wave6N106<<5)|(wave7N106<<6)|(wave8N106<<7));

      ui->actionWave_1N106->setChecked(wave1N106);
      ui->actionWave_2N106->setChecked(wave2N106);
      ui->actionWave_3N106->setChecked(wave3N106);
      ui->actionWave_4N106->setChecked(wave4N106);
      ui->actionWave_5N106->setChecked(wave5N106);
      ui->actionWave_6N106->setChecked(wave6N106);
      ui->actionWave_7N106->setChecked(wave7N106);
      ui->actionWave_8N106->setChecked(wave8N106);
      nesSetN106AudioChannelMask(mask);
   }

   if ( initial || EmulatorPrefsDialog::videoSettingsChanged() )
   {
      switch ( EmulatorPrefsDialog::getScalingFactor() )
      {
      case 0:
         // 1x
         on_action1x_triggered();
         break;
      case 1:
         // 1.5x
         on_action1_5x_triggered();
         break;
      case 2:
         // 2x
         on_action2x_triggered();
         break;
      case 3:
         // 2.5x
         on_action2_5x_triggered();
         break;
      case 4:
         // 3x
         on_action3x_triggered();
         break;
      }
      ui->actionLinear_Interpolation->setChecked(EmulatorPrefsDialog::getLinearInterpolation());
      m_pEmulator->setLinearInterpolation(EmulatorPrefsDialog::getLinearInterpolation());
      ui->action4_3_Aspect->setChecked(EmulatorPrefsDialog::get43Aspect());
      m_pEmulator->set43Aspect(EmulatorPrefsDialog::get43Aspect());
   }

   if ( initial || EmulatorPrefsDialog::controllerSettingsChanged() )
   {
      // Set up controllers.
      nesSetControllerType(0,EmulatorPrefsDialog::getControllerType(0));
      nesSetControllerSpecial(0,EmulatorPrefsDialog::getControllerSpecial(0));
      nesSetControllerType(1,EmulatorPrefsDialog::getControllerType(1));
      nesSetControllerSpecial(1,EmulatorPrefsDialog::getControllerSpecial(1));
   }
}

void MainWindow::on_actionConfigure_triggered()
{
   EmulatorPrefsDialog dlg("nes");

   dlg.exec();

   updateFromEmulatorPrefs(false);
}

void MainWindow::on_actionFullscreen_toggled(bool value)
{
   if ( value )
   {
      m_pEmulator->setFloating(true);
      m_pEmulator->fixTitleBar();
      m_pEmulator->showFullScreen();
      m_pEmulator->setFocus();
   }
   else
   {
      m_pEmulator->showNormal();
      m_pEmulator->fixTitleBar();
      m_pEmulator->setFloating(false);
      m_pEmulator->setFocus();
   }
}

void MainWindow::on_actionAbout_Qt_triggered()
{
   QMessageBox::aboutQt(this);
}

void MainWindow::on_action1x_triggered()
{
   if ( m_pEmulator->isFullScreen() )
   {
      ui->actionFullscreen->setChecked(false);
   }
   QRect rect;
   if ( EmulatorPrefsDialog::get43Aspect() )
   {
      rect = ncRect.adjusted(0,0,1.0*292,1.0*240);
   }
   else
   {
      rect = ncRect.adjusted(0,0,1.0*256,1.0*240);
   }
   EmulatorPrefsDialog::setScalingFactor(0);
   setMinimumSize(rect.width(),rect.height());
   setMaximumSize(rect.width(),rect.height());
   resize(rect.width(),rect.height());
   ui->action1x->setChecked(true);
   ui->action1_5x->setChecked(false);
   ui->action2x->setChecked(false);
   ui->action2_5x->setChecked(false);
   ui->action3x->setChecked(false);
}

void MainWindow::on_action1_5x_triggered()
{
   if ( m_pEmulator->isFullScreen() )
   {
      ui->actionFullscreen->setChecked(false);
   }
   QRect rect;
   if ( EmulatorPrefsDialog::get43Aspect() )
   {
      rect = ncRect.adjusted(0,0,1.5*292,1.5*240);
   }
   else
   {
      rect = ncRect.adjusted(0,0,1.5*256,1.5*240);
   }
   EmulatorPrefsDialog::setScalingFactor(1);
   setMinimumSize(rect.width(),rect.height());
   setMaximumSize(rect.width(),rect.height());
   resize(rect.width(),rect.height());
   ui->action1x->setChecked(false);
   ui->action1_5x->setChecked(true);
   ui->action2x->setChecked(false);
   ui->action2_5x->setChecked(false);
   ui->action3x->setChecked(false);
}

void MainWindow::on_action2x_triggered()
{
   if ( m_pEmulator->isFullScreen() )
   {
      ui->actionFullscreen->setChecked(false);
   }
   QRect rect;
   if ( EmulatorPrefsDialog::get43Aspect() )
   {
      rect = ncRect.adjusted(0,0,2.0*292,2.0*240);
   }
   else
   {
      rect = ncRect.adjusted(0,0,2.0*256,2.0*240);
   }
   EmulatorPrefsDialog::setScalingFactor(2);
   setMinimumSize(rect.width(),rect.height());
   setMaximumSize(rect.width(),rect.height());
   resize(rect.width(),rect.height());
   ui->action1x->setChecked(false);
   ui->action1_5x->setChecked(false);
   ui->action2x->setChecked(true);
   ui->action2_5x->setChecked(false);
   ui->action3x->setChecked(false);
}

void MainWindow::on_action2_5x_triggered()
{
   if ( m_pEmulator->isFullScreen() )
   {
      ui->actionFullscreen->setChecked(false);
   }
   QRect rect;
   if ( EmulatorPrefsDialog::get43Aspect() )
   {
      rect = ncRect.adjusted(0,0,2.5*292,2.5*240);
   }
   else
   {
      rect = ncRect.adjusted(0,0,2.5*256,2.5*240);
   }
   EmulatorPrefsDialog::setScalingFactor(3);
   setMinimumSize(rect.width(),rect.height());
   setMaximumSize(rect.width(),rect.height());
   resize(rect.width(),rect.height());
   ui->action1x->setChecked(false);
   ui->action1_5x->setChecked(false);
   ui->action2x->setChecked(false);
   ui->action2_5x->setChecked(true);
   ui->action3x->setChecked(false);
}

void MainWindow::on_action3x_triggered()
{
   if ( m_pEmulator->isFullScreen() )
   {
      ui->actionFullscreen->setChecked(false);
   }
   QRect rect;
   if ( EmulatorPrefsDialog::get43Aspect() )
   {
      rect = ncRect.adjusted(0,0,3.0*292,3.0*240);
   }
   else
   {
      rect = ncRect.adjusted(0,0,3.0*256,3.0*240);
   }
   EmulatorPrefsDialog::setScalingFactor(4);
   setMinimumSize(rect.width(),rect.height());
   setMaximumSize(rect.width(),rect.height());
   resize(rect.width(),rect.height());
   ui->action1x->setChecked(false);
   ui->action1_5x->setChecked(false);
   ui->action2x->setChecked(false);
   ui->action2_5x->setChecked(false);
   ui->action3x->setChecked(true);
}

void MainWindow::on_actionLinear_Interpolation_toggled(bool )
{
   EmulatorPrefsDialog::setLinearInterpolation(ui->actionLinear_Interpolation->isChecked());
   m_pEmulator->setLinearInterpolation(ui->actionLinear_Interpolation->isChecked());
}

void MainWindow::on_action4_3_Aspect_toggled(bool )
{
   EmulatorPrefsDialog::set43Aspect(ui->action4_3_Aspect->isChecked());
   m_pEmulator->set43Aspect(ui->action4_3_Aspect->isChecked());
   switch ( EmulatorPrefsDialog::getScalingFactor() )
   {
   case 0:
      ui->action1x->trigger();
      break;
   case 1:
      ui->action1_5x->trigger();
      break;
   case 2:
      ui->action2x->trigger();
      break;
   case 3:
      ui->action2_5x->trigger();
      break;
   case 4:
      ui->action3x->trigger();
      break;
   }
}

void MainWindow::on_actionSawtoothVRC6_toggled(bool value)
{
   EmulatorPrefsDialog::setSawtoothVRC6Enabled(value);
   if ( value )
   {
      nesSetVRC6AudioChannelMask(EmulatorPrefsDialog::getPulse1VRC6Enabled()|
                                 (EmulatorPrefsDialog::getPulse2VRC6Enabled()<<1)|
                                 0x04);
   }
   else
   {
      nesSetVRC6AudioChannelMask(EmulatorPrefsDialog::getPulse1VRC6Enabled()|
                                 (EmulatorPrefsDialog::getPulse2VRC6Enabled()<<1));
   }
}

void MainWindow::on_actionPulse_2VRC6_toggled(bool value)
{
   EmulatorPrefsDialog::setPulse2VRC6Enabled(value);
   if ( value )
   {
      nesSetVRC6AudioChannelMask(EmulatorPrefsDialog::getPulse1VRC6Enabled()|
                                 0x02|
                                 (EmulatorPrefsDialog::getSawtoothVRC6Enabled()<<2));
   }
   else
   {
      nesSetVRC6AudioChannelMask(EmulatorPrefsDialog::getPulse1VRC6Enabled()|
                                 (EmulatorPrefsDialog::getSawtoothVRC6Enabled()<<2));
   }
}

void MainWindow::on_actionPulse_1VRC6_toggled(bool value)
{
   EmulatorPrefsDialog::setPulse1VRC6Enabled(value);
   if ( value )
   {
      nesSetVRC6AudioChannelMask(0x01|
                                 (EmulatorPrefsDialog::getPulse2VRC6Enabled()<<1)|
                                 (EmulatorPrefsDialog::getSawtoothVRC6Enabled()<<2));
   }
   else
   {
      nesSetVRC6AudioChannelMask((EmulatorPrefsDialog::getPulse2VRC6Enabled()<<1)|
                                 (EmulatorPrefsDialog::getSawtoothVRC6Enabled()<<2));
   }
}

void MainWindow::on_actionWave_8N106_toggled(bool value)
{
   EmulatorPrefsDialog::setWave8N106Enabled(value);
   if ( value )
   {
      nesSetN106AudioChannelMask((EmulatorPrefsDialog::getWave1N106Enabled()<<0)|
                                 (EmulatorPrefsDialog::getWave2N106Enabled()<<1)|
                                 (EmulatorPrefsDialog::getWave3N106Enabled()<<2)|
                                 (EmulatorPrefsDialog::getWave4N106Enabled()<<3)|
                                 (EmulatorPrefsDialog::getWave5N106Enabled()<<4)|
                                 (EmulatorPrefsDialog::getWave6N106Enabled()<<5)|
                                 (EmulatorPrefsDialog::getWave7N106Enabled()<<6)|
                                 0x80);
   }
   else
   {
      nesSetN106AudioChannelMask((EmulatorPrefsDialog::getWave1N106Enabled()<<0)|
                                 (EmulatorPrefsDialog::getWave2N106Enabled()<<1)|
                                 (EmulatorPrefsDialog::getWave3N106Enabled()<<2)|
                                 (EmulatorPrefsDialog::getWave4N106Enabled()<<3)|
                                 (EmulatorPrefsDialog::getWave5N106Enabled()<<4)|
                                 (EmulatorPrefsDialog::getWave6N106Enabled()<<5)|
                                 (EmulatorPrefsDialog::getWave7N106Enabled()<<6));
   }
}

void MainWindow::on_actionWave_7N106_toggled(bool value)
{
   EmulatorPrefsDialog::setWave7N106Enabled(value);
   if ( value )
   {
      nesSetN106AudioChannelMask((EmulatorPrefsDialog::getWave1N106Enabled()<<0)|
                                 (EmulatorPrefsDialog::getWave2N106Enabled()<<1)|
                                 (EmulatorPrefsDialog::getWave3N106Enabled()<<2)|
                                 (EmulatorPrefsDialog::getWave4N106Enabled()<<3)|
                                 (EmulatorPrefsDialog::getWave5N106Enabled()<<4)|
                                 (EmulatorPrefsDialog::getWave6N106Enabled()<<5)|
                                 0x40|
                                 (EmulatorPrefsDialog::getWave8N106Enabled()<<7));
   }
   else
   {
      nesSetN106AudioChannelMask((EmulatorPrefsDialog::getWave1N106Enabled()<<0)|
                                 (EmulatorPrefsDialog::getWave2N106Enabled()<<1)|
                                 (EmulatorPrefsDialog::getWave3N106Enabled()<<2)|
                                 (EmulatorPrefsDialog::getWave4N106Enabled()<<3)|
                                 (EmulatorPrefsDialog::getWave5N106Enabled()<<4)|
                                 (EmulatorPrefsDialog::getWave6N106Enabled()<<5)|
                                 (EmulatorPrefsDialog::getWave8N106Enabled()<<7));
   }
}

void MainWindow::on_actionWave_6N106_toggled(bool value)
{
   EmulatorPrefsDialog::setWave6N106Enabled(value);
   if ( value )
   {
      nesSetN106AudioChannelMask((EmulatorPrefsDialog::getWave1N106Enabled()<<0)|
                                 (EmulatorPrefsDialog::getWave2N106Enabled()<<1)|
                                 (EmulatorPrefsDialog::getWave3N106Enabled()<<2)|
                                 (EmulatorPrefsDialog::getWave4N106Enabled()<<3)|
                                 (EmulatorPrefsDialog::getWave5N106Enabled()<<4)|
                                 0x20|
                                 (EmulatorPrefsDialog::getWave7N106Enabled()<<6)|
                                 (EmulatorPrefsDialog::getWave8N106Enabled()<<7));
   }
   else
   {
      nesSetN106AudioChannelMask((EmulatorPrefsDialog::getWave1N106Enabled()<<0)|
                                 (EmulatorPrefsDialog::getWave2N106Enabled()<<1)|
                                 (EmulatorPrefsDialog::getWave3N106Enabled()<<2)|
                                 (EmulatorPrefsDialog::getWave4N106Enabled()<<3)|
                                 (EmulatorPrefsDialog::getWave5N106Enabled()<<4)|
                                 (EmulatorPrefsDialog::getWave7N106Enabled()<<6)|
                                 (EmulatorPrefsDialog::getWave8N106Enabled()<<7));
   }
}

void MainWindow::on_actionWave_5N106_toggled(bool value)
{
   EmulatorPrefsDialog::setWave5N106Enabled(value);
   if ( value )
   {
      nesSetN106AudioChannelMask((EmulatorPrefsDialog::getWave1N106Enabled()<<0)|
                                 (EmulatorPrefsDialog::getWave2N106Enabled()<<1)|
                                 (EmulatorPrefsDialog::getWave3N106Enabled()<<2)|
                                 (EmulatorPrefsDialog::getWave4N106Enabled()<<3)|
                                 0x10|
                                 (EmulatorPrefsDialog::getWave6N106Enabled()<<5)|
                                 (EmulatorPrefsDialog::getWave7N106Enabled()<<6)|
                                 (EmulatorPrefsDialog::getWave8N106Enabled()<<7));
   }
   else
   {
      nesSetN106AudioChannelMask((EmulatorPrefsDialog::getWave1N106Enabled()<<0)|
                                 (EmulatorPrefsDialog::getWave2N106Enabled()<<1)|
                                 (EmulatorPrefsDialog::getWave3N106Enabled()<<2)|
                                 (EmulatorPrefsDialog::getWave4N106Enabled()<<3)|
                                 (EmulatorPrefsDialog::getWave6N106Enabled()<<5)|
                                 (EmulatorPrefsDialog::getWave7N106Enabled()<<6)|
                                 (EmulatorPrefsDialog::getWave8N106Enabled()<<7));
   }
}

void MainWindow::on_actionWave_4N106_toggled(bool value)
{
   EmulatorPrefsDialog::setWave4N106Enabled(value);
   if ( value )
   {
      nesSetN106AudioChannelMask((EmulatorPrefsDialog::getWave1N106Enabled()<<0)|
                                 (EmulatorPrefsDialog::getWave2N106Enabled()<<1)|
                                 (EmulatorPrefsDialog::getWave3N106Enabled()<<2)|
                                 0x08|
                                 (EmulatorPrefsDialog::getWave5N106Enabled()<<4)|
                                 (EmulatorPrefsDialog::getWave6N106Enabled()<<5)|
                                 (EmulatorPrefsDialog::getWave7N106Enabled()<<6)|
                                 (EmulatorPrefsDialog::getWave8N106Enabled()<<7));
   }
   else
   {
      nesSetN106AudioChannelMask((EmulatorPrefsDialog::getWave1N106Enabled()<<0)|
                                 (EmulatorPrefsDialog::getWave2N106Enabled()<<1)|
                                 (EmulatorPrefsDialog::getWave3N106Enabled()<<2)|
                                 (EmulatorPrefsDialog::getWave5N106Enabled()<<4)|
                                 (EmulatorPrefsDialog::getWave6N106Enabled()<<5)|
                                 (EmulatorPrefsDialog::getWave7N106Enabled()<<6)|
                                 (EmulatorPrefsDialog::getWave8N106Enabled()<<7));
   }
}

void MainWindow::on_actionWave_3N106_toggled(bool value)
{
   EmulatorPrefsDialog::setWave3N106Enabled(value);
   if ( value )
   {
      nesSetN106AudioChannelMask((EmulatorPrefsDialog::getWave1N106Enabled()<<0)|
                                 (EmulatorPrefsDialog::getWave2N106Enabled()<<1)|
                                 0x04|
                                 (EmulatorPrefsDialog::getWave4N106Enabled()<<3)|
                                 (EmulatorPrefsDialog::getWave5N106Enabled()<<4)|
                                 (EmulatorPrefsDialog::getWave6N106Enabled()<<5)|
                                 (EmulatorPrefsDialog::getWave7N106Enabled()<<6)|
                                 (EmulatorPrefsDialog::getWave8N106Enabled()<<7));
   }
   else
   {
      nesSetN106AudioChannelMask((EmulatorPrefsDialog::getWave1N106Enabled()<<0)|
                                 (EmulatorPrefsDialog::getWave2N106Enabled()<<1)|
                                 (EmulatorPrefsDialog::getWave4N106Enabled()<<3)|
                                 (EmulatorPrefsDialog::getWave5N106Enabled()<<4)|
                                 (EmulatorPrefsDialog::getWave6N106Enabled()<<5)|
                                 (EmulatorPrefsDialog::getWave7N106Enabled()<<6)|
                                 (EmulatorPrefsDialog::getWave8N106Enabled()<<7));
   }
}

void MainWindow::on_actionWave_2N106_toggled(bool value)
{
   EmulatorPrefsDialog::setWave2N106Enabled(value);
   if ( value )
   {
      nesSetN106AudioChannelMask((EmulatorPrefsDialog::getWave1N106Enabled()<<0)|
                                 0x02|
                                 (EmulatorPrefsDialog::getWave3N106Enabled()<<2)|
                                 (EmulatorPrefsDialog::getWave4N106Enabled()<<3)|
                                 (EmulatorPrefsDialog::getWave5N106Enabled()<<4)|
                                 (EmulatorPrefsDialog::getWave6N106Enabled()<<5)|
                                 (EmulatorPrefsDialog::getWave7N106Enabled()<<6)|
                                 (EmulatorPrefsDialog::getWave8N106Enabled()<<7));
   }
   else
   {
      nesSetN106AudioChannelMask((EmulatorPrefsDialog::getWave1N106Enabled()<<0)|
                                 (EmulatorPrefsDialog::getWave3N106Enabled()<<2)|
                                 (EmulatorPrefsDialog::getWave4N106Enabled()<<3)|
                                 (EmulatorPrefsDialog::getWave5N106Enabled()<<4)|
                                 (EmulatorPrefsDialog::getWave6N106Enabled()<<5)|
                                 (EmulatorPrefsDialog::getWave7N106Enabled()<<6)|
                                 (EmulatorPrefsDialog::getWave8N106Enabled()<<7));
   }
}

void MainWindow::on_actionWave_1N106_toggled(bool value)
{
   EmulatorPrefsDialog::setWave1N106Enabled(value);
   if ( value )
   {
      nesSetN106AudioChannelMask(0x01|
                                 (EmulatorPrefsDialog::getWave2N106Enabled()<<1)|
                                 (EmulatorPrefsDialog::getWave3N106Enabled()<<2)|
                                 (EmulatorPrefsDialog::getWave4N106Enabled()<<3)|
                                 (EmulatorPrefsDialog::getWave5N106Enabled()<<4)|
                                 (EmulatorPrefsDialog::getWave6N106Enabled()<<5)|
                                 (EmulatorPrefsDialog::getWave7N106Enabled()<<6)|
                                 (EmulatorPrefsDialog::getWave8N106Enabled()<<7));
   }
   else
   {
      nesSetN106AudioChannelMask((EmulatorPrefsDialog::getWave2N106Enabled()<<1)|
                                 (EmulatorPrefsDialog::getWave3N106Enabled()<<2)|
                                 (EmulatorPrefsDialog::getWave4N106Enabled()<<3)|
                                 (EmulatorPrefsDialog::getWave5N106Enabled()<<4)|
                                 (EmulatorPrefsDialog::getWave6N106Enabled()<<5)|
                                 (EmulatorPrefsDialog::getWave7N106Enabled()<<6)|
                                 (EmulatorPrefsDialog::getWave8N106Enabled()<<7));
   }
}
