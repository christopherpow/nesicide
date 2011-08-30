#include "codeeditorform.h"
#include "ui_codeeditorform.h"

#include <QToolTip>
#include <QMenu>
#include <QAction>
#include <QPixmap>

#include "Qsci/qsciscintillabase.h"

#include "main.h"

#include "ccc65interface.h"

#include "cdockwidgetregistry.h"

#include "dbg_cnes6502.h"

#include "cbreakpointinfo.h"
#include "cmarker.h"

static char toolTipText [ 2048 ];
static char annotationBuffer [ 2048 ];
static char resolutionBuffer [ 2048 ];

CodeEditorForm::CodeEditorForm(QString fileName,QString sourceCode,IProjectTreeViewItem* link,QWidget* parent) :
   CDesignerEditorBase(link,parent),
   ui(new Ui::CodeEditorForm)
{
   QDockWidget* codeBrowser = dynamic_cast<QDockWidget*>(CDockWidgetRegistry::getWidget("Assembly Browser"));
   QDockWidget* breakpoints = dynamic_cast<QDockWidget*>(CDockWidgetRegistry::getWidget("Breakpoints"));
   QDockWidget* symbolWatch = dynamic_cast<QDockWidget*>(CDockWidgetRegistry::getWidget("Symbol Inspector"));
   QSettings settings;
   CMarker* markers = nesGetExecutionMarkerDatabase();
   MarkerSetInfo* pMarker;
   int marker;

   ui->setupUi(this);

   m_scintilla = new QsciScintilla();

   m_scintilla->setFrameShape(QFrame::NoFrame);

   m_fileName = fileName;
   m_searchText = "";
   m_language = Language_Default;

   foreach ( QString ext, EnvironmentSettingsDialog::sourceExtensionsForC().split(" ") )
   {
      if ( m_fileName.endsWith(ext,Qt::CaseInsensitive) )
      {
         m_language = Language_C;
      }
   }
   if ( m_language == Language_Default )
   {
      foreach ( QString ext, EnvironmentSettingsDialog::sourceExtensionsForAssembly().split(" ") )
      {
         if ( m_fileName.endsWith(ext,Qt::CaseInsensitive) )
         {
            m_language = Language_Assembly;
         }
      }
   }

   if ( m_language == Language_C )
   {
      m_lexer = new QsciLexerCPP(m_scintilla);

      m_scintilla->setLexer(m_lexer);
   }
   else if ( m_language == Language_Assembly )
   {
      m_lexer = new QsciLexerCA65(m_scintilla);

      m_scintilla->setLexer(m_lexer);
   }
   else
   {
      m_lexer = new QsciLexerDefault(m_scintilla);

      m_scintilla->setLexer(m_lexer);
   }

   m_lexer->readSettings(settings,"CodeEditor");

   m_scintilla->installEventFilter(this);
   m_scintilla->setContextMenuPolicy(Qt::CustomContextMenu);
   QObject::connect(m_scintilla,SIGNAL(customContextMenuRequested(const QPoint&)),this,SLOT(customContextMenuRequested(const QPoint&)));

   // Use a timer to do periodic checks for tooltips since mouse tracking doesn't seem to work.
   m_timer = startTimer(50);

   m_scintilla->setMarginsBackgroundColor(EnvironmentSettingsDialog::marginBackgroundColor());
   m_scintilla->setMarginsForegroundColor(EnvironmentSettingsDialog::marginForegroundColor());
   m_scintilla->setMarginsFont(m_lexer->font(QsciLexerCA65::CA65_Default));

   m_scintilla->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
   m_scintilla->setMarginWidth(3,0);
   m_scintilla->setMarginMarkerMask(3,0);
   m_scintilla->setMarginWidth(4,0);
   m_scintilla->setMarginMarkerMask(4,0);

   m_scintilla->setMarginWidth(2,5);
   m_scintilla->setMarginMarkerMask(2,QsciScintilla::SymbolMargin);
   m_scintilla->setMarginSensitivity(2,true);
   m_scintilla->setMarginMarkerMask(2,0x00000FF0);

   m_scintilla->setMarginWidth(Margin_Decorations,22);
   m_scintilla->setMarginMarkerMask(Margin_Decorations,0x0000000F);
   m_scintilla->setMarginType(Margin_Decorations,QsciScintilla::SymbolMargin);
   m_scintilla->setMarginSensitivity(Margin_Decorations,true);

   m_scintilla->setMarginLineNumbers(Margin_LineNumbers,true);
   m_scintilla->setMarginWidth(Margin_LineNumbers,0);
   m_scintilla->setMarginMarkerMask(Margin_LineNumbers,0x00000000);
   m_scintilla->setMarginType(Margin_LineNumbers,QsciScintilla::NumberMargin);
   m_scintilla->setMarginSensitivity(Margin_LineNumbers,true);

   m_scintilla->setSelectionBackgroundColor(QColor(215,215,215));
   m_scintilla->setSelectionToEol(true);

   m_scintilla->markerDefine(QPixmap(":/resources/22_execution_pointer.png"),Marker_Execution);
   m_scintilla->markerDefine(QPixmap(":/resources/22_breakpoint.png"),Marker_Breakpoint);
   m_scintilla->markerDefine(QPixmap(":/resources/22_breakpoint_disabled.png"),Marker_BreakpointDisabled);
   m_scintilla->markerDefine(QPixmap(":/resources/error-mark.png"),Marker_Error);
   for ( marker = 0; marker < markers->GetNumMarkers(); marker++ )
   {
      pMarker = markers->GetMarker(marker);
      m_scintilla->markerDefine(QsciScintilla::FullRectangle,Marker_Marker1+marker);
      m_scintilla->setMarkerBackgroundColor(QColor(pMarker->red,pMarker->green,pMarker->blue),Marker_Marker1+marker);
      m_scintilla->setMarkerForegroundColor(QColor(pMarker->red,pMarker->green,pMarker->blue),Marker_Marker1+marker);
   }
   m_scintilla->setMarkerForegroundColor(QColor(255,255,0),Marker_Error);
   m_scintilla->setMarkerBackgroundColor(QColor(255,0,0),Marker_Error);
   m_scintilla->markerDefine(QsciScintilla::Background,Marker_Highlight);
   if ( EnvironmentSettingsDialog::highlightBarEnabled() )
   {
      m_scintilla->setMarkerBackgroundColor(EnvironmentSettingsDialog::highlightBarColor(),Marker_Highlight);
   }
   else
   {
      // Set the highlight bar color to background to hide it =]
      m_scintilla->setMarkerBackgroundColor(m_lexer->defaultPaper(),Marker_Highlight);
   }

   m_scintilla->setAutoIndent( EnvironmentSettingsDialog::autoIndent() );
   m_scintilla->setTabWidth( EnvironmentSettingsDialog::spacesPerTab() );
   m_scintilla->setIndentationsUseTabs( !EnvironmentSettingsDialog::replaceTabs() );

   m_scintilla->setAnnotationDisplay ( QsciScintilla::AnnotationBoxed );

   // Connect signals from Scintilla to update the UI.
   QObject::connect(m_scintilla,SIGNAL(marginClicked(int,int,Qt::KeyboardModifiers)),this,SLOT(editor_marginClicked(int,int,Qt::KeyboardModifiers)));
   QObject::connect(m_scintilla,SIGNAL(linesChanged()),this,SLOT(editor_linesChanged()));
   QObject::connect(m_scintilla,SIGNAL(modificationChanged(bool)),this,SLOT(editor_modificationChanged(bool)));
   QObject::connect(m_scintilla,SIGNAL(copyAvailable(bool)),this,SLOT(editor_copyAvailable(bool)));

   ui->gridLayout->addWidget(m_scintilla);

   // Connect signals to the UI to have the UI update.
   QObject::connect ( codeBrowser,SIGNAL(breakpointsChanged()),this,SLOT(external_breakpointsChanged()) );
   QObject::connect ( breakpointWatcher, SIGNAL(breakpointHit()), this,SLOT(breakpointHit()) );
   QObject::connect ( this, SIGNAL(addWatchedItem(QString)), symbolWatch, SLOT(addWatchedItem(QString)) );
   QObject::connect ( this, SIGNAL(breakpointsChanged()), breakpoints, SIGNAL(breakpointsChanged()) );
   QObject::connect ( breakpoints, SIGNAL(breakpointsChanged()), this, SLOT(external_breakpointsChanged()) );
   QObject::connect ( compiler, SIGNAL(compileStarted()), this, SLOT(compiler_compileStarted()) );
   QObject::connect ( compiler, SIGNAL(compileDone(bool)), this, SLOT(compiler_compileDone(bool)) );
   QObject::connect ( emulator, SIGNAL(emulatorStarted()), this, SLOT(emulator_emulatorStarted()) );

   // Finally set the text in the Scintilla object.
   setSourceCode(sourceCode);
}

CodeEditorForm::~CodeEditorForm()
{
   delete ui;

   delete m_lexer;
   delete m_scintilla;
}

void CodeEditorForm::customContextMenuRequested(const QPoint &pos)
{
   const QString GO_TO_DEFINITION_TEXT = "Go to Definition...";
   const QString ADD_TO_WATCH_TEXT = "Watch...";

   QMenu menu;
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   int bp;
   int line = m_scintilla->lineAt(pos);
   int addr = 0;
   int absAddr = 0;
   QsciDocument doc = m_scintilla->document();
   bool writable = m_scintilla->SendScintilla(QsciScintilla::SCI_GETREADONLY, (unsigned long)0, (long)0);
   bool undoable = m_scintilla->SendScintilla(QsciScintilla::SCI_CANUNDO, (unsigned long)0, (long)0);
   bool redoable = m_scintilla->SendScintilla(QsciScintilla::SCI_CANREDO, (unsigned long)0, (long)0);
   bool pasteable = m_scintilla->SendScintilla(QsciScintilla::SCI_CANPASTE, (unsigned long)0, (long)0);
   QString symbol = m_scintilla->wordAtPoint(pos);

   m_scintilla->setCursorPosition(line,0);

   QAction* action;
   action = menu.addAction("Undo",this,SLOT(editor_undo()),QKeySequence(Qt::CTRL + Qt::Key_Z));
   action->setEnabled(writable && undoable);
   action = menu.addAction("Redo",this,SLOT(editor_redo()),QKeySequence(Qt::CTRL + Qt::Key_Y));
   action->setEnabled(writable && redoable);
   menu.addSeparator();
   action = menu.addAction("Cut",this,SLOT(editor_cut()),QKeySequence(Qt::CTRL + Qt::Key_X));
   action->setEnabled(writable && m_scintilla->hasSelectedText());
   action = menu.addAction("Copy",this,SLOT(editor_copy()),QKeySequence(Qt::CTRL + Qt::Key_C));
   action->setEnabled(m_scintilla->hasSelectedText());
   action = menu.addAction("Paste",this,SLOT(editor_paste()),QKeySequence(Qt::CTRL + Qt::Key_V));
   action->setEnabled(writable && pasteable);
//   action = menu.addAction("Delete",this,SLOT(editor_delete()));
//   action->setEnabled(writable && m_scintilla->hasSelectedText());
   menu.addSeparator();
   action = menu.addAction("Select All",this,SLOT(editor_selectAll()),QKeySequence(Qt::CTRL + Qt::Key_A));

   resolveLineAddress(line,&addr,&absAddr);

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

      m_breakpointIndex = bp;
   }

   if ( (!symbol.isEmpty()) &&
        (CCC65Interface::isStringASymbol(symbol)) )
   {
      menu.addAction(GO_TO_DEFINITION_TEXT);
      menu.addAction(ADD_TO_WATCH_TEXT);
   }

   // Run the context menu...
   action = menu.exec(QWidget::mapToGlobal(pos));

   if ( (action) && (action->text() == GO_TO_DEFINITION_TEXT) )
   {
      QString file = CCC65Interface::getSourceFileFromSymbol(symbol);
      emit snapToTab("SourceNavigatorFile,"+file);
      emit snapToTab("SourceNavigatorSymbol,"+symbol);
   }
   else if ( (action) && (action->text() == ADD_TO_WATCH_TEXT) )
   {
      emit addWatchedItem(symbol);
   }
}

void CodeEditorForm::onSave()
{
   if ( treeLink() )
   {
      // This editor is paired with a project item, use the normal
      // project mechanics to do the saving.
      CDesignerEditorBase::onSave();
   }
   else
   {
      // This editor does not live in the project, create a temporary
      // unlinked project item and do the save through it.
      QDir dir = QDir::currentPath();
      CSourceItem sourceItem(NULL);

      sourceItem.setEditor(this);
      sourceItem.setName(dir.fromNativeSeparators(dir.relativeFilePath(m_fileName)));
      sourceItem.setPath(dir.fromNativeSeparators(dir.relativeFilePath(m_fileName)));
      sourceItem.serializeContent();
      // Be sure to clear the editor otherwise the object gets deleted when
      // the destructor for this temporary is called.
      sourceItem.setEditor(NULL);
   }
}

bool CodeEditorForm::eventFilter(QObject *obj, QEvent *event)
{
   if (obj == m_scintilla)
   {
      // Capture Ctrl-S keypress since it's otherwise not handled.
      if (event->type() == QEvent::KeyPress)
      {
         QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
         if ( (keyEvent->modifiers() == Qt::ControlModifier) &&
              (keyEvent->key() == Qt::Key_S) )
         {
            onSave();
            return true;
         }
         else if ( (keyEvent->modifiers() == Qt::ControlModifier) &&
                   (keyEvent->key() == Qt::Key_F) )
         {
            emit activateSearchBar();
            return true;
         }
         else
         {
            return false;
         }
      }
      else
      {
         return false;
      }
   }
   else
   {
      // pass the event on to the parent class
      return CDesignerEditorBase::eventFilter(obj, event);
   }
}

void CodeEditorForm::changeEvent(QEvent *e)
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

void CodeEditorForm::timerEvent(QTimerEvent *e)
{
   QString    symbol;

   if ( e->timerId() == m_timer )
   {
      // Figure out if there's anything useful we can ToolTip.
      symbol = m_scintilla->wordAtPoint(mapFromGlobal(QCursor::pos()));

      if ( !symbol.isEmpty() )
      {
         updateToolTip(symbol);
      }
      else
      {
         setToolTip("");
      }
   }
}

void CodeEditorForm::compiler_compileStarted()
{
   m_scintilla->markerDeleteAll(Marker_Error);

   m_scintilla->clearAnnotations();
}

void CodeEditorForm::compiler_compileDone(bool ok)
{
   int line;

   for ( line = 0; line < m_scintilla->lines(); line++ )
   {
      if ( CCC65Interface::isErrorOnLineOfFile(m_fileName,line+1) )
      {
         m_scintilla->markerAdd(line,Marker_Error);
      }
   }

   annotateText();
}

void CodeEditorForm::emulator_emulatorStarted()
{
   m_scintilla->markerDeleteAll(Marker_Execution);
   m_scintilla->markerDeleteAll(Marker_Highlight);
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
   int asmcount;
   int asmline;

   m_scintilla->getCursorPosition(&line,&index);

   m_scintilla->markerDeleteAll(Marker_Breakpoint);
   m_scintilla->markerDeleteAll(Marker_BreakpointDisabled);
   for ( idx = 0; idx < markers->GetNumMarkers(); idx++ )
   {
      m_scintilla->markerDeleteAll(Marker_Marker1+idx);
   }

   for ( line = 0; line < m_scintilla->lines(); line++ )
   {
      asmcount = CCC65Interface::getLineMatchCount(m_fileName,line+1);
      for ( asmline = 0; asmline < asmcount; asmline++ )
      {
         addr = CCC65Interface::getAddressFromFileAndLine(m_fileName,line+1,asmline);
         absAddr = CCC65Interface::getAbsoluteAddressFromFileAndLine(m_fileName,line+1,asmline);

         if ( addr != -1 )
         {
            for ( idx = 0; idx < markers->GetNumMarkers(); idx++ )
            {
               pMarker = markers->GetMarker(idx);

               if ( (pMarker->state == eMarkerSet_Started) ||
                    (pMarker->state == eMarkerSet_Complete) )
               {
                  if ( (absAddr >= pMarker->startAbsAddr) &&
                       (absAddr <= pMarker->endAbsAddr) )
                  {
                     m_scintilla->markerAdd(line,Marker_Marker1+idx);
                  }
               }
            }

            for ( idx = 0; idx < pBreakpoints->GetNumBreakpoints(); idx++ )
            {
               BreakpointInfo* pBreakpoint = pBreakpoints->GetBreakpoint(idx);

               if ( (pBreakpoint->enabled) &&
                    (pBreakpoint->type == eBreakOnCPUExecution) &&
                    (pBreakpoint->item1 <= addr) &&
                    ((absAddr == -1) || (absAddr == pBreakpoint->item1Absolute)) )
               {
                  m_scintilla->markerAdd(line,Marker_Breakpoint);
               }
               else if ( (!pBreakpoint->enabled) &&
                         (pBreakpoint->type == eBreakOnCPUExecution) &&
                         (pBreakpoint->item1 <= addr) &&
                         ((absAddr == -1) || (absAddr == pBreakpoint->item1Absolute)) )
               {
                  m_scintilla->markerAdd(line,Marker_BreakpointDisabled);
               }
            }
         }
      }
   }
}

void CodeEditorForm::breakpointHit()
{
}

void CodeEditorForm::editor_undo()
{
   m_scintilla->SendScintilla(QsciScintilla::SCI_UNDO, (unsigned long)0, (long)0);
}

void CodeEditorForm::editor_redo()
{
   m_scintilla->SendScintilla(QsciScintilla::SCI_REDO, (unsigned long)0, (long)0);
}

void CodeEditorForm::editor_cut()
{
   m_scintilla->SendScintilla(QsciScintilla::SCI_CUT, (unsigned long)0, (long)0);
}

void CodeEditorForm::editor_copy()
{
   m_scintilla->SendScintilla(QsciScintilla::SCI_COPY, (unsigned long)0, (long)0);
}

void CodeEditorForm::editor_paste()
{
   m_scintilla->SendScintilla(QsciScintilla::SCI_PASTE, (unsigned long)0, (long)0);
}

void CodeEditorForm::editor_delete()
{
   m_scintilla->SendScintilla(QsciScintilla::SCI_CLEAR, (unsigned long)0, (long)0);
}

void CodeEditorForm::editor_selectAll()
{
   m_scintilla->SendScintilla(QsciScintilla::SCI_SELECTALL, (unsigned long)0, (long)0);
}

void CodeEditorForm::editor_modificationChanged(bool m)
{
   emit editor_modified(m);
}

void CodeEditorForm::editor_linesChanged()
{
   if ( EnvironmentSettingsDialog::lineNumbersEnabled() )
   {
      QString maxLineNum;

      maxLineNum.sprintf("%d",m_scintilla->lines());

      m_scintilla->setMarginWidth(Margin_LineNumbers,maxLineNum);
   }
   else
   {
      m_scintilla->setMarginWidth(Margin_LineNumbers,0);
   }
}

void CodeEditorForm::editor_marginClicked(int margin,int line,Qt::KeyboardModifiers modifiers)
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   int bp;
   int addr = 0;
   int absAddr = 0;

   m_scintilla->setCursorPosition(line,0);

   resolveLineAddress(line,&addr,&absAddr);

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
         setBreakpoint(line,addr,absAddr);
      }
      else
      {
         if ( pBreakpoints->GetStatus(bp) == Breakpoint_Disabled )
         {
            on_actionRemove_breakpoint_triggered();

            m_scintilla->markerDelete(line,Marker_BreakpointDisabled);
         }
         else
         {
            on_actionDisable_breakpoint_triggered();

            m_scintilla->markerDelete(line,Marker_Breakpoint);
            m_scintilla->markerAdd(line,Marker_BreakpointDisabled);
         }
      }

      emit breakpointsChanged();
      emit markProjectDirty(true);
   }
}

void CodeEditorForm::editor_copyAvailable(bool yes)
{
   QString    symbol;
   int lineFrom;
   int lineTo;
   int indexFrom;
   int indexTo;

   setToolTip("");

   if ( yes )
   {
      // Figure out if there's anything useful we can ToolTip.
      m_scintilla->getSelection(&lineFrom,&indexFrom,&lineTo,&indexTo);

      // Only do this if only some characters of a single line are selected.
      if ( lineFrom == lineTo )
      {
         symbol = m_scintilla->selectedText();

         updateToolTip(symbol);
      }
   }
}

void CodeEditorForm::updateToolTip(QString symbol)
{
   const char* TOOLTIP_LABEL = "<b>%s</b><br>Location: %s(%d)<br>Address: %s<br>Value: %02X";
   const char* TOOLTIP_EQUATE = "<b>%s</b>(%X)";
   const char* opcodeToolTipText;
   char        address[32];
   QString     opcodeToolTipForm;
   unsigned int addr;
   unsigned int absAddr;
   QString      file;
   int          line;
   QString clangSymbol = "_"+symbol;

   setToolTip("");

   // First check for opcode tooltips.
   if ( EnvironmentSettingsDialog::showOpcodeTips() )
   {
      opcodeToolTipText = OPCODEINFO(symbol.toAscii().constData());
      if ( opcodeToolTipText )
      {
         opcodeToolTipForm.sprintf("<pre>%s</pre>",opcodeToolTipText);
         setToolTip(opcodeToolTipForm.toAscii().constData());
      }
   }

   // Next check for symbol tooltips.
   if ( EnvironmentSettingsDialog::showSymbolTips() )
   {
      if ( (CCC65Interface::getSymbolType(symbol) == CC65_SYM_LABEL) ||
           (CCC65Interface::getSymbolType(symbol) == CC65_SYM_IMPORT) )
      {
         addr = CCC65Interface::getSymbolAddress(symbol);

         if ( addr != 0xFFFFFFFF )
         {
            absAddr = CCC65Interface::getSymbolAbsoluteAddress(symbol);
            nesGetPrintableAddressWithAbsolute(address,addr,absAddr);

            file = CCC65Interface::getSourceFileFromSymbol(symbol);
            line = CCC65Interface::getSourceLineFromFileAndSymbol(file,symbol);

            sprintf(toolTipText,TOOLTIP_LABEL,symbol.toAscii().constData(),file.toAscii().constData(),line,address,nesGetMemory(addr));
            setToolTip(toolTipText);
         }
         else
         {
            addr = CCC65Interface::getSymbolAddress(clangSymbol);

            if ( addr != 0xFFFFFFFF )
            {
               absAddr = CCC65Interface::getSymbolAbsoluteAddress(clangSymbol);
               nesGetPrintableAddressWithAbsolute(address,addr,absAddr);

               file = CCC65Interface::getSourceFileFromSymbol(symbol);
               line = CCC65Interface::getSourceLineFromFileAndSymbol(file,symbol);

               sprintf(toolTipText,TOOLTIP_LABEL,symbol.toAscii().constData(),file.toAscii().constData(),line,address,nesGetMemory(addr));
               setToolTip(toolTipText);
            }
         }
      }
      else if ( CCC65Interface::getSymbolType(symbol) == CC65_SYM_EQUATE )
      {
         addr = CCC65Interface::getSymbolAddress(symbol);

         sprintf(toolTipText,TOOLTIP_EQUATE,symbol.toAscii().constData(),addr);
         setToolTip(toolTipText);
      }
   }
}

void CodeEditorForm::setBreakpoint(int line, int addr, int absAddr)
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   int bpIdx;
   int index;

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
         m_scintilla->markerAdd(line,Marker_Breakpoint);

         emit breakpointsChanged();
         emit markProjectDirty(true);
      }
   }
}

void CodeEditorForm::on_actionBreak_on_CPU_execution_here_triggered()
{
   int line;
   int index;
   int addr = 0;
   int absAddr = 0;

   m_scintilla->getCursorPosition(&line,&index);

   resolveLineAddress(line,&addr,&absAddr);

   setBreakpoint(line,addr,absAddr);
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
      nesSetGotoAddress(addr);
   }// CPTODO: fix the goto for absolute
#endif
}

void CodeEditorForm::on_actionDisable_breakpoint_triggered()
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();

   if ( m_breakpointIndex >= 0 )
   {
      pBreakpoints->ToggleEnabled(m_breakpointIndex);

      emit breakpointsChanged();
      emit markProjectDirty(true);
   }
}

void CodeEditorForm::on_actionRemove_breakpoint_triggered()
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();

   if ( m_breakpointIndex >= 0 )
   {
      pBreakpoints->RemoveBreakpoint(m_breakpointIndex);

      emit breakpointsChanged();
      emit markProjectDirty(true);
   }
}

void CodeEditorForm::on_actionEnable_breakpoint_triggered()
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();

   if ( m_breakpointIndex >= 0 )
   {
      pBreakpoints->ToggleEnabled(m_breakpointIndex);

      emit breakpointsChanged();
      emit markProjectDirty(true);
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

   m_scintilla->getCursorPosition(&line,&index);

   resolveLineAddress(line,&addr,&absAddr);

   if ( addr != -1 )
   {
      // Find unused Marker entry...
      marker = markers->AddMarker(addr,absAddr);

      emit breakpointsChanged();
      emit markProjectDirty(true);
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
      m_scintilla->getCursorPosition(&line,&index);

      resolveLineAddress(line,&addr,&absAddr);

      if ( addr != -1 )
      {
         markers->CompleteMarker(marker,addr,absAddr);

         emit breakpointsChanged();
         emit markProjectDirty(true);
      }
   }
}

void CodeEditorForm::on_actionClear_marker_triggered()
{
   CMarker* markers = nesGetExecutionMarkerDatabase();
   markers->ClearAllMarkers();

   emit breakpointsChanged();
   emit markProjectDirty(true);
}

QString CodeEditorForm::sourceCode()
{
   return m_scintilla->text();
}

void CodeEditorForm::setSourceCode(QString source)
{
   m_scintilla->setText(source);

   // Force re-do of annotations.
   annotateText();

   // Force repaint of breakpoints since the reason this API is
   // called is usually when a CodeEditorForm is opened for the
   // first time or subsequent times after being closed.  Any
   // breakpoints set in this code module would otherwise disappear
   // on subsequent opens.  (They're still in the breakpoint database
   // they just wouldn't show up in the code editor).
   external_breakpointsChanged();

   // Force repaint of error tags.
   compiler_compileDone(true);

   // Setting the text of the Scintilla object unfortunately marks
   // it as "modified".  Reset our modified flag.
   setModified(false);
}

void CodeEditorForm::showExecutionLine(int linenumber)
{
   QSettings settings;
   bool      follow = EnvironmentSettingsDialog::followExecution();

   if ( m_scintilla )
   {
      m_scintilla->markerDeleteAll(Marker_Execution);
      m_scintilla->markerDeleteAll(Marker_Highlight);
      if ( (follow) && (linenumber >= 0) )
      {
         m_scintilla->ensureLineVisible(linenumber-1);
         m_scintilla->markerAdd(linenumber-1,Marker_Execution);
         m_scintilla->markerAdd(linenumber-1,Marker_Highlight);
      }
   }
}

void CodeEditorForm::highlightLine(int linenumber)
{
   if ( m_scintilla )
   {
      m_scintilla->markerDeleteAll(Marker_Highlight);
      if ( linenumber >= 0 )
      {
         m_scintilla->ensureLineVisible(linenumber-1);
         m_scintilla->markerAdd(linenumber-1,Marker_Highlight);
      }
   }
}

void CodeEditorForm::resolveLineAddress(int line, int *addr, int *absAddr)
{
   QStringList asmChunks;
   QString asmChunk;
   QList<int> asmAddrs;
   QList<int> asmAbsAddrs;
   QString selStr;
   int selIdx;
   int asmcount;
   int asmline;
   bool ok;

   asmcount = CCC65Interface::getLineMatchCount(m_fileName,line+1);
   if ( asmcount > 1 )
   {
      for ( asmline = 0; asmline < asmcount; asmline++ )
      {
         (*addr) = CCC65Interface::getAddressFromFileAndLine(m_fileName,line+1,asmline);
         (*absAddr) = CCC65Interface::getAbsoluteAddressFromFileAndLine(m_fileName,line+1,asmline);

         nesGetPrintableAddressWithAbsolute(resolutionBuffer,(*addr),(*absAddr));
         asmChunk = resolutionBuffer;
         nesGetDisassemblyAtAbsoluteAddress((*absAddr),resolutionBuffer);
         asmChunk += ":";
         asmChunk += resolutionBuffer;
         asmChunks.append(asmChunk);
         asmAddrs.append((*addr));
         asmAbsAddrs.append((*absAddr));
      }
      selStr = QInputDialog::getItem(0,"Help!","Line has multiple possible matches, pick one:",asmChunks,0,false,&ok);

      if ( !ok )
      {
         (*addr) = -1;
         (*absAddr) = -1;
         return;
      }
      for ( selIdx = 0; selIdx < asmcount; selIdx++ )
      {
         if ( asmChunks.at(selIdx) == selStr )
         {
            break;
         }
      }
      (*addr) = asmAddrs.at(selIdx);
      (*absAddr) = asmAbsAddrs.at(selIdx);
   }
   else
   {
      (*addr) = CCC65Interface::getAddressFromFileAndLine(m_fileName,line+1);
      (*absAddr) = CCC65Interface::getAbsoluteAddressFromFileAndLine(m_fileName,line+1);
   }
}

void CodeEditorForm::annotateText()
{
   int line;
   int addr;
   int absAddr;
   int endAddr;
   char disassembly[32];
   char address[32];
   char* pAnnotationBuffer;
   bool first;
   bool firstBlock;
   int asmcount;
   int asmline;

   // Clear annotations.
   m_scintilla->clearAnnotations();

   // Annotate C-language source with assembly if desired.
   if ( (EnvironmentSettingsDialog::annotateSource()) && (m_language == Language_C) )
   {
      for ( line = 0; line < m_scintilla->lines(); line++ )
      {
         annotationBuffer[0] = 0;
         pAnnotationBuffer = annotationBuffer;
         first = true;
         firstBlock = true;

         asmcount = CCC65Interface::getLineMatchCount(m_fileName,line+1);
         for ( asmline = 0; asmline < asmcount; asmline++ )
         {
            addr = CCC65Interface::getAddressFromFileAndLine(m_fileName,line+1,asmline);
            absAddr = CCC65Interface::getAbsoluteAddressFromFileAndLine(m_fileName,line+1,asmline);
            endAddr = CCC65Interface::getEndAddressFromAbsoluteAddress(addr,absAddr);

            if ( (addr != -1) && (absAddr != -1) && (endAddr != -1) )
            {
               if ( !firstBlock )
               {
                  pAnnotationBuffer += sprintf(pAnnotationBuffer,"\n...");
               }
               firstBlock = false;

               for ( ; addr <= endAddr; addr++, absAddr++ )
               {
                  if ( CCC65Interface::isAbsoluteAddressAnOpcode(absAddr) )
                  {
                     nesGetDisassemblyAtAbsoluteAddress(absAddr,disassembly);
                     if ( disassembly[0] )
                     {
                        if ( !first )
                        {
                           pAnnotationBuffer += sprintf(pAnnotationBuffer,"\n");
                        }
                        first = false;

                        nesGetPrintableAddressWithAbsolute(address,addr,absAddr);
                        pAnnotationBuffer += sprintf(pAnnotationBuffer,"%s:",address);
                        pAnnotationBuffer += sprintf(pAnnotationBuffer,disassembly);
                     }
                  }
               }
            }
         }

         (*pAnnotationBuffer) = 0;

         if ( annotationBuffer[0] )
         {
            m_scintilla->annotate(line,annotationBuffer,0);
         }
      }
   }
}

void CodeEditorForm::restyleText()
{
   QSettings settings;

   m_lexer->readSettings(settings,"CodeEditor");

   m_scintilla->setAutoIndent( EnvironmentSettingsDialog::autoIndent() );
   m_scintilla->setTabWidth( EnvironmentSettingsDialog::spacesPerTab() );
   m_scintilla->setIndentationsUseTabs( !EnvironmentSettingsDialog::replaceTabs() );
}

void CodeEditorForm::replaceText(QString from, QString to, bool replaceAll)
{
   QStringList splits;
   int      lineStart;
   int      indexStart;
   int      lineEnd;
   int      indexEnd;
   int      line;
   int      index;
   bool     found = false;

   if ( from.startsWith("SearchBar,") )
   {
      if ( isVisible() )
      {
         m_scintilla->getSelection(&lineStart,&indexStart,&lineEnd,&indexEnd);
         m_scintilla->setSelection(-1,-1,-1,-1);

         splits = from.split(QRegExp("[,]"));
         if ( splits.at(3) == "0" )
         {
            line = lineEnd;
            index = indexEnd;
         }
         else
         {
            line = lineStart;
            index = indexStart;
         }

         if ( (from != m_searchText) ||
              (splits.at(4) != m_scintilla->selectedText()) )
         {
            m_searchText = from; // Capture entire search configuration.
            found = m_scintilla->findFirst(splits.at(4),splits.at(2).toInt(),splits.at(1).toInt(),false,true,splits.at(3).toInt(),line,index);
         }
         else if ( (from == m_searchText) &&
                   (splits.at(4) != m_scintilla->selectedText()) )
         {
            found = m_scintilla->findNext();
         }
         else if ( from == m_searchText )
         {
            found = true;
         }

         do
         {
            if ( found )
            {
               m_scintilla->replaceSelectedText(to);
            }
            found = m_scintilla->findNext();
         } while ( replaceAll && found );
      }
   }
}

void CodeEditorForm::snapTo(QString item)
{
   QStringList splits;
   QString  fileName;
   uint32_t addr;
   uint32_t absAddr;
   int      line;
   int      index;
   int      throwAway1;
   int      throwAway2;

   // Make sure item is something we care about
   if ( item.startsWith("Address,") )
   {
      splits = item.split(QRegExp("[,()]"));
      if ( splits.count() == 5 )
      {
         addr = splits.at(3).toInt(NULL,16);
         absAddr = (splits.at(1).toInt(NULL,16)*MEM_8KB)+splits.at(2).toInt(NULL,16);

         fileName = CCC65Interface::getSourceFileFromAbsoluteAddress(addr,absAddr);
         if ( fileName == m_fileName )
         {
            line = CCC65Interface::getSourceLineFromAbsoluteAddress(addr,absAddr);
            highlightLine(line);
         }
      }
   }
   else if ( item.startsWith("SearchBar,") )
   {
      if ( isVisible() )
      {
         splits = item.split(QRegExp("[,]"));
         if ( splits.at(3).toInt() )
         {
            // If searching forward, search from the beginning of the selection.
            m_scintilla->getSelection(&line,&index,&throwAway1,&throwAway2);
         }
         else
         {
            // If searching backward, search from the end of the selection.
            m_scintilla->getSelection(&throwAway1,&throwAway2,&line,&index);
            index += 1;
         }
         if ( item != m_searchText )
         {
            m_searchText = item; // Capture entire search configuration.
            m_scintilla->findFirst(splits.at(4),splits.at(2).toInt(),splits.at(1).toInt(),false,true,splits.at(3).toInt(),line,index);
//            virtual bool findFirst(const QString &expr, bool re, bool cs, bool wo,
//                    bool wrap, bool forward = true, int line = -1, int index = -1,
//                    bool show = true);
         }
         else
         {
            m_scintilla->findNext();
         }
      }
   }
   else if ( item.startsWith("SourceNavigatorFile,") )
   {
      splits = item.split(QRegExp("[,]"));
      if ( splits.at(1) == m_fileName )
      {
         if ( splits.count() == 3 )
         {
            showExecutionLine(splits.at(2).toInt());
         }
         else
         {
            showExecutionLine(-1);
         }
      }
      else
      {
         showExecutionLine(-1);
      }
   }
   else if ( item.startsWith("OutputPaneFile,") )
   {
      splits = item.split(QRegExp("[,]"));
      if ( splits.at(1) == m_fileName )
      {
         if ( splits.count() == 3 )
         {
            highlightLine(splits.at(2).toInt());
         }
         else
         {
            highlightLine(-1);
         }
      }
      else
      {
         highlightLine(-1);
      }
   }
   else if ( item.startsWith("SourceNavigatorSymbol,") )
   {
      splits = item.split(QRegExp("[,]"));
      line = CCC65Interface::getSourceLineFromFileAndSymbol(m_fileName,splits.at(1));
      if ( line >= 0 )
      {
         highlightLine(line);
      }
   }
}
