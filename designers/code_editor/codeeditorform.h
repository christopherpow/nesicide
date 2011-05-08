#ifndef CODEEDITORFORM_H
#define CODEEDITORFORM_H

#include <QWidget>
#include <QTextCursor>

#include <Qsci/qsciscintilla.h>
#include "qscilexerca65.h"

namespace Ui
{
class CodeEditorForm;
}

class CodeEditorForm : public QWidget
{
   Q_OBJECT
public:
   CodeEditorForm(QString fileName,QWidget* parent = 0);
   ~CodeEditorForm();

   QString get_sourceCode();
   void set_sourceCode(QString source);
   void selectLine(int linenumber);
   QString fileName() { return m_fileName; }

protected:
   void changeEvent(QEvent* e);
   void contextMenuEvent(QContextMenuEvent* e);

private:
   Ui::CodeEditorForm* ui;
   QString m_fileName;
   QsciScintilla* m_editor;
   QsciLexerCA65* m_lexer;
   int m_breakpointIndex;
   QTextCursor m_ctxtTextCursor;

signals:
   void breakpointsChanged();

private slots:
   void editor_marginClicked(int margin,int line,Qt::KeyboardModifiers modifiers);
   void editor_linesChanged();
   void external_breakpointsChanged();
   void breakpointHit();
   void on_actionClear_marker_triggered();
   void on_actionEnd_marker_here_triggered();
   void on_actionStart_marker_here_triggered();
   void on_actionEnable_breakpoint_triggered();
   void on_actionRemove_breakpoint_triggered();
   void on_actionDisable_breakpoint_triggered();
   void on_actionRun_to_here_triggered();
   void on_actionBreak_on_CPU_execution_here_triggered();
};

#endif // CODEEDITORFORM_H
