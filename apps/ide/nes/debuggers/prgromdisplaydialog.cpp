#include "prgromdisplaydialog.h"
#include "ui_prgromdisplaydialog.h"

#include "main.h"

PRGROMDisplayDialog::PRGROMDisplayDialog(uint8_t* bankData,IProjectTreeViewItem* link,QWidget* parent) :
   CDesignerEditorBase(link,parent),
   m_data(0),
   ui(new Ui::PRGROMDisplayDialog)
{
   ui->setupUi(this);

   m_data = bankData;

   if ( m_data )
   {
      QString rt;
      char    temp[4];

      for (int i=0; i<MEM_8KB; i++)
      {
         char l = (m_data[i]>>4)&0x0F;
         char r = m_data[i]&0x0F;
         sprintf(temp,"%01X%01X ",l,r);
         rt.append(temp);
      }

      ui->textBrowser->setText(rt);
   }
}

PRGROMDisplayDialog::~PRGROMDisplayDialog()
{
   delete ui;
}

void PRGROMDisplayDialog::changeEvent(QEvent* e)
{
   QWidget::changeEvent(e);

   switch (e->type())
   {
      case QEvent::LanguageChange:
         ui->retranslateUi(this);
         break;
      default:
         break;
   }
}

void PRGROMDisplayDialog::showEvent(QShowEvent* /*e*/)
{
}

void PRGROMDisplayDialog::applyProjectPropertiesToTab()
{
}
