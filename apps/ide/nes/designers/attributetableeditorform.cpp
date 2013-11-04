#include <stdint.h>

#include "attributetableeditorform.h"
#include "ui_attributetableeditorform.h"

#include "nes_emulator_core.h"
#include "cnessystempalette.h"

#include <QToolTip>

AttributeTableEditorForm::AttributeTableEditorForm(QList<uint8_t> palette,IProjectTreeViewItem* link,QWidget* parent) :
   CDesignerEditorBase(link,parent),
   ui(new Ui::AttributeTableEditorForm)
{
   int idx;

   ui->setupUi(this);

   m_palette = palette;

   m_colors.append(ui->bkgndcol);
   m_colors.append(ui->pal0col1);
   m_colors.append(ui->pal0col2);
   m_colors.append(ui->pal0col3);
   m_colors.append(ui->bkgndcol);
   m_colors.append(ui->pal1col1);
   m_colors.append(ui->pal1col2);
   m_colors.append(ui->pal1col3);
   m_colors.append(ui->bkgndcol);
   m_colors.append(ui->pal2col1);
   m_colors.append(ui->pal2col2);
   m_colors.append(ui->pal2col3);
   m_colors.append(ui->bkgndcol);
   m_colors.append(ui->pal3col1);
   m_colors.append(ui->pal3col2);
   m_colors.append(ui->pal3col3);

   for ( idx = 0; idx < m_colors.count(); idx++ )
   {
      m_colors.at(idx)->setCurrentColor(QColor(nesGetPaletteRedComponent(m_palette.at(idx)),nesGetPaletteGreenComponent(m_palette.at(idx)),nesGetPaletteBlueComponent(m_palette.at(idx))));
      QObject::connect(m_colors.at(idx),SIGNAL(colorChanged(QColor)),this,SLOT(colorChanged(QColor)));
   }
   
   setCentralWidget(ui->window);
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

void AttributeTableEditorForm::contextMenuEvent(QContextMenuEvent */*event*/)
{
}

void AttributeTableEditorForm::keyPressEvent(QKeyEvent *event)
{
   int idx;

   if ( event->modifiers() == Qt::ControlModifier )
   {
      if ( event->key() == Qt::Key_Z )
      {
         m_undoStack.undo();
         if ( m_undoStack.isClean() )
         {
            setModified(false);
         }
         for ( idx = 0; idx < m_colors.count(); idx++ )
         {
            m_colors.at(idx)->update();
         }
      }
      else if ( event->key() == Qt::Key_Y )
      {
         m_undoStack.redo();
         for ( idx = 0; idx < m_colors.count(); idx++ )
         {
            m_colors.at(idx)->update();
         }
         setModified(true);
         emit markProjectDirty(true);
      }
   }
}

void AttributeTableEditorForm::colorChanged(QColor color)
{
   int idx;
   uint8_t oldColor;

   for ( idx = 0; idx < m_colors.count(); idx++ )
   {
      if ( sender() == m_colors.at(idx) )
      {
         if ( !(idx%4) )
         {
            m_palette.replace(0x00,CBasePalette::GetPaletteIndex(color.red(),color.green(),color.blue()));
            m_palette.replace(0x04,CBasePalette::GetPaletteIndex(color.red(),color.green(),color.blue()));
            m_palette.replace(0x08,CBasePalette::GetPaletteIndex(color.red(),color.green(),color.blue()));
            m_palette.replace(0x0C,CBasePalette::GetPaletteIndex(color.red(),color.green(),color.blue()));
         }
         else
         {
            oldColor = CBasePalette::GetPaletteIndex(color.red(),color.green(),color.blue());
            m_palette.replace(idx,CBasePalette::GetPaletteIndex(color.red(),color.green(),color.blue()));
            m_undoStack.push(new AttributeTableChangeColorCommand(this,idx,oldColor));
         }
      }
   }
   setModified(true);
   emit markProjectDirty(true);
}

void AttributeTableEditorForm::applyProjectPropertiesToTab()
{
}

AttributeTableChangeColorCommand::AttributeTableChangeColorCommand(AttributeTableEditorForm *pEditor,
                                             int colorIdx,
                                             uint8_t oldColor,
                                             QUndoCommand *parent)
   : QUndoCommand(parent),
     m_pEditor(pEditor),
     m_colorIdx(colorIdx),
     m_oldColor(oldColor)
{
   setText("change");
   m_newColor = pEditor->attributeTable().at(m_colorIdx);
}

bool AttributeTableChangeColorCommand::mergeWith(const QUndoCommand* /*command*/)
{
   return false;
}

void AttributeTableChangeColorCommand::redo()
{
   m_pEditor->setPalette(m_colorIdx,m_newColor);
}

void AttributeTableChangeColorCommand::undo()
{
   m_pEditor->setPalette(m_colorIdx,m_oldColor);
}
