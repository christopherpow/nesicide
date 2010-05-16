#include "aboutdialog.h"
#include "ui_aboutdialog.h"

aboutdialog::aboutdialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::aboutdialog)
{
    ui->setupUi(this);
    ui->webView->setHtml("<html><head></head><body style=\"background-color: black; color: #CCCCCC; text-align:center; font-family:sans-serif; font-size: 10pt\">"
                         "<img src=\"http://www.nesicide.com/images/nesicide.jpg\" /><br />"
                         "NESICIDE - The NES Development GUI.<br /><br />"
                         "<b>Programmers</b><br />"
                         "Christopher Pow - <i>Emulation, Assembler and Debugger Subsystems</i><br />"
                         "Tim Sarbin - <i>User Interface and Designers</i><br />"
                         "<br /><b>Support</b><br />"
                         "Benjamin Eriksson - <i>Mac Platform Testing, Graphics, End User Testing"
                         "</body></html>");
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
