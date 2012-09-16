#include "aboutdialog.h"
#include "ui_aboutdialog.h"

#include "nes_emulator_core.h"

#include <QProcess>
#include <QDir>

extern char* nesicideGetVersion();

AboutDialog::AboutDialog(QWidget* parent) :
   QDialog(parent),
   ui(new Ui::AboutDialog)
{
   ui->setupUi(this);

   ui->versionsText->append("<a href=\"#top\"/>");
   ui->versionsText->append("<b>NESICIDE standalone emulator version:</b>");
   ui->versionsText->append(nesicideGetVersion());
   ui->versionsText->append("");

   ui->versionsText->append("<b>NESICIDE Emulator Library version:</b>");
   ui->versionsText->append(nesGetVersion());
   ui->versionsText->append("");

   ui->versionsText->scrollToAnchor("top");
}

AboutDialog::~AboutDialog()
{
   delete ui;
}
