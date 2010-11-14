#include "aboutdialog.h"
#include "ui_aboutdialog.h"

#include "main.h"
#include "emulator_core.h"

aboutdialog::aboutdialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::aboutdialog)
{
   char aboutTextFmt [ 2048 ] = "<html><head></head><body style=\"background-color: black; color: #CCCCCC; text-align:center; font-family:sans-serif; font-size: 10pt\">"
                         "<img src=\"http://wiki.nesicide.com/lib/exe/fetch.php?media=nesicide.png\" /><br /><br />"
                         "NESICIDE2: The NES Development GUI.<br /><br />"
                         "<b>Programmers</b><br />"
                         "Christopher Pow: <i>Emulation, Assembler and Debugger Subsystems</i><br />"
                         "Tim Sarbin: <i>User Interface and Designers</i><br />"
                         "Benjamin Eriksson: <i>Designers, Graphics, Mac Platform Testing</i><br />"
                         "<br /><b>Component Versions</b><br />"
                         "IDE: %s<br />"
                         "NES Emulation Core Library: %s"
                         "</body></html>";
   char aboutText [ 2048 ];
   
   sprintf(aboutText,aboutTextFmt,ideGetVersion(), nesGetVersion());
   ui->setupUi(this);
   ui->webView->setHtml(aboutText);
}

aboutdialog::~aboutdialog()
{
    delete ui;
}

void aboutdialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
