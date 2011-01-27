#ifndef CSYNTAXHIGHLIGHTEDTEXTEDIT_H
#define CSYNTAXHIGHLIGHTEDTEXTEDIT_H

#include <QPlainTextEdit>
#include <QObject>
#include <QPainter>
#include <QTextBlock>
#include <QScrollBar>
#include <QIcon>
#include <QListWidget>
#include <QStringList>

#include "csyntaxhighlighter.h"

class LineNumberArea;


class CSyntaxHighlightedTextEdit : public QPlainTextEdit
{
   Q_OBJECT

public:
   CSyntaxHighlightedTextEdit(QWidget* parent = 0);
   virtual ~CSyntaxHighlightedTextEdit();

   void lineNumberAreaPaintEvent(QPaintEvent* event);
   void lineNumberAreaMouseDoubleClickEvent(QMouseEvent* event);
   void lineNumberAreaMousePressEvent(QMouseEvent* event);
   void lineNumberAreaMouseReleaseEvent(QMouseEvent* event);
   void lineNumberAreaMouseMoveEvent(QMouseEvent* event);
   int lineNumberAreaWidth();
   void enableDecoration(bool enable) { decorationEnabled = enable; }

protected:
   void resizeEvent(QResizeEvent* event);
   void mouseMoveEvent(QMouseEvent* event);
   void mousePressEvent(QMouseEvent* event);
   void keyPressEvent(QKeyEvent* event);
   void focusOutEvent(QFocusEvent* event);

public slots:
   void highlightCurrentLine();

private slots:
   void updateLineNumberAreaWidth(int newBlockCount);
   void updateLineNumberArea(const QRect&, int);

private:
   QWidget* lineNumberArea;
   CSyntaxHighlighter* highlighter;
   QIcon*   executionIcon;
   QIcon*   breakEnabledIcon;
   QIcon*   breakDisabledIcon;
   QIcon*   breakEnabledHitIcon;
   QIcon*   breakDisabledHitIcon;
   bool     decorationEnabled;
   QListWidget* symbolListWidget;
   void updateToolTip(QString selectedText);
   int updateSymbolList(QString selectedText,QStringList* symbols);
};


class LineNumberArea : public QWidget
{
public:
   LineNumberArea(CSyntaxHighlightedTextEdit* editor) : QWidget(editor)
   {
      codeEditor = editor;
   }

   QSize sizeHint() const
   {
      return QSize(codeEditor->lineNumberAreaWidth(), 0);
   }

protected:
   void paintEvent(QPaintEvent* event)
   {
      codeEditor->lineNumberAreaPaintEvent(event);
   }
   
   void mouseDoubleClickEvent(QMouseEvent *e)
   {
      return codeEditor->lineNumberAreaMouseDoubleClickEvent(e);
   }
   
   void mousePressEvent(QMouseEvent *e)
   {
      return codeEditor->lineNumberAreaMousePressEvent(e);
   }
   
   void mouseReleaseEvent(QMouseEvent *e)
   {
      return codeEditor->lineNumberAreaMouseReleaseEvent(e);
   }
   
   void mouseMoveEvent(QMouseEvent *e)
   {
      return codeEditor->lineNumberAreaMouseMoveEvent(e);
   }

private:
   CSyntaxHighlightedTextEdit* codeEditor;
};


#endif // CSYNTAXHIGHLIGHTEDTEXTEDIT_H
