#ifndef CODEEDITORFORM_H
#define CODEEDITORFORM_H

#include <QWidget>

namespace Ui
{
class CodeEditorForm;
}

class CodeEditorForm : public QWidget
{
   Q_OBJECT
public:
   CodeEditorForm(QWidget* parent = 0);
   ~CodeEditorForm();

   QString get_sourceCode();
   void set_sourceCode(QString source);
   void selectLine(int linenumber);

protected:
   void changeEvent(QEvent* e);
   void mouseMoveEvent ( QMouseEvent* e );
   void contextMenuEvent(QContextMenuEvent* e);

private:
   Ui::CodeEditorForm* ui;

private slots:
   void on_textEdit_selectionChanged();
   void on_textEdit_textChanged();
};

#endif // CODEEDITORFORM_H
