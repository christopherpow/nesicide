#include "csyntaxhighlightedtextedit.h"

#include "pasm_lib.h"

#include "cbreakpointinfo.h"
#include "cmarker.h"

#include "dbg_cnes6502.h"

#include <QCoreApplication>
#include <QLayout>

CSyntaxHighlightedTextEdit::CSyntaxHighlightedTextEdit(QWidget*)
{
#ifdef Q_WS_MAC
   this->setFont(QFont("Monaco", 11));
#endif
#ifdef Q_WS_X11
   this->setFont(QFont("Monospace", 10));
#endif
#ifdef Q_WS_WIN
   this->setFont(QFont("Consolas", 11));
#endif
   this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
   this->setWordWrapMode(QTextOption::NoWrap);
   lineNumberArea = new LineNumberArea(this);

   connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
   connect(this, SIGNAL(updateRequest(const QRect&, int)), this, SLOT(updateLineNumberArea(const QRect&, int)));
   connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

   highlighter = new CSyntaxHighlighter(this->document());
      
   executionIcon = new QIcon(":/resources/22_execution_pointer.png");
   breakEnabledIcon = new QIcon(":/resources/22_breakpoint.png");
   breakDisabledIcon = new QIcon(":/resources/22_breakpoint_disabled.png");
   breakEnabledHitIcon = new QIcon(":/resources/22_execution_break.png");
   breakDisabledHitIcon = new QIcon(":/resources/22_execution_break_disabled.png");
   
   updateLineNumberAreaWidth(0);
   highlightCurrentLine();
   
   decorationEnabled = true;
   
   symbolListWidget = new QListWidget(this);
   symbolListWidget->setFont(font());
   symbolListWidget->hide();
}

CSyntaxHighlightedTextEdit::~CSyntaxHighlightedTextEdit()
{
   delete highlighter;
   delete lineNumberArea;
   delete executionIcon;
   delete breakEnabledIcon;
   delete breakDisabledIcon;
   delete breakEnabledHitIcon;
   delete breakDisabledHitIcon;
   delete symbolListWidget;
}

void CSyntaxHighlightedTextEdit::keyPressEvent(QKeyEvent* event)
{
   QTextCursor cursor = textCursor();
   QStringList symbolList;
   QString     selection;
   QPoint      symbolListPos;
   int         picked;

   if ( ((event->key() > Qt::Key_Space) && 
        (event->key() <= Qt::Key_AsciiTilde)) ||
        (event->key() == Qt::Key_Delete) ||
        (event->key() == Qt::Key_Backspace) )
   {
      QPlainTextEdit::keyPressEvent(event);
   
      cursor.select(QTextCursor::WordUnderCursor);
      
      selection = cursor.selectedText();
   
      if ( !(selection.isEmpty()) )
      {
         picked = updateSymbolList(selection,&symbolList);
         
         if ( symbolList.count() > 0 )
         {
            symbolListWidget->clear();
            symbolListWidget->addItems(symbolList);
            symbolListWidget->setFixedSize(100,(fontMetrics().height()*symbolList.count()));
            symbolListPos = pos()+cursorRect().bottomLeft();
            symbolListPos.setX(symbolListPos.x()+lineNumberAreaWidth());
            symbolListPos.setX(symbolListPos.x()-fontMetrics().width(selection));
            symbolListWidget->move(symbolListPos);
            symbolListWidget->showNormal();
            symbolListWidget->setCurrentRow(picked);
            symbolListWidget->repaint();
         }
         else
         {
            symbolListWidget->hide();
         }
      }
      else
      {
         symbolListWidget->hide();
      }
   }
   else if ( (event->key() == Qt::Key_Up) || 
             (event->key() == Qt::Key_PageUp) )
   {
      if ( symbolListWidget->isVisible() )
      {
         if ( symbolListWidget->currentRow() > 0 )
         {
            QCoreApplication::sendEvent(symbolListWidget,event);
            event->accept();
         }
      }
      else
      {
         QPlainTextEdit::keyPressEvent(event);
      }
   }
   else if ( (event->key() == Qt::Key_Down) || 
             (event->key() == Qt::Key_PageDown) )
   {
      if ( symbolListWidget->isVisible() )
      {
         if ( symbolListWidget->currentRow() < symbolListWidget->count()-1 )
         {
            QCoreApplication::sendEvent(symbolListWidget,event);
            event->accept();
         }
      }
      else
      {
         QPlainTextEdit::keyPressEvent(event);
      }
   }
   else if ( (event->key() != Qt::Key_Tab) &&
             (event->key() != Qt::Key_Shift) )
   {
      symbolListWidget->hide();
      QPlainTextEdit::keyPressEvent(event);
   }
   
   if ( !symbolListWidget->isHidden() )
   {
      if ( event->key() == Qt::Key_Tab )
      {
         cursor.movePosition(QTextCursor::WordLeft);
         cursor.selectionStart();
         cursor.movePosition(QTextCursor::WordRight,QTextCursor::KeepAnchor);
         cursor.selectionEnd();
         cursor.removeSelectedText();
         insertPlainText(symbolListWidget->currentItem()->text().toAscii().constData());
         event->accept();
         symbolListWidget->hide();
      }
   }
   else
   {
      if ( event->key() == Qt::Key_Tab )
      {
         QPlainTextEdit::keyPressEvent(event);
      }
   }
}

void CSyntaxHighlightedTextEdit::mouseMoveEvent(QMouseEvent *event)
{
   QPoint toolTipPos = event->pos();
   
   toolTipPos.setX(toolTipPos.x()-lineNumberAreaWidth());
   
   QTextCursor cursor = cursorForPosition(toolTipPos);
   cursor.select(QTextCursor::WordUnderCursor);
   QString selection = cursor.selectedText();
   updateToolTip ( selection );
   event->accept();
}

void CSyntaxHighlightedTextEdit::mousePressEvent(QMouseEvent *event)
{
   symbolListWidget->hide();
   QPlainTextEdit::mousePressEvent(event);
}

void CSyntaxHighlightedTextEdit::focusOutEvent(QFocusEvent* event)
{
   symbolListWidget->hide();
   QPlainTextEdit::focusOutEvent(event);
}

void CSyntaxHighlightedTextEdit::resizeEvent(QResizeEvent* e)
{
   QPlainTextEdit::resizeEvent(e);

   QRect cr = contentsRect();
#ifndef Q_WS_MAC
   lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
#else
   lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(),
                                     cr.height() - this->horizontalScrollBar()->height()));
#endif
}

int CSyntaxHighlightedTextEdit::lineNumberAreaWidth()
{
   int digits = 1;
   int max = qMax(1, blockCount());

   while (max >= 10)
   {
      max /= 10;
      ++digits;
   }

   int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;

   return space + 16;
}

void CSyntaxHighlightedTextEdit::updateLineNumberAreaWidth(int /* newBlockCount */)
{
   setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CSyntaxHighlightedTextEdit::updateLineNumberArea(const QRect& rect, int dy)
{
   if (dy)
   {
      lineNumberArea->scroll(0, dy);
   }
   else
   {
      lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());
   }

   if (rect.contains(viewport()->rect()))
   {
      updateLineNumberAreaWidth(0);
   }
}

void CSyntaxHighlightedTextEdit::highlightCurrentLine()
{
   QList<QTextEdit::ExtraSelection> extraSelections;

   if (!isReadOnly())
   {
      QTextEdit::ExtraSelection selection;

      QColor lineColor = QColor(Qt::gray).lighter(150);

      selection.format.setBackground(lineColor);
      selection.format.setProperty(QTextFormat::FullWidthSelection, true);
      selection.cursor = textCursor();
      selection.cursor.clearSelection();
      extraSelections.append(selection);
   }

   setExtraSelections(extraSelections);
}

void CSyntaxHighlightedTextEdit::lineNumberAreaMouseDoubleClickEvent(QMouseEvent* event)
{
   QMouseEvent* newEvent;
   QPointF      pos;
   QPoint       globalPos;
   QPoint       adjust;
   
   pos = event->pos();
   globalPos = event->globalPos();
   
   adjust.setX(pos.x());
   adjust.setY(0);
   
   pos -= adjust;
   globalPos -= adjust;
   
   newEvent = event->createExtendedMouseEvent(event->type(),pos,globalPos,event->button(),event->buttons(),event->modifiers());
   
   mouseDoubleClickEvent(newEvent);
   
   event->ignore();
   
   delete newEvent;
}

void CSyntaxHighlightedTextEdit::lineNumberAreaMousePressEvent(QMouseEvent* event)
{
   QMouseEvent* newEvent;
   QPointF      pos;
   QPoint       globalPos;
   QPoint       adjust;
   
   pos = event->pos();
   globalPos = event->globalPos();
   
   adjust.setX(pos.x());
   adjust.setY(0);
   
   pos -= adjust;
   globalPos -= adjust;
   
   newEvent = event->createExtendedMouseEvent(event->type(),pos,globalPos,event->button(),event->buttons(),event->modifiers());
   
   mousePressEvent(newEvent);
   
   delete newEvent;
}

void CSyntaxHighlightedTextEdit::lineNumberAreaMouseReleaseEvent(QMouseEvent* event)
{
   QMouseEvent* newEvent;
   QPointF      pos;
   QPoint       globalPos;
   QPoint       adjust;
   
   pos = event->pos();
   globalPos = event->globalPos();
   
   adjust.setX(pos.x());
   adjust.setY(0);
   
   pos -= adjust;
   globalPos -= adjust;
   
   newEvent = event->createExtendedMouseEvent(event->type(),pos,globalPos,event->button(),event->buttons(),event->modifiers());
   
   mouseReleaseEvent(event);
   
   delete newEvent;
}

void CSyntaxHighlightedTextEdit::lineNumberAreaMouseMoveEvent(QMouseEvent* event)
{
   QMouseEvent* newEvent;
   QPointF      pos;
   QPoint       globalPos;
   QPoint       adjust;
   
   pos = event->pos();
   globalPos = event->globalPos();
   
   adjust.setX(pos.x());
   adjust.setY(0);
   
   pos -= adjust;
   globalPos -= adjust;
   
   newEvent = event->createExtendedMouseEvent(event->type(),pos,globalPos,event->button(),event->buttons(),event->modifiers());
   
   mouseMoveEvent(event);
   
   delete newEvent;
}

void CSyntaxHighlightedTextEdit::lineNumberAreaPaintEvent(QPaintEvent* event)
{
   CBreakpointInfo* pBreakpoints = nesGetBreakpointDatabase();
   int idx;
   QPainter painter(lineNumberArea);
   QTextBlock block = firstVisibleBlock();
   int blockNumber = block.blockNumber();
   int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
   int bottom = top + (int) blockBoundingRect(block).height();
   int32_t addr;
   int32_t absAddr;
   CMarker* markers = nesGetExecutionMarkerDatabase();
   MarkerSetInfo* pMarker;
   QString number;

   painter.fillRect(event->rect(), this->palette().background().color());

   while (block.isValid() && top <= event->rect().bottom())
   {
      if (block.isVisible() && bottom >= event->rect().top())
      {
         addr = pasm_get_source_addr_by_linenum_and_file(blockNumber+1,this->documentTitle().toAscii().constData());
      
         absAddr = pasm_get_source_absolute_addr_by_linenum_and_file(blockNumber+1,this->documentTitle().toAscii().constData());
            
         for ( idx = 0; idx < markers->GetNumMarkers(); idx++ )
         {
            pMarker = markers->GetMarker(idx);
   
            if ( (pMarker->state == eMarkerSet_Started) ||
                  (pMarker->state == eMarkerSet_Complete) )
            {
               if ( (absAddr >= pMarker->startAbsAddr) &&
                     (absAddr <= pMarker->endAbsAddr) )
               {
                  painter.fillRect(0,top,lineNumberArea->width(),fontMetrics().height(),QColor(pMarker->red,pMarker->green,pMarker->blue));
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
               if ( addr == nesGetCPUProgramCounterOfLastSync() )
               {
                  breakEnabledHitIcon->paint(&painter,0,top,16,fontMetrics().height());
               }
               else
               {
                  breakEnabledIcon->paint(&painter,0,top,16,fontMetrics().height());
               }
            }
            else if ( (!pBreakpoint->enabled) &&
                      (pBreakpoint->type == eBreakOnCPUExecution) &&
                      (pBreakpoint->item1 <= addr) &&
                      ((absAddr == -1) || (absAddr == pBreakpoint->item1Absolute)) &&
                      (pBreakpoint->item2 >= addr) )
            {
               if ( addr == nesGetCPUProgramCounterOfLastSync() )
               {
                  breakDisabledHitIcon->paint(&painter,0,top,16,fontMetrics().height());
               }
               else
               {
                  breakDisabledIcon->paint(&painter,0,top,16,fontMetrics().height());
               }
            }
         }
   
         if ( decorationEnabled )
         {
            if ( absAddr == nesGetAbsoluteAddressFromAddress(nesGetCPUProgramCounterOfLastSync()) )
            {
               executionIcon->paint(&painter,0,top,16,fontMetrics().height());
            }
         }
                  
         number = QString::number(blockNumber + 1);
         painter.setPen(Qt::black);
         painter.drawText(0, top, lineNumberAreaWidth() - 2, fontMetrics().height(),
                          Qt::AlignRight, number);
      }

      block = block.next();
      top = bottom;
      bottom = top + (int) blockBoundingRect(block).height();
      ++blockNumber;
   }
}

int CSyntaxHighlightedTextEdit::updateSymbolList(QString selectedText,QStringList* symbols)
{
   symbol_table* pSymbol;
   int idx;
   QString match;
   const char* opcodeText;
   int selected = -1;
   
   if ( selectedText.length() )
   {
      symbols->clear();
      
      // All opcode mnemonics are 3-characters
      if ( selectedText.length() <= 3 )
      {
         for ( idx = 0; idx < 256; idx++ )
         {
            opcodeText = OPCODECHECK(idx,selectedText.toAscii().constData());   
            if ( opcodeText )
            {
               if ( !(symbols->contains(opcodeText,Qt::CaseSensitive)) )
               {
                  if ( selected < 0 )
                  {
                     selected = symbols->count();
                  }
                  symbols->append(opcodeText);
               }
            }
         }
      }
      for ( idx = 0; idx < pasm_get_num_symbols(); idx++ )
      {   
         pSymbol = pasm_get_symbol_by_index(idx);
         match = pSymbol->symbol;
         if ( match.startsWith(selectedText,Qt::CaseInsensitive) )
         {
            if ( selected < 0 )
            {
               selected = symbols->count();
            }
            symbols->append(match);
         }
      }
   }
   return selected;
}

void CSyntaxHighlightedTextEdit::updateToolTip(QString selectedText)
{
   symbol_table* pSymbol;
   int symbol;
   int symbolType;
   QString toolTipText;
   const char* opcodeToolTipText;
   
   setToolTip("");
   
   opcodeToolTipText = OPCODEINFO(selectedText.toAscii().constData());
   if ( opcodeToolTipText )
   {
      toolTipText = opcodeToolTipText;
   }
   else
   {   
      for ( symbol = 0; symbol < pasm_get_num_symbols(); symbol++ )
      {   
         if ( selectedText == pasm_get_symbol_name_by_index(symbol) )
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
