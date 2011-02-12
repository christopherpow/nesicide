#include "codeeditorform.h"
#include "ui_codeeditorform.h"

#include <QToolTip>
#include <QMenu>

#include "main.h"

#include "pasm_lib.h"

#include "cdockwidgetregistry.h"

#include "dbg_cnes6502.h"

#include "cbreakpointinfo.h"
#include "cmarker.h"

CodeEditorForm::CodeEditorForm(QString fileName,QWidget* parent) :
   QWidget(parent),
   ui(new Ui::CodeEditorForm)
{
   QDockWidget* codeBrowser = CDockWidgetRegistry::getWidget("Code Browser");
   QDockWidget* breakpoints = CDockWidgetRegistry::getWidget("Breakpoints");
   
   ui->setupUi(this);
   
   m_editor = new QsciScintilla();
#ifdef Q_WS_MAC
   m_editor->setFont(QFont("Monaco", 11));
#endif
#ifdef Q_WS_X11
   m_editor->setFont(QFont("Monospace", 10));
#endif
#ifdef Q_WS_WIN
   m_editor->setFont(QFont("Consolas", 11));
#endif
   m_editor->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
   m_editor->setMarginLineNumbers(1,true);
   m_editor->setMarginWidth(1,"01234");
   m_editor->setMarginType(1,QsciScintilla::NumberMargin);
   m_editor->setMarginSensitivity(1,true);
   m_editor->setMarginWidth(0,22);
   m_editor->setMarginType(0,QsciScintilla::SymbolMargin);
   m_editor->setMarginSensitivity(0,true);
   m_editor->setSelectionBackgroundColor(QColor(230,230,230));
   QObject::connect(m_editor,SIGNAL(marginClicked(int,int,Qt::KeyboardModifiers)),this,SLOT(editor_marginClicked(int,int,Qt::KeyboardModifiers)));
   
   ui->gridLayout->addWidget(m_editor);

//   QObject::connect(codeBrowser,SIGNAL(breakpointsChanged()),ui->textEdit, SLOT(repaint()) );
   
   // Connect signals to the UI to have the UI update.
//   QObject::connect ( emulator, SIGNAL(cartridgeLoaded()), this, SLOT(repaint()) );
//   QObject::connect ( emulator, SIGNAL(emulatorReset()), this, SLOT(repaint()) );
//   QObject::connect ( emulator, SIGNAL(emulatorPaused(bool)), this, SLOT(repaint()) );
//   QObject::connect ( emulator, SIGNAL(emulatorStarted()), this, SLOT(repaint()) );
//   QObject::connect ( breakpointWatcher, SIGNAL(breakpointHit()), this, SLOT(repaint()) );
   
//   QObject::connect ( this, SIGNAL(breakpointsChanged()), breakpoints, SIGNAL(breakpointsChanged()) );
   
//   QObject::connect ( breakpoints, SIGNAL(breakpointsChanged()), this, SLOT(repaint()) );

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

void CodeEditorForm::editor_marginClicked(int margin,int line,Qt::KeyboardModifiers modifiers)
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   int bp;
   int addr = 0;
   int absAddr = 0;
   
   addr = pasm_get_source_addr_by_linenum_and_file ( line+1, m_fileName.toAscii().constData() );

   absAddr = pasm_get_source_absolute_addr_by_linenum_and_file ( line+1, m_fileName.toAscii().constData() );

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
      
      m_breakpointIndex = bp;
   
      // If breakpoint isn't set here, give menu options to set one...
      if ( bp < 0 )
      {
         on_actionBreak_on_CPU_execution_here_triggered();
      }
      else
      {
         if ( pBreakpoints->GetStatus(bp) == Breakpoint_Disabled )
         {
            on_actionRemove_breakpoint_triggered();
         }
         else
         {
            on_actionDisable_breakpoint_triggered();
         }
      }
      
      emit breakpointsChanged();
   }
}

void CodeEditorForm::contextMenuEvent(QContextMenuEvent *e)
{
#if 0
   QMenu menu;
   QMenu *pMenu = ui->textEdit->createStandardContextMenu();
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
      menu.addAction(ui->actionRun_to_here);
      menu.addSeparator();
   
      // If breakpoint isn't set here, give menu options to set one...
      if ( bp < 0 )
      {
         menu.addAction(ui->actionBreak_on_CPU_execution_here);
      }
      else
      {
         if ( pBreakpoints->GetStatus(bp) == Breakpoint_Disabled )
         {
            menu.addAction(ui->actionEnable_breakpoint);
            menu.addAction(ui->actionRemove_breakpoint);
         }
         else
         {
            menu.addAction(ui->actionDisable_breakpoint);
            menu.addAction(ui->actionRemove_breakpoint);
         }
      }
   
      menu.addSeparator();
      menu.addAction(ui->actionClear_marker);
      menu.addSeparator();
   
      menu.addAction(ui->actionStart_marker_here);
      menu.addAction(ui->actionEnd_marker_here);   
      menu.addSeparator();
      
      menu.addActions(pMenu->actions());

      m_breakpointIndex = bp;
      
      m_ctxtTextCursor = textCursor;
   }
   else
   {      
      menu.addActions(pMenu->actions());
   }

   // Run the context menu...
   menu.exec(e->globalPos());
   
   delete pMenu;
#endif
}


void CodeEditorForm::on_actionBreak_on_CPU_execution_here_triggered()
{
#if 0
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
   
      emit breakpointsChanged();
   }
#endif
}

void CodeEditorForm::on_actionRun_to_here_triggered()
{
#if 0
   int addr = 0;
   int absAddr = 0;

   addr = pasm_get_source_addr_by_linenum_and_file ( m_ctxtTextCursor.blockNumber()+1, ui->textEdit->documentTitle().toAscii().constData() );

   absAddr = pasm_get_source_absolute_addr_by_linenum_and_file ( m_ctxtTextCursor.blockNumber()+1, ui->textEdit->documentTitle().toAscii().constData() );

   if ( addr != -1 )
   {
//      C6502DBG::GOTO(addr);
   }// CPTODO: fix the goto for absolute
#endif
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
#if 0
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
#endif
}

void CodeEditorForm::on_actionEnd_marker_here_triggered()
{
#if 0
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
#endif
}

void CodeEditorForm::on_actionClear_marker_triggered()
{
   CMarker* markers = nesGetExecutionMarkerDatabase();
   markers->ClearAllMarkers();
}

QString CodeEditorForm::get_sourceCode()
{
//   return ui->textEdit->toPlainText();
}

void CodeEditorForm::set_sourceCode(QString source)
{
   m_editor->setText(source);
//   ui->textEdit->setPlainText(source);
//   ui->textEdit->setDocumentTitle(m_fileName);
}

void CodeEditorForm::selectLine(int linenumber)
{
   m_editor->ensureLineVisible(linenumber-1);
   m_editor->setSelection(linenumber-1,0,linenumber-1,m_editor->lineLength(linenumber-1));
   
#if 0
   QTextCursor textCursor = ui->textEdit->textCursor();
   textCursor.movePosition(QTextCursor::Start);
   textCursor.movePosition(QTextCursor::Down,QTextCursor::MoveAnchor,linenumber-1);
   ui->textEdit->setTextCursor(textCursor);
   ui->textEdit->highlightCurrentLine();
#endif
}
