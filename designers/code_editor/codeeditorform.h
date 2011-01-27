#ifndef CODEEDITORFORM_H
#define CODEEDITORFORM_H

#include <QWidget>
#include <QTextCursor>

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

protected:
   void changeEvent(QEvent* e);
   void mouseDoubleClickEvent ( QMouseEvent* e );
   void contextMenuEvent(QContextMenuEvent* e);

private:
   Ui::CodeEditorForm* ui;
   QString m_fileName;
   int m_breakpointIndex;
   QTextCursor m_ctxtTextCursor;

signals:
   void breakpointsChanged();

private slots:
   void on_textEdit_cursorPositionChanged();
   void on_actionClear_marker_triggered();
   void on_actionEnd_marker_here_triggered();
   void on_actionStart_marker_here_triggered();
   void on_actionEnable_breakpoint_triggered();
   void on_actionRemove_breakpoint_triggered();
   void on_actionDisable_breakpoint_triggered();
   void on_actionRun_to_here_triggered();
   void on_actionBreak_on_CPU_execution_here_triggered();
   void on_textEdit_selectionChanged();
   void on_textEdit_textChanged();
   void repaintWithDecoration();
   void repaintWithoutDecoration();
};

#endif // CODEEDITORFORM_H
