#include "codeeditorform.h"
#include "ui_codeeditorform.h"

#include <QToolTip>
#include <QMenu>

#include "main.h"

#include "pasm_lib.h"

#include "inspectorregistry.h"

#include "dbg_cnes6502.h"

#include "cbreakpointinfo.h"
#include "cmarker.h"

CodeEditorForm::CodeEditorForm(QString fileName,QWidget* parent) :
   QWidget(parent),
   ui(new Ui::CodeEditorForm)
{
   QDockWidget* codeBrowser = InspectorRegistry::getInspector("Code Browser");
   
   ui->setupUi(this);
   ui->textEdit->viewport()->setMouseTracking(true);

   QObject::connect(codeBrowser,SIGNAL(breakpointsChanged()),ui->textEdit, SLOT(repaint()) );
   
   // Connect signals to the UI to have the UI update.
   QObject::connect ( emulator, SIGNAL(cartridgeLoaded()), this, SLOT(repaint()) );
   QObject::connect ( emulator, SIGNAL(emulatorReset()), this, SLOT(repaint()) );
   QObject::connect ( emulator, SIGNAL(emulatorPaused(bool)), this, SLOT(repaint()) );
   QObject::connect ( breakpointWatcher, SIGNAL(breakpointHit()), this, SLOT(repaint()) );

   m_fileName = fileName;
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
   QTextCursor textCursor = ui->textEdit->cursorForPosition(e->globalPos());
   QString text = textCursor.block().text();
   setToolTip ( text );
   e->accept();
}

void CodeEditorForm::contextMenuEvent(QContextMenuEvent *e)
{
   QMenu *menu = ui->textEdit->createStandardContextMenu();
   QTextCursor textCursor = ui->textEdit->cursorForPosition(e->pos());
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   int bp;
   int addr = 0;
   int absAddr = 0;
   
   addr = pasm_get_source_addr_by_linenum_and_file ( textCursor.blockNumber()+1, ui->textEdit->documentTitle().toAscii().constData() );

   absAddr = pasm_get_source_absolute_addr_by_linenum_and_file ( textCursor.blockNumber()+1, ui->textEdit->documentTitle().toAscii().constData() );

   if ( addr != -1 )
   {
      bp = pBreakpoints->FindExactMatch ( eBreakOnCPUExecution,
                                          eBreakpointItemAddress,
                                          0,
                                          addr,
                                          absAddr,
                                          addr,
                                          eBreakpointConditionNone,
                                          0,
                                          eBreakpointDataNone,
                                          0 );
   
      // Build context menu...
      menu->addSeparator();
      menu->addAction(ui->actionRun_to_here);
      menu->addSeparator();
   
      // If breakpoint isn't set here, give menu options to set one...
      if ( bp < 0 )
      {
         menu->addAction(ui->actionBreak_on_CPU_execution_here);
      }
      else
      {
         if ( pBreakpoints->GetStatus(bp) == Breakpoint_Disabled )
         {
            menu->addAction(ui->actionEnable_breakpoint);
            menu->addAction(ui->actionRemove_breakpoint);
         }
         else
         {
            menu->addAction(ui->actionDisable_breakpoint);
            menu->addAction(ui->actionRemove_breakpoint);
         }
      }
   
      menu->addSeparator();
      menu->addAction(ui->actionClear_marker);
      menu->addSeparator();
   
      menu->addAction(ui->actionStart_marker_here);
      menu->addAction(ui->actionEnd_marker_here);   
      
      m_breakpointIndex = bp;
      
      m_ctxtTextCursor = textCursor;
   }

   // Run the context menu...
   menu->exec(e->globalPos());

   emit breakpointsChanged();
   
   delete menu;
}


void CodeEditorForm::on_actionBreak_on_CPU_execution_here_triggered()
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   int bpIdx;
   int addr = 0;
   int absAddr = 0;

   addr = pasm_get_source_addr_by_linenum_and_file ( m_ctxtTextCursor.blockNumber()+1, ui->textEdit->documentTitle().toAscii().constData() );

   absAddr = pasm_get_source_absolute_addr_by_linenum_and_file ( m_ctxtTextCursor.blockNumber()+1, ui->textEdit->documentTitle().toAscii().constData() );

   if ( addr != -1 )
   {
      bpIdx = pBreakpoints->AddBreakpoint ( eBreakOnCPUExecution,
                                            eBreakpointItemAddress,
                                            0,
                                            addr,
                                            absAddr,
                                            addr,
                                            eBreakpointConditionNone,
                                            0,
                                            eBreakpointDataNone,
                                            0,
                                            true );
   
      if ( bpIdx < 0 )
      {
         QString str;
         str.sprintf("Cannot add breakpoint, maximum of %d already used.", NUM_BREAKPOINTS);
         QMessageBox::information(0, "Error", str);
      }
   
      InspectorRegistry::getInspector("Breakpoints")->hide();
      InspectorRegistry::getInspector("Breakpoints")->show();
   
      emit breakpointsChanged();
   }
}

void CodeEditorForm::on_actionRun_to_here_triggered()
{
   int addr = 0;
   int absAddr = 0;

   addr = pasm_get_source_addr_by_linenum_and_file ( m_ctxtTextCursor.blockNumber()+1, ui->textEdit->documentTitle().toAscii().constData() );

   absAddr = pasm_get_source_absolute_addr_by_linenum_and_file ( m_ctxtTextCursor.blockNumber()+1, ui->textEdit->documentTitle().toAscii().constData() );

   if ( addr != -1 )
   {
//      C6502DBG::GOTO(addr);
   }// CPTODO: fix the goto for absolute
}

void CodeEditorForm::on_actionDisable_breakpoint_triggered()
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();

   if ( m_breakpointIndex >= 0 )
   {
      pBreakpoints->ToggleEnabled(m_breakpointIndex);
   }
}

void CodeEditorForm::on_actionRemove_breakpoint_triggered()
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();

   if ( m_breakpointIndex >= 0 )
   {
      pBreakpoints->RemoveBreakpoint(m_breakpointIndex);
   }
}

void CodeEditorForm::on_actionEnable_breakpoint_triggered()
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();

   if ( m_breakpointIndex >= 0 )
   {
      pBreakpoints->ToggleEnabled(m_breakpointIndex);
   }
}

void CodeEditorForm::on_actionStart_marker_here_triggered()
{
   CMarker* markers = nesGetExecutionMarkerDatabase();
   int marker;
   int addr = 0;
   int absAddr = 0;

   addr = pasm_get_source_addr_by_linenum_and_file ( m_ctxtTextCursor.blockNumber()+1, ui->textEdit->documentTitle().toAscii().constData() );

   absAddr = pasm_get_source_absolute_addr_by_linenum_and_file ( m_ctxtTextCursor.blockNumber()+1, ui->textEdit->documentTitle().toAscii().constData() );

   if ( addr != -1 )
   {
      // Find unused Marker entry...
      marker = markers->AddMarker(absAddr);
   }
}

void CodeEditorForm::on_actionEnd_marker_here_triggered()
{
   CMarker* markers = nesGetExecutionMarkerDatabase();
   int marker = markers->FindInProgressMarker();
   int addr = 0;
   int absAddr = 0;

   if ( marker >= 0 )
   {
      addr = pasm_get_source_addr_by_linenum_and_file ( m_ctxtTextCursor.blockNumber()+1, ui->textEdit->documentTitle().toAscii().constData() );
   
      absAddr = pasm_get_source_absolute_addr_by_linenum_and_file ( m_ctxtTextCursor.blockNumber()+1, ui->textEdit->documentTitle().toAscii().constData() );

      if ( addr != -1 )
      {
         markers->CompleteMarker(marker,nesGetAbsoluteAddressFromAddress(addr));
      }
   }
}

void CodeEditorForm::on_actionClear_marker_triggered()
{
   CMarker* markers = nesGetExecutionMarkerDatabase();
   markers->ClearAllMarkers();
}

QString CodeEditorForm::get_sourceCode()
{
   return ui->textEdit->toPlainText();
}

void CodeEditorForm::set_sourceCode(QString source)
{
   ui->textEdit->setPlainText(source);
   ui->textEdit->setDocumentTitle(m_fileName);
}

void CodeEditorForm::selectLine(int linenumber)
{
   QTextCursor textCursor = ui->textEdit->textCursor();
   textCursor.movePosition(QTextCursor::Start);
   textCursor.movePosition(QTextCursor::Down,QTextCursor::MoveAnchor,linenumber-1);
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
