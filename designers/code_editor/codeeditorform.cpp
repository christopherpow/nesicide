#include "codeeditorform.h"
#include "ui_codeeditorform.h"

#include <QToolTip>
#include <QMenu>

#include "pasm_lib.h"
#include "dbg_cnes6502.h"

CodeEditorForm::CodeEditorForm(QWidget* parent) :
   QWidget(parent),
   ui(new Ui::CodeEditorForm)
{
   ui->setupUi(this);
   ui->textEdit->viewport()->setMouseTracking(true);
}

CodeEditorForm::~CodeEditorForm()
{
   delete ui;
}

void CodeEditorForm::changeEvent(QEvent* e)
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

void CodeEditorForm::mouseMoveEvent ( QMouseEvent* e )
{
   QTextCursor textCursor = ui->textEdit->cursorForPosition(e->pos());
   QString text = textCursor.block().text();
   setToolTip ( text );
   e->accept();
}

void CodeEditorForm::contextMenuEvent ( QContextMenuEvent* e )
{
   QMenu menu;

   // Build context menu...
   menu.addSeparator();
   menu.addSeparator();
   menu.addSeparator();

   // Run the context menu...
   menu.exec(e->globalPos());
}

QString CodeEditorForm::get_sourceCode()
{
   return ui->textEdit->toPlainText();
}

void CodeEditorForm::set_sourceCode(QString source)
{
   ui->textEdit->setPlainText(source);
}

void CodeEditorForm::selectLine(int linenumber)
{
   QTextCursor textCursor = ui->textEdit->textCursor();
   textCursor.movePosition(QTextCursor::Start);
   textCursor.movePosition(QTextCursor::Down,QTextCursor::MoveAnchor,linenumber-1);
//   textCursor.movePosition(QTextCursor::EndOfLine);
//   textCursor.movePosition(QTextCursor::StartOfLine,QTextCursor::KeepAnchor);
   ui->textEdit->setTextCursor(textCursor);
   ui->textEdit->highlightCurrentLine();
}

void CodeEditorForm::on_textEdit_textChanged()
{
}

void CodeEditorForm::on_textEdit_selectionChanged()
{
   QTextCursor textCursor = ui->textEdit->textCursor();
   QString selection = textCursor.selectedText();
   symbol_table* pSymbol;
   int symbol;
   int symbolType;
   QString toolTipText;
   const char* opcodeToolTipText;
   
   setToolTip("");
   
   opcodeToolTipText = OPCODEINFO(selection.toAscii().constData());
   if ( opcodeToolTipText )
   {
      toolTipText = opcodeToolTipText;
   }
   else
   {   
      for ( symbol = 0; symbol < pasm_get_num_symbols(); symbol++ )
      {   
         if ( selection == pasm_get_symbol_name_by_index(symbol) )
         {
            pSymbol = pasm_get_symbol_by_index(symbol);
            symbolType = pasm_get_symbol_type_by_index(symbol);         
            
            if ( symbolType == symbol_global )
            {
               toolTipText.sprintf("GLOBAL %s: %04X", pSymbol->symbol, pSymbol->expr->value.ival);
            }
            else
            {
               toolTipText.sprintf("LABEL %s: %04X", pSymbol->symbol, pSymbol->ir->addr);
            }
   
            break;
         }
      }
   }
   setToolTip(toolTipText);
         
   // Figure out what is being selected and show an appropriate tooltip...
   // 1. If selection is 6502 opcode, show helpful information on it.
   // 2. If selection is a compiler-identified symbol, show its address and value.
   // 3. If selection is a macro, show its declaration.
   // 4. If selection is a label, show its address.
   // 5. If selection is an expression, evaluate and display its current value.
}
