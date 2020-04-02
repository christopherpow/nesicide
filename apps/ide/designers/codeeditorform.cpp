#include "codeeditorform.h"
#include "ui_codeeditorform.h"

#include <QToolTip>
#include <QMenu>
#include <QAction>
#include <QPixmap>
#include <QStringList>
#include <QSettings>
#include <QDir>

#include "Qsci/qsciscintillabase.h"

#include "cobjectregistry.h"

#include "ccc65interface.h"

#include "cdockwidgetregistry.h"

#include "dbg_cnes6502.h"

#include "cbreakpointinfo.h"
#include "cmarker.h"

#include "cnesicideproject.h"
#include "csourceitem.h"

#include "nes_emulator_core.h"
#include "c64_emulator_core.h"

#include "environmentsettingsdialog.h"

static char toolTipText [ 2048 ];
static char annotationBuffer [ 2048 ];
static char resolutionBuffer [ 2048 ];

CodeEditorForm::CodeEditorForm(QString fileName,QString sourceCode,IProjectTreeViewItem* link,QWidget* parent) :
   CDesignerEditorBase(link,parent),
   ui(new Ui::CodeEditorForm),
   m_lexer(NULL),
   m_scintilla(NULL),
   m_apis(NULL)
{
   QDockWidget* codeBrowser = dynamic_cast<QDockWidget*>(CDockWidgetRegistry::instance()->getWidget("Assembly Browser"));
   QDockWidget* breakpoints = dynamic_cast<QDockWidget*>(CDockWidgetRegistry::instance()->getWidget("Breakpoints"));
   QDockWidget* symbolWatch = dynamic_cast<QDockWidget*>(CDockWidgetRegistry::instance()->getWidget("Symbol Inspector"));
   QDockWidget* executionVisualizer = dynamic_cast<QDockWidget*>(CDockWidgetRegistry::instance()->getWidget("Execution Visualizer"));
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");
   CMarker* markers = nesGetExecutionMarkerDatabase();
   MarkerSetInfo* pMarker;
   int marker;

   ui->setupUi(this);

   // Create the status-bar widget.
   info = new QLabel(this);

   m_pBreakpoints = NULL;

   m_scintilla = new QsciScintilla();

   setCentralWidget(m_scintilla);
   setWindowFlags(Qt::Widget);

   m_scintilla->setFrameShape(QFrame::NoFrame);

   m_fileName = fileName;
   m_searchText = "";

   m_scintilla->setAutoCompletionSource(QsciScintilla::AcsAll);

   m_scintilla->installEventFilter(this);
   m_scintilla->setContextMenuPolicy(Qt::CustomContextMenu);
   QObject::connect(m_scintilla,SIGNAL(customContextMenuRequested(const QPoint&)),this,SLOT(customContextMenuRequested(const QPoint&)));

   // Use a timer to do periodic checks for tooltips since mouse tracking doesn't seem to work.
   m_toolTipTimer = startTimer(50);

   m_scintilla->setAutoCompletionSource(QsciScintilla::AcsAll);
   m_scintilla->setAutoCompletionThreshold(2);
   m_scintilla->setAutoCompletionCaseSensitivity(false);
   m_scintilla->setAutoCompletionReplaceWord(false);
   m_scintilla->setAutoCompletionUseSingle(QsciScintilla::AcusNever);

   m_scintilla->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
   m_scintilla->setMarginWidth(4,0);
   m_scintilla->setMarginMarkerMask(4,0);

   m_scintilla->setMarginWidth(3,5);
   m_scintilla->setMarginType(3,QsciScintilla::SymbolMargin);
   m_scintilla->setMarginSensitivity(3,true);
   m_scintilla->setMarginMarkerMask(3,0x00000FF0);

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

   m_scintilla->setAnnotationDisplay ( QsciScintilla::AnnotationBoxed );

   applyEnvironmentSettingsToTab();

   // Connect signals from Scintilla to update the UI.
   QObject::connect(m_scintilla,SIGNAL(marginClicked(int,int,Qt::KeyboardModifiers)),this,SLOT(editor_marginClicked(int,int,Qt::KeyboardModifiers)));
   QObject::connect(m_scintilla,SIGNAL(linesChanged()),this,SLOT(editor_linesChanged()));
   QObject::connect(m_scintilla,SIGNAL(modificationChanged(bool)),this,SLOT(editor_modificationChanged(bool)));
   QObject::connect(m_scintilla,SIGNAL(copyAvailable(bool)),this,SLOT(editor_copyAvailable(bool)));
   QObject::connect(m_scintilla,SIGNAL(cursorPositionChanged(int,int)),this,SLOT(editor_cursorPositionChanged(int,int)));

   // Connect signals to the UI to have the UI update.
   QObject::connect ( codeBrowser,SIGNAL(breakpointsChanged()),this,SLOT(external_breakpointsChanged()) );
   QObject::connect ( codeBrowser, SIGNAL(snapTo(QString)),this, SLOT(snapTo(QString)) );
   QObject::connect ( this, SIGNAL(addWatchedItem(QString)), symbolWatch, SLOT(addWatchedItem(QString)) );
   QObject::connect ( this, SIGNAL(breakpointsChanged()), breakpoints, SIGNAL(breakpointsChanged()) );
   QObject::connect ( breakpoints, SIGNAL(breakpointsChanged()), this, SLOT(external_breakpointsChanged()) );

   if ( !CNesicideProject::instance()->getProjectTarget().compare("nes",Qt::CaseInsensitive) )
   {
      QObject::connect ( executionVisualizer, SIGNAL(breakpointsChanged()), this, SLOT(external_breakpointsChanged()) );
   }

   QObject* breakpointWatcher = CObjectRegistry::instance()->getObject("Breakpoint Watcher");
   QObject* emulator = CObjectRegistry::instance()->getObject("Emulator");
   QObject* compiler = CObjectRegistry::instance()->getObject("Compiler");

   QObject::connect ( compiler, SIGNAL(compileStarted()), this, SLOT(compiler_compileStarted()) );
   QObject::connect ( compiler, SIGNAL(compileDone(bool)), this, SLOT(compiler_compileDone(bool)) );
   QObject::connect ( breakpointWatcher, SIGNAL(breakpointHit()), this,SLOT(breakpointHit()) );
   if ( emulator )
   {
      QObject::connect ( emulator, SIGNAL(emulatorStarted()), this, SLOT(emulator_emulatorStarted()) );
   }

   // Finally set the text in the Scintilla object.
   setSourceCode(sourceCode);
}

CodeEditorForm::~CodeEditorForm()
{
   delete ui;

   if ( info->parent() == this )
   {
      delete info;
   }

   delete m_lexer;
   delete m_scintilla;
}

void CodeEditorForm::updateTargetMachine(QString /*target*/)
{
}

void CodeEditorForm::customContextMenuRequested(const QPoint &pos)
{
   const QString GO_TO_DEFINITION_TEXT = "Go to Definition of ";
   const QString ADD_TO_WATCH_TEXT = "Watch ";

   QMenu& menu = editorMenu();
   int bp;
   int addr = 0;
   int absAddr = 0;
   QsciDocument doc = m_scintilla->document();
   QString symbol = m_scintilla->wordAtPoint(pos);
   QAction* action;

   if ( !CNesicideProject::instance()->getProjectTarget().compare("nes",Qt::CaseInsensitive) )
   {
      m_pBreakpoints = nesGetBreakpointDatabase();
   }
   else if ( !CNesicideProject::instance()->getProjectTarget().compare("c64",Qt::CaseInsensitive) )
   {
      m_pBreakpoints = c64GetBreakpointDatabase();
   }

   m_contextMenuLine = m_scintilla->lineAt(pos);

   menu.addSeparator();

   if ( m_pBreakpoints )
   {
      resolveLineAddress(m_contextMenuLine,&addr,&absAddr);

      if ( addr != -1 )
      {
         bp = m_pBreakpoints->FindExactMatch ( eBreakOnCPUExecution,
                                             eBreakpointItemAddress,
                                             0,
                                             addr,
                                             absAddr,
                                             addr,
                                             0xFFFF,
                                             true,
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
            if ( m_pBreakpoints->GetStatus(bp) == Breakpoint_Disabled )
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
         menu.addAction(ui->actionStart_End_marker_here);
         menu.addSeparator();

         m_breakpointIndex = bp;
      }
   }

   if ( (!symbol.isEmpty()) &&
        (CCC65Interface::instance()->isStringASymbol(symbol)) )
   {
      QString str = GO_TO_DEFINITION_TEXT;
      str += symbol;
      menu.addAction(str);
      str = ADD_TO_WATCH_TEXT;
      str += symbol;
      menu.addAction(str);
   }

   // Run the context menu...
   action = menu.exec(QWidget::mapToGlobal(pos));

   if ( (action) && (action->text().startsWith(GO_TO_DEFINITION_TEXT)) )
   {
      QString file = CCC65Interface::instance()->getSourceFileFromSymbol(symbol);
      emit snapToTab("SourceNavigatorFile,"+file);
      emit snapToTab("SourceNavigatorSymbol,"+symbol);
   }
   else if ( (action) && (action->text().startsWith(ADD_TO_WATCH_TEXT)) )
   {
      emit addWatchedItem(symbol);
   }
}

void CodeEditorForm::onSave()
{
   // Force EOL conversion if desired.
   if ( EnvironmentSettingsDialog::eolForceConsistent() )
   {
      m_scintilla->convertEols((QsciScintilla::EolMode)EnvironmentSettingsDialog::eolMode());
   }

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
            emit activateSearchBar(m_scintilla->selectedText());
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

void CodeEditorForm::changeEvent(QEvent *event)
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

void CodeEditorForm::timerEvent(QTimerEvent *event)
{
   QString    symbol;

   if ( event->timerId() == m_toolTipTimer )
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

void CodeEditorForm::showEvent(QShowEvent */*event*/)
{
   emit addStatusBarWidget(info);
   info->show();
}

void CodeEditorForm::hideEvent(QHideEvent */*event*/)
{
   emit removeStatusBarWidget(info);
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
      if ( CCC65Interface::instance()->isErrorOnLineOfFile(m_fileName,line+1) )
      {
         m_scintilla->markerAdd(line,Marker_Error);
      }
   }

   if ( ok )
   {
      annotateText();
   }
}

void CodeEditorForm::emulator_emulatorStarted()
{
   m_scintilla->markerDeleteAll(Marker_Execution);
   m_scintilla->markerDeleteAll(Marker_Highlight);
}

void CodeEditorForm::external_breakpointsChanged()
{
   CMarker* markers = nesGetExecutionMarkerDatabase();
   MarkerSetInfo* pMarker;
   unsigned int addr;
   unsigned int absAddr;
   int line;
   int idx;
   int asmcount;
   int asmline;

   if ( !CNesicideProject::instance()->getProjectTarget().compare("nes",Qt::CaseInsensitive) )
   {
      m_pBreakpoints = nesGetBreakpointDatabase();
   }
   else if ( !CNesicideProject::instance()->getProjectTarget().compare("c64",Qt::CaseInsensitive) )
   {
      m_pBreakpoints = c64GetBreakpointDatabase();
   }

   m_scintilla->markerDeleteAll(Marker_Breakpoint);
   m_scintilla->markerDeleteAll(Marker_BreakpointDisabled);
   for ( idx = 0; idx < markers->GetNumMarkers(); idx++ )
   {
      m_scintilla->markerDeleteAll(Marker_Marker1+idx);
   }

   for ( line = 0; line < m_scintilla->lines(); line++ )
   {
      asmcount = CCC65Interface::instance()->getLineMatchCount(m_fileName,line+1);
      for ( asmline = 0; asmline < asmcount; asmline++ )
      {
         addr = CCC65Interface::instance()->getAddressFromFileAndLine(m_fileName,line+1,asmline);
         absAddr = CCC65Interface::instance()->getPhysicalAddressFromFileAndLine(m_fileName,line+1,asmline);

         if ( addr != (unsigned int)-1 )
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

            if ( m_pBreakpoints )
            {
               for ( idx = 0; idx < m_pBreakpoints->GetNumBreakpoints(); idx++ )
               {
                  BreakpointInfo* pBreakpoint = m_pBreakpoints->GetBreakpoint(idx);

                  if ( (pBreakpoint->enabled) &&
                       (pBreakpoint->type == eBreakOnCPUExecution) &&
                       (pBreakpoint->item1 <= addr) &&
                       ((absAddr == (unsigned int)-1) || (absAddr == pBreakpoint->item1Physical)) )
                  {
                     m_scintilla->markerAdd(line,Marker_Breakpoint);
                  }
                  else if ( (!pBreakpoint->enabled) &&
                            (pBreakpoint->type == eBreakOnCPUExecution) &&
                            (pBreakpoint->item1 <= addr) &&
                            ((absAddr == (unsigned int)-1) || (absAddr == pBreakpoint->item1Physical)) )
                  {
                     m_scintilla->markerAdd(line,Marker_BreakpointDisabled);
                  }
               }
            }
         }
      }
   }
}

void CodeEditorForm::breakpointHit()
{
}

void CodeEditorForm::editor_cursorPositionChanged(int line, int index)
{
   info->setText("Line: " + QString::number(line+1) + ", Col: " + QString::number(index+1));
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

void CodeEditorForm::editor_find()
{
   emit activateSearchBar(m_scintilla->selectedText());
}

void CodeEditorForm::editor_modificationChanged(bool m)
{
   // Clear annotations.
   if ( m )
   {
      m_scintilla->clearAnnotations();
   }

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

void CodeEditorForm::editor_marginClicked(int margin,int line,Qt::KeyboardModifiers /*modifiers*/)
{
   int bp;
   int addr = 0;
   int absAddr = 0;

   if ( !CNesicideProject::instance()->getProjectTarget().compare("nes",Qt::CaseInsensitive) )
   {
      m_pBreakpoints = nesGetBreakpointDatabase();
   }
   else if ( !CNesicideProject::instance()->getProjectTarget().compare("c64",Qt::CaseInsensitive) )
   {
      m_pBreakpoints = c64GetBreakpointDatabase();
   }

   m_scintilla->setCursorPosition(line,0);

   if ( margin < 2 )
   {
      if ( m_pBreakpoints )
      {
         resolveLineAddress(line,&addr,&absAddr);

         if ( addr != -1 )
         {
            bp = m_pBreakpoints->FindExactMatch ( eBreakOnCPUExecution,
                                                  eBreakpointItemAddress,
                                                  0,
                                                  addr,
                                                  absAddr,
                                                  addr,
                                                  0xFFFF,
                                                  true,
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
               if ( m_pBreakpoints->GetStatus(bp) == Breakpoint_Disabled )
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
      opcodeToolTipText = OPCODEINFO(symbol.toLatin1().constData());
      if ( opcodeToolTipText )
      {
         opcodeToolTipForm.sprintf("<pre>%s</pre>",opcodeToolTipText);
         setToolTip(opcodeToolTipForm.toLatin1().constData());
      }
   }

   // Next check for symbol tooltips.
   if ( EnvironmentSettingsDialog::showSymbolTips() )
   {
      // Look for C-language emitted symbols [prepended with underscore]...
      if ( (CCC65Interface::instance()->getSymbolType(clangSymbol) == CC65_SYM_LABEL) ||
           (CCC65Interface::instance()->getSymbolType(clangSymbol) == CC65_SYM_IMPORT) )
      {
         symbol = clangSymbol;
      }
      if ( (CCC65Interface::instance()->getSymbolType(symbol) == CC65_SYM_LABEL) ||
           (CCC65Interface::instance()->getSymbolType(symbol) == CC65_SYM_IMPORT) )
      {
         addr = CCC65Interface::instance()->getSymbolAddress(symbol);

         if ( addr != 0xFFFFFFFF )
         {
            absAddr = CCC65Interface::instance()->getSymbolPhysicalAddress(symbol);
            if ( !CNesicideProject::instance()->getProjectTarget().compare("nes",Qt::CaseInsensitive) )
            {
               nesGetPrintablePhysicalAddress(address,addr,absAddr);
            }
            else if ( !CNesicideProject::instance()->getProjectTarget().compare("c64",Qt::CaseInsensitive) )
            {
               c64GetPrintablePhysicalAddress(address,addr,absAddr);
            }

            file = CCC65Interface::instance()->getSourceFileFromSymbol(symbol);
            line = CCC65Interface::instance()->getSourceLineFromFileAndSymbol(file,symbol);

            if ( !CNesicideProject::instance()->getProjectTarget().compare("nes",Qt::CaseInsensitive) )
            {
               sprintf(toolTipText,TOOLTIP_LABEL,symbol.toLatin1().constData(),file.toLatin1().constData(),line,address,nesGetMemory(addr));
            }
            else if ( !CNesicideProject::instance()->getProjectTarget().compare("c64",Qt::CaseInsensitive) )
            {
               sprintf(toolTipText,TOOLTIP_LABEL,symbol.toLatin1().constData(),file.toLatin1().constData(),line,address,c64GetMemory(addr));
            }
            setToolTip(toolTipText);
         }
      }
      else if ( CCC65Interface::instance()->getSymbolType(symbol) == CC65_SYM_EQUATE )
      {
         addr = CCC65Interface::instance()->getSymbolAddress(symbol);

         sprintf(toolTipText,TOOLTIP_EQUATE,symbol.toLatin1().constData(),addr);
         setToolTip(toolTipText);
      }
   }
}

void CodeEditorForm::setBreakpoint(int line, int addr, int absAddr)
{
   int bpIdx;

   if ( !CNesicideProject::instance()->getProjectTarget().compare("nes",Qt::CaseInsensitive) )
   {
      m_pBreakpoints = nesGetBreakpointDatabase();
   }
   else if ( !CNesicideProject::instance()->getProjectTarget().compare("c64",Qt::CaseInsensitive) )
   {
      m_pBreakpoints = c64GetBreakpointDatabase();
   }

   if ( addr != -1 )
   {
      bpIdx = m_pBreakpoints->AddBreakpoint ( eBreakOnCPUExecution,
                                              eBreakpointItemAddress,
                                              0,
                                              addr,
                                              absAddr,
                                              addr,
                                              0xFFFF,
                                              true,
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
   int addr = 0;
   int absAddr = 0;

   resolveLineAddress(m_contextMenuLine,&addr,&absAddr);
   setBreakpoint(m_contextMenuLine,addr,absAddr);
}

void CodeEditorForm::on_actionRun_to_here_triggered()
{
#if 0
   int addr = 0;
   int absAddr = 0;

   addr = pasm_get_source_addr_by_linenum_and_file ( m_ctxtTextCursor.blockNumber()+1, ui->textEdit->documentTitle().toLatin1().constData() );

   absAddr = pasm_get_source_absolute_addr_by_linenum_and_file ( m_ctxtTextCursor.blockNumber()+1, ui->textEdit->documentTitle().toLatin1().constData() );

   if ( addr != -1 )
   {
      nesSetGotoAddress(addr);
   }// CPTODO: fix the goto for absolute
#endif
}

void CodeEditorForm::on_actionDisable_breakpoint_triggered()
{
   if ( m_breakpointIndex >= 0 )
   {
      m_pBreakpoints->ToggleEnabled(m_breakpointIndex);

      emit breakpointsChanged();
      emit markProjectDirty(true);
   }
}

void CodeEditorForm::on_actionRemove_breakpoint_triggered()
{
   if ( m_breakpointIndex >= 0 )
   {
      m_pBreakpoints->RemoveBreakpoint(m_breakpointIndex);

      emit breakpointsChanged();
      emit markProjectDirty(true);
   }
}

void CodeEditorForm::on_actionEnable_breakpoint_triggered()
{
   if ( m_breakpointIndex >= 0 )
   {
      m_pBreakpoints->ToggleEnabled(m_breakpointIndex);

      emit breakpointsChanged();
      emit markProjectDirty(true);
   }
}

void CodeEditorForm::on_actionStart_End_marker_here_triggered()
{
   CMarker* markers = nesGetExecutionMarkerDatabase();
   int marker = -1;
   int line, lineFrom, lineTo, indexFrom, indexTo;
   int addr = 0;
   int absAddr = 0;

   m_scintilla->getSelection(&lineFrom,&indexFrom,&lineTo,&indexTo);
   if ( lineFrom != -1 )
   {
      resolveLineAddress(lineFrom,&addr,&absAddr);

      if ( addr != -1 )
      {
         // Find unused Marker entry...
         marker = markers->AddMarker(addr,absAddr);

         emit breakpointsChanged();
         emit markProjectDirty(true);
      }
   }
   if ( lineTo != -1 )
   {
      if ( marker >= 0 )
      {
         resolveLineAddress(lineTo,&addr,&absAddr);

         if ( addr != -1 )
         {
            markers->CompleteMarker(marker,addr,absAddr);

            emit breakpointsChanged();
            emit markProjectDirty(true);
         }
      }
   }
}

void CodeEditorForm::on_actionStart_marker_here_triggered()
{
   CMarker* markers = nesGetExecutionMarkerDatabase();
   int line;
   int index;
   int addr = 0;
   int absAddr = 0;

   m_scintilla->getCursorPosition(&line,&index);

   resolveLineAddress(line,&addr,&absAddr);

   if ( addr != -1 )
   {
      // Find unused Marker entry...
      markers->AddMarker(addr,absAddr);

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
   markers->RemoveAllMarkers();

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

   // Force repaint of breakpoints since the reason this API is
   // called is usually when a CodeEditorForm is opened for the
   // first time or subsequent times after being closed.  Any
   // breakpoints set in this code module would otherwise disappear
   // on subsequent opens.  (They're still in the breakpoint database
   // they just wouldn't show up in the code editor).
   external_breakpointsChanged();

   // Force EOL conversion if desired.
   if ( EnvironmentSettingsDialog::eolForceConsistent() )
   {
      m_scintilla->convertEols((QsciScintilla::EolMode)EnvironmentSettingsDialog::eolMode());
   }

   // Force repaint of error tags.
   compiler_compileDone(true);

   // Setting the text of the Scintilla object unfortunately marks
   // it as "modified".  Reset our modified flag.
   setModified(false);
}

void CodeEditorForm::showExecutionLine(int linenumber)
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");
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

   asmcount = CCC65Interface::instance()->getLineMatchCount(m_fileName,line+1);
   if ( asmcount > 1 )
   {
      for ( asmline = 0; asmline < asmcount; asmline++ )
      {
         (*addr) = CCC65Interface::instance()->getAddressFromFileAndLine(m_fileName,line+1,asmline);
         (*absAddr) = CCC65Interface::instance()->getPhysicalAddressFromFileAndLine(m_fileName,line+1,asmline);

         if ( !CNesicideProject::instance()->getProjectTarget().compare("nes",Qt::CaseInsensitive) )
         {
            nesGetPrintablePhysicalAddress(resolutionBuffer,(*addr),(*absAddr));
            nesGetDisassemblyAtPhysicalAddress((*absAddr),resolutionBuffer);
         }
         else if ( !CNesicideProject::instance()->getProjectTarget().compare("c64",Qt::CaseInsensitive) )
         {
            c64GetPrintablePhysicalAddress(resolutionBuffer,(*addr),(*absAddr));
            c64GetDisassemblyAtPhysicalAddress((*absAddr),resolutionBuffer);
         }
         asmChunk = resolutionBuffer;
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
      (*addr) = CCC65Interface::instance()->getAddressFromFileAndLine(m_fileName,line+1);
      (*absAddr) = CCC65Interface::instance()->getPhysicalAddressFromFileAndLine(m_fileName,line+1);
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

         asmcount = CCC65Interface::instance()->getLineMatchCount(m_fileName,line+1);
         for ( asmline = 0; asmline < asmcount; asmline++ )
         {
            addr = CCC65Interface::instance()->getAddressFromFileAndLine(m_fileName,line+1,asmline);
            absAddr = CCC65Interface::instance()->getPhysicalAddressFromFileAndLine(m_fileName,line+1,asmline);
            endAddr = CCC65Interface::instance()->getEndAddressFromPhysicalAddress(addr,absAddr);

            if ( (addr != -1) && (absAddr != -1) && (endAddr != -1) )
            {
               if ( !firstBlock )
               {
                  pAnnotationBuffer += sprintf(pAnnotationBuffer,"\n...");
               }
               firstBlock = false;

               for ( ; addr <= endAddr; addr++, absAddr++ )
               {
                  if ( CCC65Interface::instance()->isPhysicalAddressAnOpcode(absAddr) )
                  {
                     if ( !CNesicideProject::instance()->getProjectTarget().compare("nes",Qt::CaseInsensitive) )
                     {
                        nesGetDisassemblyAtPhysicalAddress(absAddr,disassembly);
                     }
                     else if ( !CNesicideProject::instance()->getProjectTarget().compare("c64",Qt::CaseInsensitive) )
                     {
                        c64GetDisassemblyAtPhysicalAddress(absAddr,disassembly);
                     }
                     if ( disassembly[0] )
                     {
                        if ( !first )
                        {
                           pAnnotationBuffer += sprintf(pAnnotationBuffer,"\n");
                        }
                        first = false;

                        if ( !CNesicideProject::instance()->getProjectTarget().compare("nes",Qt::CaseInsensitive) )
                        {
                           nesGetPrintablePhysicalAddress(address,addr,absAddr);
                        }
                        else if ( !CNesicideProject::instance()->getProjectTarget().compare("c64",Qt::CaseInsensitive) )
                        {
                           c64GetPrintablePhysicalAddress(address,addr,absAddr);
                        }
                        pAnnotationBuffer += sprintf(pAnnotationBuffer,"%s:",address);
                        pAnnotationBuffer += sprintf(pAnnotationBuffer,disassembly);
                     }
                  }
               }
            }

            // Short circuit to prevent crash...
            if ( pAnnotationBuffer-annotationBuffer > 1536 )
            {
               pAnnotationBuffer += sprintf(pAnnotationBuffer,"\n...");
               break;
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
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");

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

   qDebug("from: %s to: %s",from.toLatin1().data(),to.toLatin1().data());
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
   bool     found;

   // Make sure item is something we care about
   if ( item.startsWith("Address,") )
   {
      splits = item.split(QRegExp("[,():]"),QString::SkipEmptyParts);
      if ( splits.count() == 2 )
      {
         addr = splits.at(1).toInt(NULL,16);
         absAddr = addr;
      }
      else if ( splits.count() == 3 )
      {
         addr = splits.at(2).toInt(NULL,16);
         absAddr = addr;
      }
      else if ( splits.count() == 4 )
      {
         addr = splits.at(3).toInt(NULL,16);
         absAddr = (splits.at(1).toInt(NULL,16)*MEM_8KB)+splits.at(2).toInt(NULL,16);
      }
      fileName = QDir::cleanPath(CCC65Interface::instance()->getSourceFileFromPhysicalAddress(addr,absAddr));
      if ( fileName == m_fileName )
      {
         line = CCC65Interface::instance()->getSourceLineFromPhysicalAddress(addr,absAddr);
         highlightLine(line);
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
            found = m_scintilla->findFirst(splits.at(4),splits.at(2).toInt(),splits.at(1).toInt(),false,true,splits.at(3).toInt(),line,index);

            emit setSearchBarHint(found);
         }
         else
         {
            found = m_scintilla->findNext();

            emit setSearchBarHint(found);
         }
      }
   }
   else if ( item.startsWith("SourceNavigatorFile,") )
   {
      splits = item.split(QRegExp("[,]"));
      QString cleanPath = QDir::cleanPath(splits.at(1));
      if ( m_fileName.endsWith(cleanPath,Qt::CaseInsensitive) )
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
      QString cleanPath = QDir::cleanPath(splits.at(1));
      if ( cleanPath == m_fileName )
      {
         if ( splits.count() == 3 )
         {
            highlightLine(splits.at(2).toInt());
            editor_cursorPositionChanged(splits.at(2).toInt()-1,0);
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
      line = CCC65Interface::instance()->getSourceLineFromFileAndSymbol(m_fileName,splits.at(1));
      if ( line >= 0 )
      {
         highlightLine(line);
      }
   }
}

QMenu& CodeEditorForm::editorMenu()
{
   QAction* action;
   bool writable = !m_scintilla->isReadOnly();
   bool undoable = m_scintilla->isUndoAvailable();
   bool redoable = m_scintilla->isRedoAvailable();
   bool pasteable = m_scintilla->SendScintilla(QsciScintilla::SCI_CANPASTE, (unsigned long)0, (long)0);

   m_menu.clear();
   action = m_menu.addAction("Undo",this,SLOT(editor_undo()),QKeySequence(Qt::CTRL + Qt::Key_Z));
   action->setEnabled(writable && undoable);
   action = m_menu.addAction("Redo",this,SLOT(editor_redo()),QKeySequence(Qt::CTRL + Qt::Key_Y));
   action->setEnabled(writable && redoable);
   m_menu.addSeparator();
   action = m_menu.addAction("Cut",this,SLOT(editor_cut()),QKeySequence(Qt::CTRL + Qt::Key_X));
   action->setEnabled(writable && m_scintilla->hasSelectedText());
   action = m_menu.addAction("Copy",this,SLOT(editor_copy()),QKeySequence(Qt::CTRL + Qt::Key_C));
   action->setEnabled(m_scintilla->hasSelectedText());
   action = m_menu.addAction("Paste",this,SLOT(editor_paste()),QKeySequence(Qt::CTRL + Qt::Key_V));
   action->setEnabled(writable && pasteable);
//   action = menu.addAction("Delete",this,SLOT(editor_delete()));
//   action->setEnabled(writable && m_scintilla->hasSelectedText());
   m_menu.addSeparator();
   action = m_menu.addAction("Select All",this,SLOT(editor_selectAll()),QKeySequence(Qt::CTRL + Qt::Key_A));
   m_menu.addSeparator();
   action = m_menu.addAction("Find",this,SLOT(editor_find()),QKeySequence(Qt::CTRL + Qt::Key_F));

   return m_menu;
}

void CodeEditorForm::applyProjectPropertiesToTab()
{
}

void CodeEditorForm::applyAppSettingsToTab()
{
}

void CodeEditorForm::applyEnvironmentSettingsToTab()
{
   QSettings settings(QSettings::IniFormat, QSettings::UserScope, "CSPSoftware", "NESICIDE");
   int op;

   if ( EnvironmentSettingsDialog::foldSource() )
   {
      m_scintilla->setMarginType(Margin_Folding,QsciScintilla::SymbolMargin);
      m_scintilla->setMarginSensitivity(Margin_Folding,true);
      m_scintilla->setFolding(QsciScintilla::BoxedTreeFoldStyle,Margin_Folding);
   }
   else
   {
      m_scintilla->setMarginWidth(Margin_Folding,0);
      m_scintilla->setFolding(QsciScintilla::NoFoldStyle,Margin_Folding);
   }

   m_scintilla->setUtf8(EnvironmentSettingsDialog::textEncodingIsUtf8());

   m_scintilla->setEolMode((QsciScintilla::EolMode)EnvironmentSettingsDialog::eolMode());

   m_scintilla->setMarginsBackgroundColor(EnvironmentSettingsDialog::marginBackgroundColor());
   m_scintilla->setMarginsForegroundColor(EnvironmentSettingsDialog::marginForegroundColor());

   if ( EnvironmentSettingsDialog::highlightBarEnabled() )
   {
      m_scintilla->setMarkerBackgroundColor(EnvironmentSettingsDialog::highlightBarColor(),Marker_Highlight);
   }
   else
   {
      // Set the highlight bar color to background to hide it =]
      m_scintilla->setMarkerBackgroundColor(m_lexer->defaultPaper(),Marker_Highlight);
   }

   if ( m_lexer == NULL )
   {
      m_language = Language_Default;
      m_scintilla->setLexer();

      foreach ( QString ext, EnvironmentSettingsDialog::highlightAsC().split(" ") )
      {
         if ( m_fileName.endsWith(ext,Qt::CaseInsensitive) )
         {
            m_language = Language_C;
         }
      }
      if ( m_language == Language_Default )
      {
         foreach ( QString ext, EnvironmentSettingsDialog::highlightAsASM().split(" ") )
         {
            if ( m_fileName.endsWith(ext,Qt::CaseInsensitive) )
            {
               m_language = Language_Assembly;
            }
         }
      }

      if ( m_language == Language_C )
      {
         m_lexer = new QsciLexerCC65(m_scintilla);
      }
      else if ( m_language == Language_Assembly )
      {
         m_lexer = new QsciLexerCA65(m_scintilla);
         m_apis = new QsciAPIsCA65(m_lexer);

         QStringList opcodes;
         for ( op = 0; op < 256; op++ )
         {
            QString opcodeTooltipText = OPCODEINFO(op);
            opcodeTooltipText = opcodeTooltipText.split(":")[0];
            if ( !(opcodeTooltipText.isEmpty() || opcodes.contains(opcodeTooltipText)) )
            {
               opcodes += opcodeTooltipText;
            }
         }
         foreach ( QString opcode, opcodes )
         {
            m_apis->add(opcode);
         }
         m_apis->prepare();
      }
      else
      {
         m_lexer = new QsciLexerDefault(m_scintilla);
      }
   }
   m_scintilla->setLexer(m_lexer);

   m_lexer->readSettings(settings,"CodeEditor");

   m_scintilla->setMarginsFont(m_lexer->font(QsciLexerCA65::CA65_Default));

   m_scintilla->setCaretForegroundColor(EnvironmentSettingsDialog::caretColor());

   restyleText();
   annotateText();
}

void CodeEditorForm::checkOpenFile(QDateTime lastActivationTime)
{
   QFileInfo fileInfo;
#if QT_VERSION >= 0x040700
   QDateTime now = QDateTime::currentDateTimeUtc();
#else
   QDateTime now = QDateTime::currentDateTime();
#endif
   QString str;
   int result;

   // Check whether the current open project file has changed.
   if ( lastActivationTime.isValid() )
   {
      fileInfo.setFile(m_fileName);
      if ( fileInfo.lastModified() > lastActivationTime )
      {
         str += m_fileName;
         str += "\n\nhas been modified outside of NESICIDE.\n\n";
         str += "Do you want to re-load it?";
         result = QMessageBox::warning(this,"External interference detected!",str,QMessageBox::Yes,QMessageBox::No);

         if ( result == QMessageBox::Yes )
         {
            QFile fileIn(m_fileName);
            if ( fileIn.open(QIODevice::ReadOnly) )
            {
               QByteArray fileData = fileIn.readAll();
               setSourceCode(fileData);
            }
         }
      }
   }
}
