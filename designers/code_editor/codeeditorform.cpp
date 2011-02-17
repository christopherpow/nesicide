#include "codeeditorform.h"
#include "ui_codeeditorform.h"

#include <QToolTip>
#include <QMenu>
#include <QPixmap>

#include "main.h"

#include "pasm_lib.h"

#include "cdockwidgetregistry.h"

#include "dbg_cnes6502.h"

#include "cbreakpointinfo.h"
#include "cmarker.h"

enum
{
   Margin_Decorations = 0,
   Margin_LineNumbers
};

enum
{
   Marker_Breakpoint = 0,
   Marker_BreakpointDisabled,
   Marker_Execution
};

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

   m_editor->setMarginWidth(2,0);
   m_editor->setMarginMarkerMask(2,0);
   m_editor->setMarginWidth(3,0);
   m_editor->setMarginMarkerMask(3,0);
   m_editor->setMarginWidth(4,0);
   m_editor->setMarginMarkerMask(4,0);

   m_editor->setMarginWidth(Margin_Decorations,22);
   m_editor->setMarginMarkerMask(Margin_Decorations,0xFFFFFF);
   m_editor->setMarginType(Margin_Decorations,QsciScintilla::SymbolMargin);
   m_editor->setMarginSensitivity(Margin_Decorations,true);

   m_editor->setMarginLineNumbers(Margin_LineNumbers,true);
   m_editor->setMarginWidth(Margin_LineNumbers,0);
   m_editor->setMarginMarkerMask(Margin_LineNumbers,0);
   m_editor->setMarginType(Margin_LineNumbers,QsciScintilla::NumberMargin);
   m_editor->setMarginSensitivity(Margin_LineNumbers,true);
   
   m_editor->setSelectionBackgroundColor(QColor(230,230,230));
   m_editor->setSelectionToEol(true);
   
   m_editor->markerDefine(QPixmap(":/resources/22_execution_pointer.png"),Marker_Execution);
   m_editor->markerDefine(QPixmap(":/resources/22_breakpoint.png"),Marker_Breakpoint);
   m_editor->markerDefine(QPixmap(":/resources/22_breakpoint_disabled.png"),Marker_BreakpointDisabled);
   
   QObject::connect(m_editor,SIGNAL(marginClicked(int,int,Qt::KeyboardModifiers)),this,SLOT(editor_marginClicked(int,int,Qt::KeyboardModifiers)));
   QObject::connect(m_editor,SIGNAL(linesChanged()),this,SLOT(editor_linesChanged()));
   
   ui->gridLayout->addWidget(m_editor);

   QObject::connect(codeBrowser,SIGNAL(breakpointsChanged()),this,SLOT(external_breakpointsChanged()) );
   
   // Connect signals to the UI to have the UI update.
//   QObject::connect ( emulator, SIGNAL(cartridgeLoaded()), this, SLOT(repaint()) );
//   QObject::connect ( emulator, SIGNAL(emulatorReset()), this, SLOT(repaint()) );
//   QObject::connect ( emulator, SIGNAL(emulatorPaused(bool)), this, SLOT(repaint()) );
//   QObject::connect ( emulator, SIGNAL(emulatorStarted()), this, SLOT(repaint()) );
   QObject::connect ( breakpointWatcher, SIGNAL(breakpointHit()), this,SLOT(breakpointHit()) );
   
   QObject::connect ( this, SIGNAL(breakpointsChanged()), breakpoints, SIGNAL(breakpointsChanged()) );
   
   QObject::connect ( breakpoints, SIGNAL(breakpointsChanged()), this, SLOT(repaint()) );

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

void CodeEditorForm::external_breakpointsChanged()
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   CMarker* markers = nesGetExecutionMarkerDatabase();
   MarkerSetInfo* pMarker;
   int addr;
   int absAddr;
   int line;
   int index;
   int idx;
   
   m_editor->getCursorPosition(&line,&index);
   
   m_editor->markerDeleteAll(Marker_Breakpoint);
   m_editor->markerDeleteAll(Marker_BreakpointDisabled);

   for ( line = 0; line < m_editor->lines(); line++ )
   {
      addr = pasm_get_source_addr_by_linenum_and_file(line+1,m_fileName.toAscii().constData());
      
      absAddr = pasm_get_source_absolute_addr_by_linenum_and_file(line+1,m_fileName.toAscii().constData());
      
      for ( idx = 0; idx < markers->GetNumMarkers(); idx++ )
      {
         pMarker = markers->GetMarker(idx);
         
         if ( (pMarker->state == eMarkerSet_Started) ||
              (pMarker->state == eMarkerSet_Complete) )
         {
            if ( (absAddr >= pMarker->startAbsAddr) &&
                 (absAddr <= pMarker->endAbsAddr) )
            {
            }
         }
      }
      
      for ( idx = 0; idx < pBreakpoints->GetNumBreakpoints(); idx++ )
      {
         BreakpointInfo* pBreakpoint = pBreakpoints->GetBreakpoint(idx);
      
         if ( (pBreakpoint->enabled) &&
              (pBreakpoint->type == eBreakOnCPUExecution) &&
              (pBreakpoint->item1 <= addr) &&
              ((absAddr == -1) || (absAddr == pBreakpoint->item1Absolute)) &&
              (pBreakpoint->item2 >= addr) )
         {
            m_editor->markerAdd(line,Marker_Breakpoint);
         }
         else if ( (!pBreakpoint->enabled) &&
                   (pBreakpoint->type == eBreakOnCPUExecution) &&
                   (pBreakpoint->item1 <= addr) &&
                   ((absAddr == -1) || (absAddr == pBreakpoint->item1Absolute)) &&
                   (pBreakpoint->item2 >= addr) )
         {
            m_editor->markerAdd(line,Marker_BreakpointDisabled);
         }
      }
   }
}

void CodeEditorForm::breakpointHit()
{
   
}

void CodeEditorForm::editor_linesChanged()
{
   QString maxLineNum;
   
   maxLineNum.sprintf("%d",m_editor->lines());
   
   m_editor->setMarginWidth(Margin_LineNumbers,maxLineNum);
}

void CodeEditorForm::editor_marginClicked(int margin,int line,Qt::KeyboardModifiers modifiers)
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   int bp;
   int addr = 0;
   int absAddr = 0;
   
   m_editor->setCursorPosition(line,0);
   
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
            
            m_editor->markerDelete(line,Marker_BreakpointDisabled);
         }
         else
         {
            on_actionDisable_breakpoint_triggered();
            
            m_editor->markerDelete(line,Marker_Breakpoint);
            m_editor->markerAdd(line,Marker_BreakpointDisabled);
         }
      }
      
      emit breakpointsChanged();
   }
}

void CodeEditorForm::contextMenuEvent(QContextMenuEvent *e)
{
   QMenu menu;
//   QMenu *pMenu = m_editor->createStandardContextMenu();
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   int bp;
   int line;
   int index;
   int addr = 0;
   int absAddr = 0;

   m_editor->getCursorPosition(&line,&index);
   
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
      
//      menu.addActions(pMenu->actions());

      m_breakpointIndex = bp;
   }
   else
   {      
//      menu.addActions(pMenu->actions());
   }

   // Run the context menu...
   menu.exec(e->globalPos());
   
//   delete pMenu;
}


void CodeEditorForm::on_actionBreak_on_CPU_execution_here_triggered()
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   int bpIdx;
   int line;
   int index;
   int addr = 0;
   int absAddr = 0;
   
   m_editor->getCursorPosition(&line,&index);

   addr = pasm_get_source_addr_by_linenum_and_file ( line+1, m_fileName.toAscii().constData() );

   absAddr = pasm_get_source_absolute_addr_by_linenum_and_file ( line+1, m_fileName.toAscii().constData() );

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
      else
      {
         m_editor->markerAdd(line,Marker_Breakpoint);
      }
   
      emit breakpointsChanged();
   }
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
   CMarker* markers = nesGetExecutionMarkerDatabase();
   int marker;
   int line;
   int index;
   int addr = 0;
   int absAddr = 0;

   m_editor->getCursorPosition(&line,&index);

   addr = pasm_get_source_addr_by_linenum_and_file ( line+1, m_fileName.toAscii().constData() );

   absAddr = pasm_get_source_absolute_addr_by_linenum_and_file ( line+1, m_fileName.toAscii().constData() );

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
   int line;
   int index;
   int addr = 0;
   int absAddr = 0;

   if ( marker >= 0 )
   {   
      m_editor->getCursorPosition(&line,&index);
   
      addr = pasm_get_source_addr_by_linenum_and_file ( line+1, m_fileName.toAscii().constData() );
   
      absAddr = pasm_get_source_absolute_addr_by_linenum_and_file ( line+1, m_fileName.toAscii().constData() );

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
   return m_editor->text();
}

void CodeEditorForm::set_sourceCode(QString source)
{
   m_editor->setText(source);
}

void CodeEditorForm::selectLine(int linenumber)
{
   m_editor->markerDeleteAll(Marker_Execution);
   if ( linenumber >= 0 )
   {
      m_editor->ensureLineVisible(linenumber-1);
      m_editor->setSelection(linenumber-1,0,linenumber-1,m_editor->lineLength(linenumber-1));
      m_editor->markerAdd(linenumber-1,Marker_Execution);
   }
}
