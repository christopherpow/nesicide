#include "attributetableeditorform.h"
#include "ui_attributetableeditorform.h"

#include "emulator_core.h"
#include "cnessystempalette.h"

#include <QToolTip>

AttributeTableEditorForm::AttributeTableEditorForm(IProjectTreeViewItem* link,QWidget* parent) :
   CDesignerEditorBase(link,parent),
   ui(new Ui::AttributeTableEditorForm)
{
   ui->setupUi(this);

   for (int i=0; i<NUM_PALETTES; i++)
   {
      ui->bkgndcol->insertColor(QColor(nesGetPaletteRedComponent(i),nesGetPaletteGreenComponent(i),nesGetPaletteBlueComponent(i)), "", i);

      ui->pal0col1->insertColor(QColor(nesGetPaletteRedComponent(i),nesGetPaletteGreenComponent(i),nesGetPaletteBlueComponent(i)), "", i);
      ui->pal0col2->insertColor(QColor(nesGetPaletteRedComponent(i),nesGetPaletteGreenComponent(i),nesGetPaletteBlueComponent(i)), "", i);
      ui->pal0col3->insertColor(QColor(nesGetPaletteRedComponent(i),nesGetPaletteGreenComponent(i),nesGetPaletteBlueComponent(i)), "", i);

      ui->pal1col1->insertColor(QColor(nesGetPaletteRedComponent(i),nesGetPaletteGreenComponent(i),nesGetPaletteBlueComponent(i)), "", i);
      ui->pal1col2->insertColor(QColor(nesGetPaletteRedComponent(i),nesGetPaletteGreenComponent(i),nesGetPaletteBlueComponent(i)), "", i);
      ui->pal1col3->insertColor(QColor(nesGetPaletteRedComponent(i),nesGetPaletteGreenComponent(i),nesGetPaletteBlueComponent(i)), "", i);

      ui->pal2col1->insertColor(QColor(nesGetPaletteRedComponent(i),nesGetPaletteGreenComponent(i),nesGetPaletteBlueComponent(i)), "", i);
      ui->pal2col2->insertColor(QColor(nesGetPaletteRedComponent(i),nesGetPaletteGreenComponent(i),nesGetPaletteBlueComponent(i)), "", i);
      ui->pal2col3->insertColor(QColor(nesGetPaletteRedComponent(i),nesGetPaletteGreenComponent(i),nesGetPaletteBlueComponent(i)), "", i);

      ui->pal3col1->insertColor(QColor(nesGetPaletteRedComponent(i),nesGetPaletteGreenComponent(i),nesGetPaletteBlueComponent(i)), "", i);
      ui->pal3col2->insertColor(QColor(nesGetPaletteRedComponent(i),nesGetPaletteGreenComponent(i),nesGetPaletteBlueComponent(i)), "", i);
      ui->pal3col3->insertColor(QColor(nesGetPaletteRedComponent(i),nesGetPaletteGreenComponent(i),nesGetPaletteBlueComponent(i)), "", i);
   }

   ui->bkgndcol->setCurrentColor(QColor(nesGetPaletteRedComponent(0x0D),nesGetPaletteGreenComponent(0x0D),nesGetPaletteBlueComponent(0x0D)));

   ui->pal0col1->setCurrentColor(QColor(nesGetPaletteRedComponent(0x00),nesGetPaletteGreenComponent(0x00),nesGetPaletteBlueComponent(0x00)));
   ui->pal0col2->setCurrentColor(QColor(nesGetPaletteRedComponent(0x10),nesGetPaletteGreenComponent(0x10),nesGetPaletteBlueComponent(0x10)));
   ui->pal0col3->setCurrentColor(QColor(nesGetPaletteRedComponent(0x20),nesGetPaletteGreenComponent(0x20),nesGetPaletteBlueComponent(0x20)));

   ui->pal1col1->setCurrentColor(QColor(nesGetPaletteRedComponent(0x00),nesGetPaletteGreenComponent(0x00),nesGetPaletteBlueComponent(0x00)));
   ui->pal1col2->setCurrentColor(QColor(nesGetPaletteRedComponent(0x10),nesGetPaletteGreenComponent(0x10),nesGetPaletteBlueComponent(0x10)));
   ui->pal1col3->setCurrentColor(QColor(nesGetPaletteRedComponent(0x20),nesGetPaletteGreenComponent(0x20),nesGetPaletteBlueComponent(0x20)));

   ui->pal2col1->setCurrentColor(QColor(nesGetPaletteRedComponent(0x00),nesGetPaletteGreenComponent(0x00),nesGetPaletteBlueComponent(0x00)));
   ui->pal2col2->setCurrentColor(QColor(nesGetPaletteRedComponent(0x10),nesGetPaletteGreenComponent(0x10),nesGetPaletteBlueComponent(0x10)));
   ui->pal2col3->setCurrentColor(QColor(nesGetPaletteRedComponent(0x20),nesGetPaletteGreenComponent(0x20),nesGetPaletteBlueComponent(0x20)));

   ui->pal3col1->setCurrentColor(QColor(nesGetPaletteRedComponent(0x00),nesGetPaletteGreenComponent(0x00),nesGetPaletteBlueComponent(0x00)));
   ui->pal3col2->setCurrentColor(QColor(nesGetPaletteRedComponent(0x10),nesGetPaletteGreenComponent(0x10),nesGetPaletteBlueComponent(0x10)));
   ui->pal3col3->setCurrentColor(QColor(nesGetPaletteRedComponent(0x20),nesGetPaletteGreenComponent(0x20),nesGetPaletteBlueComponent(0x20)));
}

AttributeTableEditorForm::~AttributeTableEditorForm()
{
   delete ui;
}

void AttributeTableEditorForm::changeEvent(QEvent* e)
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
