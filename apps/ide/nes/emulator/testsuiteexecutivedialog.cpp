#include "testsuiteexecutivedialog.h"
#include "ui_testsuiteexecutivedialog.h"

#include "main.h"

#include "nes_emulator_core.h"
#include "cjoypadlogger.h"
#include "cobjectregistry.h"

TestSuiteExecutiveDialog::TestSuiteExecutiveDialog(QWidget *parent) :
   QDialog(parent),
   ui(new Ui::TestSuiteExecutiveDialog)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");

   ui->setupUi(this);
   ui->suiteProgress->setMaximum(1);
   ui->suiteProgress->setValue(0);
   ui->testProgress->setMaximum(1);
   ui->testProgress->setValue(0);
   ui->passRate->setMaximum(100);
   ui->passRate->setValue(0);

   aborted = false;

   loadTestSuite(settings.value("TestSuiteFile").toString());
}

TestSuiteExecutiveDialog::~TestSuiteExecutiveDialog()
{
   delete ui;
}

void TestSuiteExecutiveDialog::showEvent(QShowEvent *)
{
   QObject* emulator = CObjectRegistry::getInstance()->getObject("Emulator");

   if ( emulator )
   {
      QObject::connect(emulator,SIGNAL(machineReady()),this,SLOT(machineReady()));
      QObject::connect(emulator,SIGNAL(emulatedFrame()),this,SLOT(updateProgress()));
      QObject::connect(this,SIGNAL(startEmulation()),emulator,SLOT(startEmulation()));
      QObject::connect(this,SIGNAL(pauseEmulationAfter(int32_t)),emulator,SLOT(pauseEmulationAfter(int32_t)));
      QObject::connect(emulator,SIGNAL(emulatorPausedAfter()),this,SLOT(emulatorPausedAfter()));
   }
}

void TestSuiteExecutiveDialog::hideEvent(QHideEvent *)
{
   QObject* emulator = CObjectRegistry::getInstance()->getObject("Emulator");

   if ( emulator )
   {
      QObject::disconnect(emulator,SIGNAL(machineReady()),this,SLOT(machineReady()));
      QObject::disconnect(emulator,SIGNAL(emulatedFrame()),this,SLOT(updateProgress()));
      QObject::disconnect(this,SIGNAL(startEmulation()),emulator,SLOT(startEmulation()));
      QObject::disconnect(this,SIGNAL(pauseEmulationAfter(int32_t)),emulator,SLOT(pauseEmulationAfter(int32_t)));
      QObject::disconnect(emulator,SIGNAL(emulatorPausedAfter()),this,SLOT(emulatorPausedAfter()));
   }
}

void TestSuiteExecutiveDialog::emulatorPausedAfter()
{
   doTestPhase();
}

void TestSuiteExecutiveDialog::machineReady()
{
   doTestPhase();
}

void TestSuiteExecutiveDialog::updateProgress()
{
   ui->testProgress->setValue(ui->testProgress->value()+1);
}

void TestSuiteExecutiveDialog::loadTestSuite(QString testSuiteFileName)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");
   QDir         testSuiteFolder(testSuiteFileName);
   QDomDocument testSuiteDoc;
   QDomElement  testSuiteElement;
   QDomNode     testNode;
   QDomElement  testElement;
   QString      testFileName;
   QString      testFrames;
   QString      testSystem;
   QString      testResult;
   QString      testFailComment;
   QString      testNotes;
   QString      previousSha1;
   QString      testRecordedInput;
   int          test = 0;
   int          numTests = 0;

   testSuiteFolder.cdUp();

   ui->testSuiteFileName->setText(testSuiteFolder.toNativeSeparators(testSuiteFolder.absoluteFilePath(testSuiteFileName)));

   if ( !testSuiteFileName.isEmpty() )
   {
      QFile testSuiteFile(testSuiteFileName);

      if ( testSuiteFile.exists() && testSuiteFile.open(QIODevice::ReadOnly|QIODevice::Text) )
      {
         settings.setValue("TestSuiteFile",testSuiteFolder.toNativeSeparators(testSuiteFolder.absoluteFilePath(testSuiteFileName)));
         testSuiteDoc.setContent(&testSuiteFile);
         testSuiteFile.close();
      }
   }

   testSuiteElement = testSuiteDoc.documentElement();
   testNode = testSuiteElement.firstChild();
   while ( !testNode.isNull() )
   {
      numTests++;

      testNode = testNode.nextSibling();
   }

   ui->tableWidget->setColumnCount(8);
   ui->tableWidget->setRowCount(numTests);

   QStringList headerLabels;
   headerLabels << "File Name" << "# of Frames" << "System" << "Last Result" << "Failure Comment" << "Notes" << "TV SHA1" << "Recorded Input";
   ui->tableWidget->setHorizontalHeaderLabels(headerLabels);

   testNode = testSuiteElement.firstChild();
   test = 0;
   while ( !testNode.isNull() )
   {
      testElement = testNode.toElement();

      testFileName = testElement.attribute("filename");
      testFrames = testElement.attribute("runframes");
      testSystem = testElement.attribute("system");
      testResult = testElement.attribute("testresult");
      testFailComment = testElement.attribute("failcomment");
      testNotes = testElement.attribute("testnotes");

      QDomNode    childNode = testElement.firstChild();
      while ( !childNode.isNull() )
      {
         QDomElement childElement = childNode.toElement();
         if ( childElement.nodeName() == "tvsha1" )
         {
            previousSha1 = childElement.firstChild().toCDATASection().data();
         }
         else if ( childElement.nodeName() == "recordedinput" )
         {
            testRecordedInput = childElement.firstChild().toCDATASection().data();
         }
         childNode = childNode.nextSibling();
      }

      ui->tableWidget->setItem(test,0,new QTableWidgetItem(testFileName));
      ui->tableWidget->setItem(test,1,new QTableWidgetItem(testFrames));
      ui->tableWidget->setItem(test,2,new QTableWidgetItem(testSystem));
      ui->tableWidget->setItem(test,3,new QTableWidgetItem(testResult));
      ui->tableWidget->setItem(test,4,new QTableWidgetItem(testFailComment));
      ui->tableWidget->setItem(test,5,new QTableWidgetItem(testNotes));
      ui->tableWidget->setItem(test,6,new QTableWidgetItem(previousSha1));
      ui->tableWidget->setItem(test,7,new QTableWidgetItem(testRecordedInput));

      testNode = testNode.nextSibling();
      test++;
   }
   ui->tableWidget->resizeRowsToContents();
   ui->tableWidget->resizeColumnsToContents();
}

void TestSuiteExecutiveDialog::on_load_clicked()
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");
   QString testSuiteFileName = QFileDialog::getOpenFileName(this,"Test Suite XML File",settings.value("TestSuiteFile").toString(),"XML Files (*.xml)");

   loadTestSuite(testSuiteFileName);
}

void TestSuiteExecutiveDialog::on_execute_clicked()
{
   if ( ui->tableWidget->rowCount() > 0 )
   {
      executeTests(-1,-1);
   }
}

void TestSuiteExecutiveDialog::on_executeSelection_clicked()
{
   QList<QTableWidgetItem*> items = ui->tableWidget->selectedItems();
   QTableWidgetItem* start = items.first();
   QTableWidgetItem* end = items.last();

   if ( start && end && (start->row() >= 0) && (end->row() >= 0) )
   {
      executeTests(start->row(),end->row()+1);
   }
}

void TestSuiteExecutiveDialog::doTestPhase()
{
   QString testSuiteFileName = ui->testSuiteFileName->text();
   QDir    testSuiteFolder(testSuiteFileName);
   QString testFileName;
   QString testFrames;
   QString testSystem;
   QString testResult;
   QString testFailComment;
   QString previousSha1;
   QString testRecordedInput;
   QByteArray inputSamplesRaw;
   JoypadLoggerInfo* inputSample;
   int     numInputSamples;
   static int     framesRun;
   int     sample;
   int     numTests = ui->tableWidget->rowCount();

   testSuiteFolder.cdUp();

   testFileName = ui->tableWidget->item(testRunning,0)->text();
   testFrames = ui->tableWidget->item(testRunning,1)->text();
   testSystem = ui->tableWidget->item(testRunning,2)->text();
   testResult = ui->tableWidget->item(testRunning,3)->text();
   testFailComment = ui->tableWidget->item(testRunning,4)->text();
   previousSha1 = ui->tableWidget->item(testRunning,6)->text();
   testRecordedInput = ui->tableWidget->item(testRunning,7)->text();

   switch ( testPhase )
   {
   case 0:
      ui->tableWidget->setCurrentCell(testRunning,4);

      ui->suiteProgress->setValue(testRunning+1);
      ui->testROM->setText(testSuiteFolder.toNativeSeparators(testSuiteFolder.absoluteFilePath(testFileName)));

      ui->testProgress->setMaximum(testFrames.toInt());
      ui->testProgress->setValue(0);

      framesRun = testFrames.toInt();

      nesResetInputRecording();

      if ( !ui->recordInputs->isChecked() )
      {
         inputSamplesRaw.clear();
         inputSamplesRaw = QByteArray::fromBase64(testRecordedInput.toLocal8Bit());

         inputSample = (JoypadLoggerInfo*)inputSamplesRaw.constData();
         numInputSamples = inputSamplesRaw.length()/sizeof(JoypadLoggerInfo);

         for ( sample = 0; sample < numInputSamples; sample++ )
         {
            nesSetInputSample(0,inputSample);
            inputSample++;
         }

         nesSetInputRecording(false);
         nesSetInputPlayback(true);
      }
      else
      {
         nesSetInputRecording(true);
         nesSetInputPlayback(false);
      }

      if ( testSystem == "ntsc" )
      {
         nesSetSystemMode(MODE_NTSC);
      }
      else
      {
         nesSetSystemMode(MODE_PAL);
      }

      emit openNesROM(ui->testROM->text(),false);

      emit pauseEmulationAfter(framesRun);

      emit startEmulation();

      // Go to next phase.
      testPhase = 1;
      break;

   case 1:
      ui->testProgress->setValue(ui->testProgress->maximum());

      QCryptographicHash crypto(QCryptographicHash::Sha1);

      crypto.addData((char*)nesGetTVOut(),256*240*4);

      if ( crypto.result().toBase64() != previousSha1.toLatin1() )
      {
         int result;

         QMessageBox dlg;
         dlg.setWindowTitle(testFileName+": Test Result");
         dlg.setText("Did the test pass?");
         dlg.setInformativeText("To add more time to the test run, click \"Retry\".\nTo abort test suite, click \"Abort\".");
         dlg.setStandardButtons(QMessageBox::Yes|QMessageBox::No|QMessageBox::Retry|QMessageBox::Abort);
         result = dlg.exec();
         if ( result == QMessageBox::Abort )
         {
            break;
         }
         if ( result == QMessageBox::Retry )
         {
            emit pauseEmulationAfter(60);

            framesRun += 60;

            ui->testProgress->setMaximum(ui->testProgress->maximum()+60);

            emit startEmulation();

            break;
         }

         if ( result == QMessageBox::No )
         {
            testFailComment = QInputDialog::getText(this,testFileName+": Enter failure comment","Reason:",QLineEdit::Normal,testFailComment);
         }
         else if ( result == QMessageBox::Yes )
         {
            testFailComment = "";
         }

         ui->tableWidget->item(testRunning,4)->setText(testFailComment);

         crypto.reset();
         crypto.addData((char*)nesGetTVOut(),256*240*4);

         if ( result == QMessageBox::Yes )
         {
            testResult = "pass";
         }
         else if ( result == QMessageBox::No )
         {
            testResult = "fail";
         }

         if ( ui->recordInputs->isChecked() )
         {
            inputSamplesRaw.clear();
            for ( sample = 0; sample < nesGetInputSamplesAvailable(0); sample++ )
            {
               inputSample = nesGetInputSample(0,sample);
               inputSamplesRaw.append((char*)inputSample,sizeof(JoypadLoggerInfo));
            }
            ui->tableWidget->item(testRunning,7)->setText(inputSamplesRaw.toBase64());
         }

         ui->tableWidget->item(testRunning,3)->setText(testResult);
         ui->tableWidget->item(testRunning,1)->setText(QString::number(framesRun));
         ui->tableWidget->item(testRunning,6)->setText(crypto.result().toBase64());

         testPhase = 0;
         testRunning++;
      }
      else
      {
         testPhase = 0;
         testRunning++;
      }

      if ( testResult == "pass" )
      {
         testsPassed++;
      }
      ui->passRate->setValue(((float)testsPassed/(float)numTests)*100);

      emit pauseEmulationAfter(-1);

      if ( testRunning < testEnd )
      {
         doTestPhase();
      }
      break;
   }
}

void TestSuiteExecutiveDialog::executeTests(int start,int end)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");
   int     numTests = ui->tableWidget->rowCount();

   // Kill the last loaded project
   settings.setValue("LastProject","");

   ui->suiteProgress->setMaximum(numTests);
   ui->suiteProgress->setValue(0);
   ui->testProgress->setValue(0);
   ui->passRate->setValue(0);

   aborted = false;

   if ( start == -1 )
   {
      start = 0;
   }
   if ( end == -1 )
   {
      end = numTests;
   }

   if ( start == end ) return;

   testStart = start;
   testEnd = end;
   testRunning = start;
   testPhase = 0;
   testsPassed = 0;

   // Kick off first phase of first test.
   doTestPhase();
}

void TestSuiteExecutiveDialog::on_abort_clicked()
{
   aborted = true;
}

void TestSuiteExecutiveDialog::on_clear_clicked()
{
   int     test;
   int     numTests = ui->tableWidget->rowCount();

   ui->suiteProgress->setMaximum(numTests);
   ui->suiteProgress->setValue(0);
   ui->testProgress->setValue(0);

   for ( test = 0; test < numTests; test++ )
   {
      ui->tableWidget->item(test,3)->setText("none");
      ui->tableWidget->item(test,4)->setText("");
      ui->tableWidget->item(test,5)->setText("");
      ui->tableWidget->item(test,6)->setText("");
      ui->tableWidget->item(test,7)->setText("");
   }
}

void TestSuiteExecutiveDialog::on_save_clicked()
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");
   QString testSuiteFileName = QFileDialog::getSaveFileName(this,"Test Suite XML File",settings.value("TestSuiteFile").toString(),"XML Files (*.xml)");
   QDir    testSuiteFolder(testSuiteFileName);
   QDomElement  testSuiteElement;
   QDomNode     testNode;
   QDomElement  testElement;
   QDomElement  childElement;
   QDomCDATASection dataSect;
   int          test = 0;
   int          numTests = ui->tableWidget->rowCount();
   QDomDocument testSuiteDoc;
   QDomProcessingInstruction instr = testSuiteDoc.createProcessingInstruction("xml", "version='1.0' encoding='UTF-8'");
   testSuiteDoc.appendChild(instr);

   testSuiteElement = testSuiteDoc.firstChildElement();

   testSuiteFolder.cdUp();

   ui->testSuiteFileName->setText(testSuiteFolder.toNativeSeparators(testSuiteFolder.absoluteFilePath(testSuiteFileName)));

   if ( !testSuiteFileName.isEmpty() )
   {
      QFile testSuiteFile(testSuiteFileName);

      if ( testSuiteFile.open(QIODevice::ReadWrite|QIODevice::Text|QIODevice::Truncate) )
      {
         settings.setValue("TestSuiteFile",testSuiteFolder.toNativeSeparators(testSuiteFolder.absoluteFilePath(testSuiteFileName)));

         testSuiteElement = addElement(testSuiteDoc,testSuiteDoc,"testsuite");
         for ( test = 0; test < numTests; test++ )
         {
            testElement = addElement(testSuiteDoc,testSuiteElement,"test");
            testElement.setAttribute("filename",ui->tableWidget->item(test,0)->text());
            testElement.setAttribute("runframes",ui->tableWidget->item(test,1)->text());
            testElement.setAttribute("system",ui->tableWidget->item(test,2)->text());
            testElement.setAttribute("testresult",ui->tableWidget->item(test,3)->text());
            testElement.setAttribute("failcomment",ui->tableWidget->item(test,4)->text());
            testElement.setAttribute("testnotes",ui->tableWidget->item(test,5)->text());
            childElement = addElement(testSuiteDoc,testElement,"tvsha1");
            dataSect = testSuiteDoc.createCDATASection(ui->tableWidget->item(test,6)->text());
            childElement.appendChild(dataSect);
            childElement = addElement(testSuiteDoc,testElement,"recordedinput");
            dataSect = testSuiteDoc.createCDATASection(ui->tableWidget->item(test,7)->text());
            childElement.appendChild(dataSect);
         }

         testSuiteFile.write(testSuiteDoc.toByteArray());
         testSuiteFile.close();
      }
   }
}
