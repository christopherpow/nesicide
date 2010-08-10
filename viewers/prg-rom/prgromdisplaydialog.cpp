#include "prgromdisplaydialog.h"
#include "ui_prgromdisplaydialog.h"

#include "main.h"

PRGROMDisplayDialog::PRGROMDisplayDialog(QWidget *parent) :
    QDialog(parent),
    m_data(0),
    ui(new Ui::PRGROMDisplayDialog)
{
    ui->setupUi(this);
}

PRGROMDisplayDialog::~PRGROMDisplayDialog()
{
    delete ui;
}

void PRGROMDisplayDialog::changeEvent(QEvent *e)
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

unsigned char c2a(unsigned char c)
{
   return hex_char [ c ];
}

void PRGROMDisplayDialog::showEvent(QShowEvent *e)
{
   if ( m_data )
   {
      QString rt;
      for(int i=0; i<MEM_16KB; i++)
      {
          char l = (m_data[i]>>4)&0x0F;
          char r = m_data[i]&0x0F;
          QChar c[2] = { c2a(l), c2a(r) };
          rt += QString(c,2);
          rt += " ";
      }

      ui->textBrowser->setText(rt);
   }
}
