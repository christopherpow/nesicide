#include <stdint.h>

#include "attributetableeditorform.h"
#include "ui_attributetableeditorform.h"

#include "emulator_core.h"
#include "cnessystempalette.h"

#include <QToolTip>

AttributeTableEditorForm::AttributeTableEditorForm(QList<uint8_t> palette,IProjectTreeViewItem* link,QWidget* parent) :
   CDesignerEditorBase(link,parent),
   ui(new Ui::AttributeTableEditorForm)
{
   ui->setupUi(this);

   m_palette = palette;

   ui->bkgndcol->setCurrentColor(QColor(nesGetPaletteRedComponent(m_palette.at(0)),nesGetPaletteGreenComponent(m_palette.at(0)),nesGetPaletteBlueComponent(m_palette.at(0))));
   QObject::connect(ui->bkgndcol,SIGNAL(colorChanged(QColor)),this,SLOT(colorChanged(QColor)));

   ui->pal0col1->setCurrentColor(QColor(nesGetPaletteRedComponent(m_palette.at(1)),nesGetPaletteGreenComponent(m_palette.at(1)),nesGetPaletteBlueComponent(m_palette.at(1))));
   QObject::connect(ui->pal0col1,SIGNAL(colorChanged(QColor)),this,SLOT(colorChanged(QColor)));
   ui->pal0col2->setCurrentColor(QColor(nesGetPaletteRedComponent(m_palette.at(2)),nesGetPaletteGreenComponent(m_palette.at(2)),nesGetPaletteBlueComponent(m_palette.at(2))));
   QObject::connect(ui->pal0col2,SIGNAL(colorChanged(QColor)),this,SLOT(colorChanged(QColor)));
   ui->pal0col3->setCurrentColor(QColor(nesGetPaletteRedComponent(m_palette.at(3)),nesGetPaletteGreenComponent(m_palette.at(3)),nesGetPaletteBlueComponent(m_palette.at(3))));
   QObject::connect(ui->pal0col3,SIGNAL(colorChanged(QColor)),this,SLOT(colorChanged(QColor)));

   ui->pal1col1->setCurrentColor(QColor(nesGetPaletteRedComponent(m_palette.at(5)),nesGetPaletteGreenComponent(m_palette.at(5)),nesGetPaletteBlueComponent(m_palette.at(5))));
   QObject::connect(ui->pal1col1,SIGNAL(colorChanged(QColor)),this,SLOT(colorChanged(QColor)));
   ui->pal1col2->setCurrentColor(QColor(nesGetPaletteRedComponent(m_palette.at(6)),nesGetPaletteGreenComponent(m_palette.at(6)),nesGetPaletteBlueComponent(m_palette.at(6))));
   QObject::connect(ui->pal1col2,SIGNAL(colorChanged(QColor)),this,SLOT(colorChanged(QColor)));
   ui->pal1col3->setCurrentColor(QColor(nesGetPaletteRedComponent(m_palette.at(7)),nesGetPaletteGreenComponent(m_palette.at(7)),nesGetPaletteBlueComponent(m_palette.at(7))));
   QObject::connect(ui->pal1col3,SIGNAL(colorChanged(QColor)),this,SLOT(colorChanged(QColor)));

   ui->pal2col1->setCurrentColor(QColor(nesGetPaletteRedComponent(m_palette.at(9)),nesGetPaletteGreenComponent(m_palette.at(9)),nesGetPaletteBlueComponent(m_palette.at(9))));
   QObject::connect(ui->pal2col1,SIGNAL(colorChanged(QColor)),this,SLOT(colorChanged(QColor)));
   ui->pal2col2->setCurrentColor(QColor(nesGetPaletteRedComponent(m_palette.at(10)),nesGetPaletteGreenComponent(m_palette.at(10)),nesGetPaletteBlueComponent(m_palette.at(10))));
   QObject::connect(ui->pal2col2,SIGNAL(colorChanged(QColor)),this,SLOT(colorChanged(QColor)));
   ui->pal2col3->setCurrentColor(QColor(nesGetPaletteRedComponent(m_palette.at(11)),nesGetPaletteGreenComponent(m_palette.at(11)),nesGetPaletteBlueComponent(m_palette.at(11))));
   QObject::connect(ui->pal2col3,SIGNAL(colorChanged(QColor)),this,SLOT(colorChanged(QColor)));

   ui->pal3col1->setCurrentColor(QColor(nesGetPaletteRedComponent(m_palette.at(13)),nesGetPaletteGreenComponent(m_palette.at(13)),nesGetPaletteBlueComponent(m_palette.at(13))));
   QObject::connect(ui->pal3col1,SIGNAL(colorChanged(QColor)),this,SLOT(colorChanged(QColor)));
   ui->pal3col2->setCurrentColor(QColor(nesGetPaletteRedComponent(m_palette.at(14)),nesGetPaletteGreenComponent(m_palette.at(14)),nesGetPaletteBlueComponent(m_palette.at(14))));
   QObject::connect(ui->pal3col2,SIGNAL(colorChanged(QColor)),this,SLOT(colorChanged(QColor)));
   ui->pal3col3->setCurrentColor(QColor(nesGetPaletteRedComponent(m_palette.at(15)),nesGetPaletteGreenComponent(m_palette.at(15)),nesGetPaletteBlueComponent(m_palette.at(15))));
   QObject::connect(ui->pal3col3,SIGNAL(colorChanged(QColor)),this,SLOT(colorChanged(QColor)));
}

AttributeTableEditorForm::~AttributeTableEditorForm()
{
   delete ui;
}

void AttributeTableEditorForm::changeEvent(QEvent* event)
{
   QWidget::changeEvent(event);

   switch (event->type())
   {
      case QEvent::LanguageChange:
         ui->retranslateUi(this);
         break;
      default:
         break;
   }
}

void AttributeTableEditorForm::contextMenuEvent(QContextMenuEvent *event)
{
}

void AttributeTableEditorForm::colorChanged(QColor color)
{
   if ( sender() == ui->bkgndcol )
   {
      m_palette.replace(0x00,CBasePalette::GetPaletteIndex(color.red(),color.green(),color.blue()));
      m_palette.replace(0x04,CBasePalette::GetPaletteIndex(color.red(),color.green(),color.blue()));
      m_palette.replace(0x08,CBasePalette::GetPaletteIndex(color.red(),color.green(),color.blue()));
      m_palette.replace(0x0C,CBasePalette::GetPaletteIndex(color.red(),color.green(),color.blue()));
   }
   else if ( sender() == ui->pal0col1 )
   {
      m_palette.replace(0x01,CBasePalette::GetPaletteIndex(color.red(),color.green(),color.blue()));
   }
   else if ( sender() == ui->pal0col2 )
   {
      m_palette.replace(0x02,CBasePalette::GetPaletteIndex(color.red(),color.green(),color.blue()));
   }
   else if ( sender() == ui->pal0col3 )
   {
      m_palette.replace(0x03,CBasePalette::GetPaletteIndex(color.red(),color.green(),color.blue()));
   }
   else if ( sender() == ui->pal1col1 )
   {
      m_palette.replace(0x05,CBasePalette::GetPaletteIndex(color.red(),color.green(),color.blue()));
   }
   else if ( sender() == ui->pal1col2 )
   {
      m_palette.replace(0x06,CBasePalette::GetPaletteIndex(color.red(),color.green(),color.blue()));
   }
   else if ( sender() == ui->pal1col3 )
   {
      m_palette.replace(0x07,CBasePalette::GetPaletteIndex(color.red(),color.green(),color.blue()));
   }
   else if ( sender() == ui->pal2col1 )
   {
      m_palette.replace(0x09,CBasePalette::GetPaletteIndex(color.red(),color.green(),color.blue()));
   }
   else if ( sender() == ui->pal2col2 )
   {
      m_palette.replace(0x0A,CBasePalette::GetPaletteIndex(color.red(),color.green(),color.blue()));
   }
   else if ( sender() == ui->pal2col3 )
   {
      m_palette.replace(0x0B,CBasePalette::GetPaletteIndex(color.red(),color.green(),color.blue()));
   }
   else if ( sender() == ui->pal3col1 )
   {
      m_palette.replace(0x0D,CBasePalette::GetPaletteIndex(color.red(),color.green(),color.blue()));
   }
   else if ( sender() == ui->pal3col2 )
   {
      m_palette.replace(0x0E,CBasePalette::GetPaletteIndex(color.red(),color.green(),color.blue()));
   }
   else if ( sender() == ui->pal3col3 )
   {
      m_palette.replace(0x0F,CBasePalette::GetPaletteIndex(color.red(),color.green(),color.blue()));
   }
   setModified(true);
   emit markProjectDirty(true);
}
