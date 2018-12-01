#ifndef CODEEDITORFORM_H
#define CODEEDITORFORM_H

#include "cdesignereditorbase.h"
#include "iprojecttreeviewitem.h"

#include "cbreakpointinfo.h"

#include <QLabel>

#include <Qsci/qsciscintilla.h>
#include <Qsci/qscistyle.h>
#include "Qsci/qsciapis.h"
#include "qscilexercc65.h"
#include "qscilexerca65.h"
#include "qsciapisca65.h"
#include "qscilexerdefault.h"

enum
{
   Margin_Decorations = 0,
   Margin_LineNumbers,
   Margin_Folding
};

enum
{
   Marker_Breakpoint = 0,
   Marker_BreakpointDisabled,
   Marker_Execution,
   Marker_Error,
   Marker_Highlight,
   Marker_Marker1,
   Marker_Marker2,
   Marker_Marker3,
   Marker_Marker4,
   Marker_Marker5,
   Marker_Marker6,
   Marker_Marker7,
   Marker_Marker8,
   Marker_MarkerMAX
};

enum
{
   Language_Default,
   Language_Assembly,
   Language_C
};

namespace Ui
{
class CodeEditorForm;
}

class CodeEditorForm : public CDesignerEditorBase
{
   Q_OBJECT
public:
   CodeEditorForm(QString fileName,QString sourceCode,IProjectTreeViewItem* link = 0,QWidget* parent = 0);
   virtual ~CodeEditorForm();

   QString sourceCode();
   void setSourceCode(QString source);
   void showExecutionLine(int linenumber);
   void highlightLine(int linenumber);
   QString fileName() { return m_fileName; }
   void setReadOnly(bool readOnly) { m_scintilla->setReadOnly(readOnly); }
   QMenu& editorMenu();

   virtual bool isModified() { return m_scintilla->isModified(); }
   virtual void setModified(bool modified) { m_scintilla->setModified(modified); }

   // ICenterWidgetItem Interface Implmentation
   virtual void onSave();

protected:
   void changeEvent(QEvent *event);
   bool eventFilter(QObject *obj, QEvent *event);
   void timerEvent(QTimerEvent *event);
   void showEvent(QShowEvent *event);
   void hideEvent(QHideEvent *event);
   void restyleText();
   void annotateText();

private:
   Ui::CodeEditorForm* ui;
   QLabel* info;
   QString m_fileName;
   CBreakpointInfo* m_pBreakpoints;
   QsciScintilla* m_scintilla;
   QsciLexer* m_lexer;
   QsciAPIsCA65* m_apis;
   int m_breakpointIndex;
   int m_contextMenuLine;
   int m_toolTipTimer;
   QString m_searchText;
   int m_language;
   void updateToolTip(QString symbol);

public slots:
   void resolveLineAddress(int line, int* addr, int* absAddr);
   void snapTo(QString item);
   void replaceText(QString from, QString to, bool replaceAll);
   void setBreakpoint(int line, int addr, int absAddr);
   void updateTargetMachine(QString target);
   void checkOpenFile(QDateTime lastActivationTime);
   void editor_undo();
   void editor_redo();
   void editor_cut();
   void editor_copy();
   void editor_paste();
   void editor_delete();
   void editor_selectAll();
   void editor_find();
   void editor_marginClicked(int margin,int line,Qt::KeyboardModifiers modifiers);
   void editor_linesChanged();
   void editor_modificationChanged(bool m);
   void editor_copyAvailable(bool yes);
   void editor_cursorPositionChanged(int line,int index);
   void external_breakpointsChanged();
   void compiler_compileStarted();
   void compiler_compileDone(bool ok);
   void emulator_emulatorStarted();
   void breakpointHit();
   void on_actionClear_marker_triggered();
   void on_actionEnd_marker_here_triggered();
   void on_actionStart_marker_here_triggered();
   void on_actionEnable_breakpoint_triggered();
   void on_actionRemove_breakpoint_triggered();
   void on_actionDisable_breakpoint_triggered();
   void on_actionRun_to_here_triggered();
   void on_actionBreak_on_CPU_execution_here_triggered();
   void customContextMenuRequested(const QPoint &pos);
   void applyProjectPropertiesToTab();
   void applyEnvironmentSettingsToTab();
   void applyAppSettingsToTab();
};

#endif // CODEEDITORFORM_H
